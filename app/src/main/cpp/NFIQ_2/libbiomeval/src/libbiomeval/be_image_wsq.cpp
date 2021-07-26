/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <cstdio>

extern "C" {
	#include <dataio.h>
	#include <wsq.h>
	int biomeval_nbis_debug = 0;	/* Required by libwsq */
}

#include <be_image_wsq.h>

BiometricEvaluation::Image::WSQ::WSQ(
    const uint8_t *data,
    const uint64_t size,
    const std::string &identifier,
    const statusCallback_t &statusCallback) :
    Image::Image(
    data,
    size,
    CompressionAlgorithm::WSQ20,
    identifier,
    statusCallback)
{
	uint8_t *marker_buf = (uint8_t *)this->getDataPointer();
	uint8_t *wsq_buf = marker_buf;

	/* Read to the "start of image" marker */
	uint16_t marker, tbl_size;
	uint32_t rv = 0;
	if ((rv = biomeval_nbis_getc_marker_wsq(&marker, SOI_WSQ, &marker_buf,
	    wsq_buf + size)))
		throw Error::StrategyError("Could not read to SOI_WSQ");

	/* Step through any tables up to the "start of frame" marker */
	for (;;) {
		if ((rv = biomeval_nbis_getc_marker_wsq(&marker, TBLS_N_SOF, &marker_buf,
		    wsq_buf + size)))
			throw Error::StrategyError("Could not read to "
			    "TBLS_N_SOF");

		if (marker == SOF_WSQ)
			break;

		if ((rv = biomeval_nbis_getc_ushort(&tbl_size, &marker_buf, wsq_buf + size)))
			throw Error::StrategyError("Could not read size "
			    "of table");
		/* Table size includes size of field but not the marker */
		marker_buf += tbl_size - sizeof(tbl_size);
	}

	/* Read the frame header */
	FRM_HEADER_WSQ wsq_header;
	if ((rv = biomeval_nbis_getc_frame_header_wsq(&wsq_header, &marker_buf,
	    wsq_buf + size)))
		throw Error::DataError("Could not read frame header");
	setDimensions(Size(wsq_header.width, wsq_header.height));

	/* Read PPI from NISTCOM, if present */
	int ppi{-1};
	if (biomeval_nbis_getc_ppi_wsq(&ppi, wsq_buf, size) == 0) {
		/* Resolution does not have to be defined */
		if (ppi == -1)
			/* WSQ is a 500 ppi specification */
			setResolution(Resolution(500, 500,
			    Resolution::Units::PPI));
		else
			setResolution(Resolution(ppi, ppi,
			    Resolution::Units::PPI));
	} else {
		/* WSQ is a 500 ppi specification */
		setResolution(Resolution(500, 500, Resolution::Units::PPI));
	}

	/*
	 * "Source fingerprint images shall be captured with 8 bits of
	 * precision per pixel."
	 */
	setColorDepth(8);
	this->setBitDepth(8);

	this->setHasAlphaChannel(false);
}

BiometricEvaluation::Image::WSQ::WSQ(
    const BiometricEvaluation::Memory::uint8Array &data,
    const std::string &identifier,
    const statusCallback_t &statusCallback) :
    BiometricEvaluation::Image::WSQ::WSQ(
    data,
    data.size(),
    identifier,
    statusCallback)
{

}

BiometricEvaluation::Memory::uint8Array
BiometricEvaluation::Image::WSQ::getRawData()
    const
{
	uint8_t *rawbuf = nullptr;
	int32_t depth, height, lossy, ppi, rv, width;
	if ((rv = biomeval_nbis_wsq_decode_mem(&rawbuf, &width, &height, &depth, &ppi,
	    &lossy, (unsigned char *)this->getDataPointer(),
	    this->getDataSize())))
		throw Error::DataError("Could not convert WSQ to raw.");

	/* rawbuf allocated within libwsq.  Copy to manage with AutoArray. */
	/* TODO: AutoBuffer-wrapped AutoArray */
	Memory::uint8Array rawData(width * height * (depth / 8));
	rawData.copy(rawbuf);
	free(rawbuf);

	return (rawData);
}

BiometricEvaluation::Memory::uint8Array
BiometricEvaluation::Image::WSQ::getRawGrayscaleData(
    uint8_t depth)
    const
{
	return (Image::getRawGrayscaleData(depth));
}

bool
BiometricEvaluation::Image::WSQ::isWSQ(
    const uint8_t *data,
    uint64_t size)
{
	static const uint8_t WSQ_SOI[2] = {0xFF, 0xA0};
	if (size < (sizeof(WSQ_SOI) / sizeof(uint8_t)))
		return (false);

	return (memcmp(data, WSQ_SOI, 2) == 0);
}
