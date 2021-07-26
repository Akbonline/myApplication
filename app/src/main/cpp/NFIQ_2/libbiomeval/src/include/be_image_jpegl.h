/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_IMAGE_JPEGL__
#define __BE_IMAGE_JPEGL__

#include <be_image_image.h>

namespace BiometricEvaluation
{
	namespace Image
	{
		/**
		 * @brief
		 * A Lossless JPEG-encoded image.
		 */
		class JPEGL : public Image
		{
		public:
			JPEGL(
			    const uint8_t *data,
			    const uint64_t size,
			    const std::string &identifier = "",
			    const statusCallback_t &statusCallback =
			        Image::defaultStatusCallback);

			JPEGL(
			    const Memory::uint8Array &data,
			    const std::string &identifier = "",
			    const statusCallback_t &statusCallback =
			        Image::defaultStatusCallback);

			~JPEGL() = default;

			Memory::uint8Array
			getRawGrayscaleData(
			    uint8_t depth) const;

			Memory::uint8Array
			getRawData()
			    const;

			/**
			 * Whether or not data is a Lossless JPEG image.
			 *
			 * @param[in] data
			 *	The buffer to check.
			 * @param[in] size
			 *	The size of data.
			 *
			 * @return
			 *	true if data appears to be a Lossless JPEG
			 *	image, false otherwise.
			 */
			static bool
			isJPEGL(
			    const uint8_t *data,
			    uint64_t size);

		protected:

		private:

		};
	}
}

#endif /* __BE_IMAGE_JPEGL__ */

