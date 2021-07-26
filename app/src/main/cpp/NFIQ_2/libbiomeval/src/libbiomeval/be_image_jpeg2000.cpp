/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <openjpeg.h>

#include <cmath>
#include <be_image_jpeg2000.h>
#include <be_memory_mutableindexedbuffer.h>

namespace BE = BiometricEvaluation;

/** 
 * @brief
 * Object with method to delete OpenJPEG codec objects.
 *
 * @details
 * Visual Studio 32-bit doesn't seem to allow function pointers as Deleters.
 */
struct OpenJPEG_CodecDeleter
{
	void
	operator()(
	    opj_codec_t *codec)
	    const
	    noexcept
	{
		opj_destroy_codec(codec);
	}
};

/**
 * @brief
 * Object with method to delete OpenJPEG image objects.
 *
 * @details
 * Visual Studio 32-bit doesn't seem to allow function pointers as Deleters.
 */
struct OpenJPEG_ImageDeleter
{
	void
	operator()(
	    opj_image_t *image)
	    const
	    noexcept
	{
		opj_image_destroy(image);
	}
};

/**
 * @brief
 * Object with method to delete OpenJPEG stream objects.
 *
 * @details
 * Visual Studio 32-bit doesn't seem to allow function pointers as Deleters.
 */
struct OpenJPEG_StreamDeleter
{
	void 
	operator()(
	    opj_stream_t *stream)
	    const
	    noexcept
	{
		opj_stream_destroy(stream);
	}
};

BiometricEvaluation::Image::JPEG2000::JPEG2000(
    const uint8_t *data,
    const uint64_t size,
    const std::string &identifier,
    const statusCallback_t &statusCallback,
    const int8_t codecFormat) :
    Image::Image(
    data,
    size,
    CompressionAlgorithm::JP2,
    identifier,
    statusCallback),
    _codecFormat(codecFormat)
{
	std::unique_ptr<opj_codec_t, OpenJPEG_CodecDeleter> codec(
	    static_cast<opj_codec_t*>(this->getDecompressionCodec()),
	    OpenJPEG_CodecDeleter{});
	std::unique_ptr<opj_stream_t, OpenJPEG_StreamDeleter> stream(
	    static_cast<opj_stream_t*>(this->getDecompressionStream()),
	    OpenJPEG_StreamDeleter{});

	opj_image_t *imagePtr = nullptr;
	if (opj_read_header(stream.get(), codec.get(), &imagePtr) == OPJ_FALSE)
		throw Error::Exception("Could not read header");
	if (imagePtr == nullptr)
		throw Error::Exception("Parsed header is empty");
	std::unique_ptr<opj_image_t, OpenJPEG_ImageDeleter> image(
	    imagePtr, OpenJPEG_ImageDeleter{});

	if (image->numcomps <= 0)
		throw Error::NotImplemented("No components");

	if ((image->color_space != OPJ_CLRSPC_SRGB) &&
	    (image->color_space != OPJ_CLRSPC_GRAY) &&
	    (image->color_space != OPJ_CLRSPC_UNSPECIFIED))
		throw Error::NotImplemented("Colorspace " +
		    std::to_string(image->color_space));

	/*
	 * Assign Image class instance variables.
	 */

	this->setDimensions(Size(image->x1, image->y1));

	/* Color depth */
	const uint32_t prec = image->comps[0].prec;
	for (uint32_t component = 1; component < image->numcomps; ++component)
		if (image->comps[component].prec != prec)
			throw Error::NotImplemented("Non-equivalent component "
			    "bit depths");
	setColorDepth(image->numcomps * prec);
	this->setBitDepth(prec);

	/* Resolution */
	static const uint8_t resc[4] = { 0x72, 0x65, 0x73, 0x63 };
	static const uint8_t resc_box_size = 10;
	/* The Capture Resolution Box is optional under some codecs */
	try {
		setResolution(parse_res(find_marker(resc, 4,
		    this->getDataPointer(), this->getDataSize(),
		    resc_box_size)));
	} catch (const Error::ObjectDoesNotExist&) {
		setResolution(Resolution(72, 72, Resolution::Units::PPI));
	}

	/*
	 * Component definition optional, but appears to only be present when
	 * not Grayscale or RGB (such as RGBA).
	 */
	try {
		this->setHasAlphaChannel(checkForAlphaInCDEF());
	} catch (BE::Error::Exception &e) {
		/* Take best guess on alpha channel presence */
		this->setHasAlphaChannel((
		    (image->color_space == OPJ_CLRSPC_GRAY) &&
		    (image->numcomps == 2)) ||
		    ((image->color_space == OPJ_CLRSPC_SRGB) &&
		    (image->numcomps == 4)) ||
		    ((image->color_space == OPJ_CLRSPC_UNSPECIFIED) &&
		    (image->numcomps == 2)) ||
		    ((image->color_space == OPJ_CLRSPC_UNSPECIFIED) &&
		    (image->numcomps == 4)));
	}
}

