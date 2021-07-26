/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_IMAGE_JPEG2000__
#define __BE_IMAGE_JPEG2000__

#include <be_image_image.h>

namespace BiometricEvaluation
{
	namespace Image
	{
		/**
		 * @brief
		 * A JPEG-2000-encoded image.
		 */
		class JPEG2000 : public Image
		{
		public:
			/**
			 * @brief
			 * Create a new JPEG2000 object.
			 *
			 * @param[in] data
			 *	The image data.
			 * @param[in] size
			 *	The size of the image data, in bytes.
			 * @param statusCallback
			 * Function to handle statuses sent when processing
			 * images.
			 * @param[in] codec
			 *	The OPJ_CODEC_FORMAT used to encode data.
			 *
			 * @throw Error::DataError
			 *	Error manipulating data.
			 * @throw Error::StrategyError
			 *	Error while creating Image.
			 */
			JPEG2000(
			    const uint8_t *data,
			    const uint64_t size,
			    const std::string &identifier = "",
			    const statusCallback_t &statusCallback =
			        Image::defaultStatusCallback,
			    const int8_t codecFormat = 2);

			JPEG2000(
			    const Memory::uint8Array &data,
			    const std::string &identifier = "",
			    const statusCallback_t &statusCallback =
			        Image::defaultStatusCallback);

			~JPEG2000() = default;

			Memory::uint8Array
			getRawData()
			    const;

			Memory::uint8Array
			getRawGrayscaleData(
			    uint8_t depth) const;

			/**
			 * Whether or not data is a JPEG-2000 image.
			 *
			 * @param[in] data
			 *	The buffer to check.
			 * @param[in] size
			 *	The size of data.
			 *
			 * @return
			 *	true if data appears to be a JPEG-2000 image,
			 *	false otherwise.
			 */
			static bool
			isJPEG2000(
			    const uint8_t *data,
			    uint64_t size);

		private:
			/** JPEG2000 codec to use (from libopenjpeg) */
			const int8_t _codecFormat;

			/**
			 * @brief
			 * Parse CDEF box to check for an opacity component.
			 *
			 * @return
			 * true if there is a separate opacity component. false
			 * otherwise.
			 *
			 * @throw Error::ObjectDoesNotExist
			 * No CDEF box.
			 */
			bool
			checkForAlphaInCDEF();

			/**
			 * @brief
			 * Callback for error output from libopenjpeg.
			 *
			 * @param msg
			 * Error from libopenjpeg
			 * @param client_data
			 * Ignored by JPEG2000. We use this as a pointer to
			 * the JPEG2000 object emitting the error.
			 */
			static void
			openjpeg_error(
			    const char *msg,
			    void *client_data);

			/**
			 * @brief
			 * Callback for warning output from libopenjpeg.
			 *
			 * @param msg
			 * Error from libopenjpeg
			 * @param client_data
			 * Ignored by JPEG2000. We use this as a pointer to
			 * the JPEG2000 object emitting the warning.
			 */
			static void
			openjpeg_warning(
			    const char *msg,
			    void *client_data);

			/**
			 * @brief
			 * Callback for info output from libopenjpeg.
			 *
			 * @param msg
			 * Error from libopenjpeg
			 * @param client_data
			 * Ignored by JPEG2000. We use this as a pointer to
			 * the JPEG2000 object emitting the info.
			 */
			static void
			openjpeg_info(
			    const char *msg,
			    void *client_data);

			/**
			 * @brief
			 * Return the value for a JPEG-2000 marker.
			 *
			 * @param[in] marker
			 * The marker to search for.
			 * @param[in] marker_size
			 * The length of marker in bytes.
			 * @param[in] buffer
			 * The JPEG-2000 buffer in which to search.
			 * @param[in] buffer_size
			 * The length of buffer in bytes.
			 *
			 * @return
			 * 0-based offset of the start of the marker.
			 *
			 * @throw Error::ObjectDoesNotExist
			 * marker not found within buffer.
			 */
			uint64_t
			static find_marker_offset(
			    const uint8_t *marker,
			    uint64_t marker_size,
			    const uint8_t *buffer,
			    uint64_t buffer_size);

