/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_VIEW_AN2KVIEW_H__
#define __BE_VIEW_AN2KVIEW_H__

#include <set>
#include <string>
#include <vector>

#include <memory>

#include <be_finger_an2kminutiae_data_record.h>
#include <be_framework_enumeration.h>
#include <be_memory_autobuffer.h>
#include <be_view_view.h>
#include <be_image_image.h>

/* an2k.h forward declares */
struct record;
typedef record RECORD;
struct ansi_nist;
typedef ansi_nist ANSI_NIST;

namespace BiometricEvaluation 
{
	namespace View
	{
		/**
		 * @brief
		 * A class to represent single biometric view and derived
		 * information.
		 *
		 * @details
		 * This abstraction represents the image and derived
		 * information taken from an ANSI/NIST record.
		 *
		 * For these types of records, the image resolution and
		 * scan resolution are identical. For compressed images,
		 * applications can compare the image resolution and size
		 * taken from the Type-3/4/5/6 record to that returned by
		 * the Image object directly.
		 */
		class AN2KView : public BiometricEvaluation::View::View {
		public:
			/** The type of AN2K record. */
			enum class RecordType : uint16_t
			{
				Type_1 = 1,
				Type_2 = 2,
				Type_3 = 3,
				Type_4 = 4,
				Type_5 = 5,
				Type_6 = 6,
				Type_7 = 7,
				Type_8 = 8,
				Type_9 = 9,
				Type_10 = 10,
				Type_11 = 11,
				Type_12 = 12,
				Type_13 = 13,
				Type_14 = 14,
				Type_15 = 15,
				Type_16 = 16,
				Type_17 = 17,
				Type_99 = 99
			};

			/**
			 * @brief
			 * The level of human monitoring for the image
			 * capture device.
			 */
			enum class DeviceMonitoringMode
			{
				/**
				 * Operator physically controls the
				 * subject to acquire biometric sample.
				 */
				Controlled,
				/**
				 * Person available to provide
				 * assistance to the subject submitting
				 * the biometric.
				 */
				Assisted,
				/**
				 * Person present to observe the
				 * operation of the device but provides
				 * no assistance.
				 */
				Observed,
				/**
				 * No one present to observe or
				 * provide assistance.
				 */
				Unattended,
				/** No information is known. */
				Unknown,
				/** Optional field -- not specified */
				NA
			};
			
			/** 
			 * @brief
			 * Convert a device monitoring mode indicator from
			 * an AN2K record.
			 *
			 * @param dmm
			 *	Item value for device monitoring mode from 
			 *	an AN2K record.
			 *
			 * @return
			 *	DeviceMonitoringMode representation of dmm.
			 *
			 * @throw Error::DataError
			 *	Invalid format of dmm.
			 */
			static DeviceMonitoringMode
			convertDeviceMonitoringMode(
			    const char *dmm);

			/**
			 * @brief
			 * Convert a compression algorithm indicator from
			 * an AN2K finger image record.
			 *
			 * @param recordType
			 *	The AN2K record type as an integer, allowing
			 *	the value taken directly from the AN2K record
			 *	or a RecordType::Kind to be passed in.
			 * @param an2kValue
			 *	Compression type data as read from an
			 *	AN2K record.
			 * @return
			 *	The compression algorithm.
			 * @throws Error::DataError
			 *	Invalid compression algorithm for record type.
			 * @throws Error::ParameterError
			 *	Invalid record type.
			 */
			static Image::CompressionAlgorithm
			    convertCompressionAlgorithm(
			    const uint16_t recordType,
			    const unsigned char *an2kValue);

			/**
			 * @brief
			 * Constants to define the minimum resolution used
			 * for fingerprint images in an AN2k record.
			 */
			static const double MinimumScanResolutionPPMM;
			static const double HalfMinimumScanResolutionPPMM;

			/**
			 * @brief
			 * The defined bit-depth for fixed-resolution images.
			*/
			static const int FixedResolutionBitDepth = 8;

			/**
			 * @brief
			 * Construct an AN2K view from a file.
			 * @details
			 * The file must contain the entire AN2K record, not
			 * just the image and other view-related records.
			 */
			AN2KView(
			    const std::string filename,
			    const RecordType typeID,
			    const uint32_t recordNumber);