BiometricEvaluation::Image::JPEG2000::JPEG2000(
    const BiometricEvaluation::Memory::uint8Array &data,
    const std::string &identifier,
    const statusCallback_t &statusCallback) :
    BiometricEvaluation::Image::JPEG2000::JPEG2000(
    data,
    data.size(),
    identifier,
    statusCallback)
{

}

BiometricEvaluation::Memory::uint8Array
BiometricEvaluation::Image::JPEG2000::getRawData()
    const
{
	std::unique_ptr<opj_codec_t, OpenJPEG_CodecDeleter> codec(
	    static_cast<opj_codec_t*>(this->getDecompressionCodec()),
	    OpenJPEG_CodecDeleter{});
	std::unique_ptr<opj_stream_t, OpenJPEG_StreamDeleter> stream(
	    static_cast<opj_stream_t*>(this->getDecompressionStream()),
	    OpenJPEG_StreamDeleter{});

	opj_image_t *imagePtr = nullptr;
	if (opj_read_header(stream.get(), codec.get(), &imagePtr) == OPJ_FALSE)
		throw Error::Exception("Could not read header");
	if (imagePtr == nullptr)
		throw Error::Exception("Parsed header is empty");
	std::unique_ptr<opj_image_t, OpenJPEG_ImageDeleter> image(
	    imagePtr, OpenJPEG_ImageDeleter{});

	if (image->numcomps <= 0)
		throw Error::NotImplemented("No components");
	if (image->comps[0].sgnd == 1)
		throw Error::NotImplemented("Signed buffers");

	if (opj_decode(codec.get(), stream.get(), image.get()) == OPJ_FALSE)
		throw Error::StrategyError("Could not initialize decoding");

	const uint32_t w = this->getDimensions().xSize;
	const uint32_t h = this->getDimensions().ySize;
	const uint8_t bpc = image->comps[0].prec;

	std::vector<int32_t*> ptr;
	for (uint32_t i = 0; i < image->numcomps; ++i) {
		ptr.push_back(image->comps[i].data);
		if ((image->comps[i].w != w) || (image->comps[i].h != h) ||
		    (image->comps[i].prec != bpc))
			throw Error::NotImplemented("Non-equal components");
	}

	Memory::uint8Array rawData(image->numcomps * (bpc / 8) * image->x1 *
	    image->y1);
	Memory::MutableIndexedBuffer buffer(rawData);

	const int32_t mask = (1 << image->comps[0].prec) - 1;
	for (uint32_t row = 0; row < h; ++row) {
		for (uint32_t col = 0; col < w; ++col) {
			if (bpc <= 8) {
				for (uint32_t i = 0; i < image->numcomps; ++i) {
					buffer.pushU8Val(*ptr[i] & mask);
					ptr[i]++;
				}
			} else if (bpc <= 16) {
				for (uint32_t i = 0; i < image->numcomps; ++i) {
					buffer.pushU16Val(*ptr[i] & mask);
					ptr[i]++;
				}
			} else {
				throw Error::NotImplemented(
				    std::to_string(bpc) +
				    "-bit-per-component images");
			}
		}
	}

	return (rawData);
}

BiometricEvaluation::Memory::uint8Array
BiometricEvaluation::Image::JPEG2000::getRawGrayscaleData(
    uint8_t depth)
    const
{
	return (Image::getRawGrayscaleData(depth));
}

bool
BiometricEvaluation::Image::JPEG2000::isJPEG2000(
    const uint8_t *data,
    uint64_t size)
{
	static const uint8_t SOC_size = 12;
	static const uint8_t SOC[SOC_size] = {
	    0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50,
	    0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A
	};
	if (size < SOC_size)
		return (false);

	return (memcmp(data, SOC, SOC_size) == 0);
}