			/**
			 * @brief
			 * Return the value for a JPEG-2000 marker.
			 *
			 * @param[in] marker
			 *	The marker to search for.
			 * @param[in] marker_size
			 *	The length of marker in bytes.
			 * @param[in] buffer
			 *	The JPEG-2000 buffer in which to search.
			 * @param[in] buffer_size
			 *	The length of buffer in bytes.
			 * @param[in] value_size
			 *	The size of the contents of the box indicated
			 *	by marker, in bytes.
			 *
			 * @return
			 *	An AutoArray of size value_size with the
			 *	contents of the box indicated by marker.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	marker not found within buffer.
			 */
			Memory::AutoArray<uint8_t>
			static find_marker(
			    const uint8_t *marker,
			    uint64_t marker_size,
			    const uint8_t *buffer,
			    uint64_t buffer_size,
			    uint64_t value_size);

			/**
			 * @brief
			 * Parse display resolution information from the
			 * resc or resd markers.
			 *
			 * @param res
			 *	AutoArray containing the contents of the
			 *	resc/resd box.
			 *
			 * @return
			 *	Resolution struct as parsed from the contents of
			 *	the resc/resd box.
			 *
			 * @throw Error::DataError
			 *	Format of the box was not as expected.
			 *
			 * @see find_marker()
			 */
			Resolution
			parse_res(
			    const Memory::AutoArray<uint8_t> &res);

			/*
			 * libopenjp2 stream callbacks.
			 *
			 * Note that libopenjp2 types have been converted to
			 * void* below so that openjpeg.h does not need to
			 * be in the include path of applications that use this
			 * file.
			 */

			/** @return libopenjp2 IO decompression stream. */
			void*
			getDecompressionStream()
			    const;

			/** @return libopenjp2 decompression codec. */
			void*
			getDecompressionCodec()
			    const;

			/*
			 * libopenjp2 callbacks.
 			 *
			 * Note that libopenjp2 types have been converted
			 * to standard types below so that openjpeg.h
			 * does not need to be in the include path of
			 * applications that use this file.
			 */

			/**
			 * @brief
			 * libopenjp2 callback to free data wrapped in stream.
			 *
			 * @param p_user_data
			 * Pointer to a Memory::IndexedBuffer.
			 */
			static void
			libopenjp2Free(
			    void *p_user_data);

			/**
			 * @brief
			 * libopenjp2 callback to read from a stream.
			 *
			 * @param p_buffer
			 * Buffer that read data should be placed in.
			 * @param p_nb_bytes
			 * Number of bytes requested
			 * @param p_user_data
			 * Pointer to a Memory::IndexedBuffer.
			 *
			 * @return
			 * Actual number of bytes placed into p_buffer.
			 */
			static size_t
			libopenjp2Read(
			    void *p_buffer,
			    size_t p_nb_bytes,
			    void *p_user_data);

			/**
			 * @brief
			 * libopenjp2 callback to skip data in stream.
			 *
			 * @param p_nb_bytes
			 * Number of bytes to skip.
			 * @param p_user_data
			 * Pointer to a Memory::IndexedBuffer.
			 *
			 * @return
			 * Actual number of bytes skipped.
			 */
			static int64_t
			libopenjp2Skip(
			    int64_t p_nb_bytes,
			    void *p_user_data);

			/**
			 * @brief
			 * libopenjp2 callback to determine if stream can seek.
			 *
			 * @param p_nb_bytes
			 * Number of bytes to seek.
			 * @param p_user_data
			 * Pointer to a Memory::IndexedBuffer.
			 *
			 * @return
			 * OPJ_TRUE if can seek p_nb_bytes, OPJ_FALSE otherwise.
			 */
			static int
			libopenjp2Seek(
			    int64_t p_nb_bytes,
			    void *p_user_data);
		};
	}
}

#endif /* __BE_IMAGE_JPEG2000__ */