			/**
			 * @brief
			 * Construct an AN2K view from a buffer.
			 * @details
			 * The buffer must contain the entire AN2K record, not
			 * just the image and other view-related records.
			 */
			AN2KView(
			    Memory::uint8Array &buf,
			    const RecordType typeID,
			    const uint32_t recordNumber);

			~AN2KView();

			/**
			 * @brief
			 * Obtain the set of minutiae records.
			 * @details
			 * Each AN2KViewVariableResolution may have more than
			 * one associated Type-9 record and each Type-9 record
			 * may have more than one minutiae format.
			 * @return
			 * A vector of minutiae data records.
		 	 */
			std::vector<Finger::AN2KMinutiaeDataRecord>
			getMinutiaeDataRecordSet() const;

			/**
			 * @brief
			 * Obtain the ANSI-NIST record type.
			 * @return
			 * The type of record used to construct this object.
			 */
			RecordType getRecordType() const;

		protected:

			/**
			 * @brief
			 * Obtain the complete ANSI/NIST record set.
			 */
			Memory::AutoBuffer<ANSI_NIST>
			getAN2K()
			    const;

			/**
			 * @brief
			 * Obtain a pointer to the single ANSI/NIST record.
			 * @details
			 * Child classes use this method to obtain a pointer
			 * to the specific ANSI/NIST record that was searched
			 * for by this class object.
			 */
			RECORD*
			getAN2KRecord()
			    const;

		private:

			/**
			 * @brief
			 * Read the common image information from an AN2K file.
			 * @details
			 * For Type-3/4/5/6/13/14 image records, read the 
			 * fields that are present in all of these types of
			 * records.
			 * This method is called during object construction
			 * and guarantees that the AN2KView common data is
			 * present and the RECORD pointer is set, else an
			 * exception is thrown.
			 * @param[in] an2k
			 *	The AN2K record.
			 * @throw ParameterError
			 *	The record parameter is nullptr.
			 * @throw DataError
			 *	The AN2K record has invalid or missing data.
			 */
			void readImageCommon(
			    const ANSI_NIST *an2k,
			    const RecordType typeID,
			    const uint32_t recordNumber);

			/**
			 * @brief
			 * Create AN2KMinutiaeDataRecord objects that share
			 * the IDC of this View.
			 * 
			 * @param buf
			 *	AN2K file in memory.
			 */
			void
			associateMinutiaeData(
			    Memory::uint8Array &buf);
			    
			/**
			 * @brief
			 * Create AN2KMinutiaeDataRecord objects that share
			 * the IDC of this View.
			 * 
			 * @param filename
			 *	Filename of an AN2K file.
			 */
    			void
			associateMinutiaeData(
			    const std::string &filename);
			    
    			/**
			 * @brief
			 * Mutator for the AN2KMinutiaeDataRecord set.
			 *
			 * @param mdr
			 *	AN2KMinutiaeDataRecord to associate with this
			 *	View.
			 */
			void
			addMinutiaeDataRecord(
			    Finger::AN2KMinutiaeDataRecord &mdr);
			    
			/* The record that this object represents. The Nth
			 * record is searched for when the object is
			 * constructed and may be referenced by subclasses.
			 */
			Memory::AutoBuffer<ANSI_NIST> _an2k;
			RECORD *_an2kRecord;
			RecordType _recordType;
			int _idc;
			
			/** 
			 * Collection of AN2KMinutiaeDataRecords that share
			 * this View's IDC.
			 */
			std::vector<Finger::AN2KMinutiaeDataRecord>
			    _minutiaeDataRecordSet;
		};
		
		/**
		 * @brief
		 * Output stream overload for DeviceMonitoringMode
		 */
		std::ostream&
		operator<<(
		    std::ostream &stream,
		    const AN2KView::DeviceMonitoringMode &kind);
	}
}

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::View::AN2KView::RecordType,
    BE_View_AN2KView_RecordType_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::View::AN2KView::DeviceMonitoringMode,
    BE_View_AN2KView_DeviceMonitoringMode_EnumToStringMap);

#endif /* __BE_VIEW_AN2KVIEW_H__ */