void
BiometricEvaluation::Image::JPEG2000::openjpeg_error(
    const char *msg,
    void *client_data)
{
	if (client_data != nullptr) {
		const JPEG2000 *jp2 = static_cast<const JPEG2000*>(client_data);
		jp2->getStatusCallback()({Framework::Status::Type::Error,
		    msg, jp2->getIdentifier()});
	}

	/* We can't continue on errors, so if handler won't throw, we will. */
	throw Error::StrategyError(msg);
}

void
BiometricEvaluation::Image::JPEG2000::openjpeg_warning(
    const char *msg,
    void *client_data)
{
	if (client_data == nullptr)
		return;

	const JPEG2000 *jp2 = static_cast<const JPEG2000*>(client_data);
	jp2->getStatusCallback()({Framework::Status::Type::Warning, msg,
	    jp2->getIdentifier()});
}

void
BiometricEvaluation::Image::JPEG2000::openjpeg_info(
    const char *msg,
    void *client_data)
{
	if (client_data == nullptr)
		return;

	const JPEG2000 *jp2 = static_cast<const JPEG2000*>(client_data);
	jp2->getStatusCallback()({Framework::Status::Type::Debug, msg,
	    jp2->getIdentifier()});
}

bool
BiometricEvaluation::Image::JPEG2000::checkForAlphaInCDEF()
{
	static constexpr uint8_t cdef[4] = { 0x63, 0x64, 0x65, 0x66 };
	static constexpr uint8_t cdefTagSize{4};

	const uint64_t offset{find_marker_offset(cdef, cdefTagSize,
	    this->getDataPointer(), this->getDataSize())};

	BE::Memory::IndexedBuffer ib(this->getDataPointer(),
	    this->getDataSize());
	ib.scan(nullptr, offset + cdefTagSize);

	volatile uint16_t typ{};
	uint16_t numCDEF{ib.scanBeU16Val()};
	for (uint16_t c{0}; c < numCDEF; ++c) {
		typ = ib.scanBeU16Val(); /* cn */

		typ = ib.scanBeU16Val();
		if (typ == 1)
			return (true);

		typ = ib.scanBeU16Val(); /* asoc */
	}

	return (false);
}

uint64_t
BiometricEvaluation::Image::JPEG2000::find_marker_offset(
    const uint8_t *marker,
    uint64_t marker_size,
    const uint8_t *buffer,
    uint64_t buffer_size)
{
	uint64_t step;
	const uint64_t max{buffer_size - marker_size};
	for (step = 0; step < max; ++step) {
		if (memcmp(buffer, marker, marker_size))
			++buffer;
		else
			break;
	}

	if (step >= max)
		throw BE::Error::ObjectDoesNotExist();

	return (step);
}

BiometricEvaluation::Memory::AutoArray<uint8_t>
BiometricEvaluation::Image::JPEG2000::find_marker(
    const uint8_t *marker,
    uint64_t marker_size,
    const uint8_t *buffer,
    uint64_t buffer_size,
    uint64_t value_size)
{
	uint64_t step{find_marker_offset(marker, marker_size, buffer,
	    buffer_size)};
	if ((step + marker_size + value_size) > buffer_size)
		throw Error::ObjectDoesNotExist();

	Memory::AutoArray<uint8_t> ret(value_size);
	ret.copy(buffer + step + marker_size, value_size);
	return (ret);
}

BiometricEvaluation::Image::Resolution
BiometricEvaluation::Image::JPEG2000::parse_res(
    const BiometricEvaluation::Memory::AutoArray<uint8_t> &res)
{
	/* Sanity check */
	if (res.size() != 10)
		throw Error::DataError("Invalid size for Resolution Box");

	Memory::IndexedBuffer ib(res);

	/* I.7.3.6.1: Capture resolution box */
	uint16_t VR_N = ib.scanBeU16Val();
	uint16_t VR_D = ib.scanBeU16Val();
	uint16_t HR_N = ib.scanBeU16Val();
	uint16_t HR_D = ib.scanBeU16Val();
	int8_t VR_E = static_cast<int8_t>(ib.scanU8Val());
	int8_t HR_E = static_cast<int8_t>(ib.scanU8Val());

	return (Resolution(
	    ((static_cast<float>(VR_N) / VR_D) * pow(10.0, VR_E)) / 100.0,
	    ((static_cast<float>(HR_N) / HR_D) * pow(10.0, HR_E)) / 100.0,
	    Resolution::Units::PPCM));
}

void*
BiometricEvaluation::Image::JPEG2000::getDecompressionCodec()
    const
{
	opj_codec_t *codec = nullptr;
	switch (this->_codecFormat) {
	case OPJ_CODEC_J2K:	/* JPEG-2000 codestream (.J2K) */
		/* FALLTHROUGH */
	case OPJ_CODEC_JP2:	/* JPEG-2000 compressed image data (.JP2) */
		/* FALLTHROUGH */
	case OPJ_CODEC_JPT:	/* JPEG 2000, JPIP (.JPT) */
		codec = opj_create_decompress(static_cast<OPJ_CODEC_FORMAT>(
		    this->_codecFormat));
		break;
	case OPJ_CODEC_UNKNOWN:
		/* FALLTHROUGH */
	default:
		throw Error::StrategyError("Unsupported decoding format: " +
		    std::to_string(this->_codecFormat));
		break;
	}

	/* libopenjpg2 error callbacks */
	opj_set_error_handler(codec, openjpeg_error, (void *)this);
	opj_set_warning_handler(codec, openjpeg_warning, (void *)this);
	opj_set_info_handler(codec, openjpeg_info, (void *)this);

	/* Use default decoding parameters, except codec, which is "unknown" */
	opj_dparameters parameters;
	opj_set_default_decoder_parameters(&parameters);
	parameters.decod_format = this->_codecFormat;
	if (opj_setup_decoder(codec, &parameters) == OPJ_FALSE) {
		opj_destroy_codec(codec);
		throw Error::StrategyError("Could not initialize decoding");
	}

	return (codec);
}

void*
BiometricEvaluation::Image::JPEG2000::getDecompressionStream()
    const
{
	auto stream = opj_stream_default_create(OPJ_TRUE);

	Memory::IndexedBuffer *ib = new Memory::IndexedBuffer(
	    this->getDataPointer(), this->getDataSize());
	opj_stream_set_user_data(stream, ib, libopenjp2Free);
	opj_stream_set_user_data_length(stream, ib->getSize());

	opj_stream_set_read_function(stream, libopenjp2Read);
	opj_stream_set_seek_function(stream, libopenjp2Seek);
	opj_stream_set_skip_function(stream, libopenjp2Skip);

	return (stream);
}

/*
 * libopenjp2 stream IO callbacks.
 */

void
BiometricEvaluation::Image::JPEG2000::libopenjp2Free(
    void *p_user_data)
{
	delete static_cast<Memory::IndexedBuffer *>(p_user_data);
}

OPJ_SIZE_T
BiometricEvaluation::Image::JPEG2000::libopenjp2Read(
    void *p_buffer,
    OPJ_SIZE_T p_nb_bytes,
    void *p_user_data)
{
	Memory::IndexedBuffer *ib = static_cast<Memory::IndexedBuffer *>(
	    p_user_data);

	OPJ_SIZE_T actualScanSize = static_cast<OPJ_SIZE_T>(std::fmin(
	    p_nb_bytes, ib->getSize() - ib->getIndex()));
	try {
		return (ib->scan(p_buffer, actualScanSize));
	} catch (Error::Exception &e) {
		return (0);
	}
}

OPJ_OFF_T
BiometricEvaluation::Image::JPEG2000::libopenjp2Skip(
    OPJ_OFF_T p_nb_bytes,
    void *p_user_data)
{
	Memory::IndexedBuffer *ib = static_cast<Memory::IndexedBuffer *>(
	    p_user_data);

	OPJ_SIZE_T actualSkipSize = static_cast<OPJ_SIZE_T>(std::fmin(
	    p_nb_bytes, ib->getSize() - ib->getIndex()));

	try {
		return (ib->scan(nullptr, actualSkipSize));
	} catch (Error::Exception &e) {
		return (0);
	}
}

OPJ_BOOL
BiometricEvaluation::Image::JPEG2000::libopenjp2Seek(
    OPJ_OFF_T p_nb_bytes,
    void *p_user_data)
{
	Memory::IndexedBuffer *ib = static_cast<Memory::IndexedBuffer *>(
	    p_user_data);

	try {
		ib->setIndex(p_nb_bytes);
		return (OPJ_TRUE);
	} catch (const BE::Error::ParameterError&) {
		return (OPJ_FALSE);
	}
}
