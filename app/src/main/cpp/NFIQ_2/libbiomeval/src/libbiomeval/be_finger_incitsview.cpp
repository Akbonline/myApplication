/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <sstream>

#include <be_finger_incitsview.h>
#include <be_feature_incitsminutiae.h>
#include <be_io_utility.h>
#include <be_image_jpeg.h>
#include <be_image_jpeg2000.h>
#include <be_image_raw.h>
#include <be_image_wsq.h>
#include <be_image_png.h>

namespace BE = BiometricEvaluation;

BiometricEvaluation::Finger::INCITSView::INCITSView()
{
}

/*
 * The base constructor for all INCITS and ISO finger
 * minutiae records opens the file and reads the contents
 * into the object state _fmr and _fir. The parsing of the
 * data is done by the child classes with help from this
 * class.
 */
BiometricEvaluation::Finger::INCITSView::INCITSView(
    const std::string &fmrFilename,
    const std::string &firFilename,
    const uint32_t viewNumber)
{
	if (fmrFilename != "") {
		try {
			this->_fmr = BE::IO::Utility::readFile(fmrFilename);
		} catch (const BE::Error::Exception&) {
			throw (BE::Error::FileError(
			     "FMR file could not be opened."));
		}
	}
	if (firFilename != "") {
		try {
			this->_fir = BE::IO::Utility::readFile(firFilename);
		} catch (const BE::Error::Exception&) {
			throw (BE::Error::FileError(
			     "FIR file could not be opened."));
		}
	}
}

BiometricEvaluation::Finger::INCITSView::INCITSView(
    const Memory::uint8Array &fmrBuffer,
    const Memory::uint8Array &firBuffer,
    const uint32_t viewNumber) :
    _fmr(fmrBuffer),
    _fir(firBuffer)
{
}

/******************************************************************************/
/* Public functions.                                                          */
/******************************************************************************/
BiometricEvaluation::Finger::Position
BiometricEvaluation::Finger::INCITSView::convertPosition(int incitsFGP)
{
	switch (incitsFGP) {

	case 0: return (BE::Finger::Position::Unknown); break;
	case 1: return (BE::Finger::Position::RightThumb); break;
	case 2: return (BE::Finger::Position::RightIndex); break;
	case 3: return (BE::Finger::Position::RightMiddle); break;
	case 4: return (BE::Finger::Position::RightRing); break;
	case 5: return (BE::Finger::Position::RightLittle); break;
	case 6: return (BE::Finger::Position::LeftThumb); break;
	case 7: return (BE::Finger::Position::LeftIndex); break;
	case 8: return (BE::Finger::Position::LeftMiddle); break;
	case 9: return (BE::Finger::Position::LeftRing); break;
	case 10: return (BE::Finger::Position::LeftLittle); break;
	case 11: return (BE::Finger::Position::PlainRightThumb); break;
	case 12: return (BE::Finger::Position::PlainLeftThumb); break;
	case 13: return (BE::Finger::Position::PlainRightFourFingers); break;
	case 14: return (BE::Finger::Position::PlainLeftFourFingers); break;
	default:
		throw (Error::DataError("Invalid finger position code")); 
	}
}

BiometricEvaluation::Finger::Impression
BiometricEvaluation::Finger::INCITSView::convertImpression(int incitsIMP)
{
	switch (incitsIMP) {
	case 0: return (BE::Finger::Impression::LiveScanPlain);
	case 1: return (BE::Finger::Impression::LiveScanRolled);
	case 2: return (BE::Finger::Impression::NonLiveScanPlain);
	case 3: return (BE::Finger::Impression::NonLiveScanRolled);
	case 8: return (BE::Finger::Impression::LiveScanVerticalSwipe);
	case 9: return (BE::Finger::Impression::LiveScanOpticalContactlessPlain);
	default:
		throw (Error::DataError("Invalid impression type code")); 
	}
}

BiometricEvaluation::Feature::INCITSMinutiae
BiometricEvaluation::Finger::INCITSView::getMinutiaeData() const
{
	return (_minutiae);
}

BiometricEvaluation::Finger::Position
BiometricEvaluation::Finger::INCITSView::getPosition() const
{
	return (_position);
}

BiometricEvaluation::Finger::Impression
BiometricEvaluation::Finger::INCITSView::getImpressionType() const
{
	return (_impression);
}

uint32_t
BiometricEvaluation::Finger::INCITSView::getQuality() const
{
	return (_quality);
}

uint16_t
BiometricEvaluation::Finger::INCITSView::getCaptureEquipmentID() const
{
	return (_captureEquipmentID);
}

std::vector<uint8_t>
BiometricEvaluation::Finger::INCITSView::getMinutiaeReservedData()
    const
{
	return (this->_fmdReserved);
}

/******************************************************************************/
/* Local functions.                                                           */
/******************************************************************************/
static std::tuple<BiometricEvaluation::Feature::MinutiaPoint, uint8_t>
scanFMD(BiometricEvaluation::Memory::IndexedBuffer &buf)
{
	uint16_t sval;
	uint8_t cval;
	BE::Feature::MinutiaPoint m;

	sval = buf.scanBeU16Val();	

	m.has_type = true;
	uint8_t nativeType = ((sval & 
	    BE::Feature::INCITSMinutiae::FMD_MINUTIA_TYPE_MASK) >>
	    BE::Feature::INCITSMinutiae::FMD_MINUTIA_TYPE_SHIFT);

	switch (nativeType) {
	case BE::Feature::INCITSMinutiae::FMD_MINUTIA_TYPE_OTHER:
		m.type = BE::Feature::MinutiaeType::Other;
		break;
	case BE::Feature::INCITSMinutiae::FMD_MINUTIA_TYPE_RIDGE_ENDING:
		m.type = BE::Feature::MinutiaeType::RidgeEnding;
		break;
	case BE::Feature::INCITSMinutiae::FMD_MINUTIA_TYPE_BIFURCATION:
		m.type = BE::Feature::MinutiaeType::Bifurcation;
		break;
	default:
		throw (BE::Error::DataError("Invalid minutiae type."));
		break;
	}
	m.coordinate.x = sval & BE::Feature::INCITSMinutiae::FMD_X_COORD_MASK;
	sval = buf.scanBeU16Val();
	m.coordinate.y = sval & BE::Feature::INCITSMinutiae::FMD_Y_COORD_MASK;
	uint8_t reservedValue = sval &
	    BE::Feature::INCITSMinutiae::FMD_RESERVED_MASK;

	/* Angle and quality */
	cval = buf.scanU8Val();
	m.theta = cval;
	cval = buf.scanU8Val();
	m.has_quality = true;
	m.quality = cval;

	return (std::make_tuple(m, reservedValue));
}

static
BiometricEvaluation::Feature::RidgeCountItem scanRCD(
    BiometricEvaluation::Memory::IndexedBuffer &buf,
    uint8_t nativeExtrMethod)
{
	uint8_t idx1 = buf.scanU8Val();
	uint8_t idx2 = buf.scanU8Val();
	uint8_t	count = buf.scanU8Val();
	BE::Feature::RidgeCountExtractionMethod extrMethod;
	switch (nativeExtrMethod) {
	case BE::Feature::INCITSMinutiae::RCE_NONSPECIFIC:
		extrMethod = BE::Feature::RidgeCountExtractionMethod::NonSpecific;
		break;
	case BE::Feature::INCITSMinutiae::RCE_FOUR_NEIGHBOR:
		extrMethod = BE::Feature::RidgeCountExtractionMethod::FourNeighbor;
		break;
	case BE::Feature::INCITSMinutiae::RCE_EIGHT_NEIGHBOR:
		extrMethod = BE::Feature::RidgeCountExtractionMethod::EightNeighbor;
		break;
	default:
		throw (BE::Error::DataError(
		    "Invalid ridge count extraction method"));
		break;
	}
	BE::Feature::RidgeCountItem rcd(extrMethod, idx1, idx2, count);
	return (rcd);
}

/******************************************************************************/
/* Protected functions.                                                       */
/******************************************************************************/

BiometricEvaluation::Memory::uint8Array const&
BiometricEvaluation::Finger::INCITSView::getFMRData() const
{
	return (_fmr);
}

BiometricEvaluation::Memory::uint8Array const&
BiometricEvaluation::Finger::INCITSView::getFIRData() const
{
	return (_fir);
}

void
BiometricEvaluation::Finger::INCITSView::setMinutiaeData(
    const BiometricEvaluation::Feature::INCITSMinutiae &minutiae)
{
	_minutiae = minutiae;
}

void
BiometricEvaluation::Finger::INCITSView::setMinutiaeReservedData(
    const std::vector<uint8_t> &reservedBits)
{
	this->_fmdReserved = reservedBits;
}

void
BiometricEvaluation::Finger::INCITSView::setPosition(
    const Finger::Position &position)
{
	_position = position;
}

void
BiometricEvaluation::Finger::INCITSView::setImpressionType(
    const Finger::Impression &impression)
{
	_impression = impression;
}

void
BiometricEvaluation::Finger::INCITSView::setQuality(uint32_t quality)
{
	_quality = quality;
}

void
BiometricEvaluation::Finger::INCITSView::setViewNumber(uint32_t viewNumber)
{
	_viewNumber = viewNumber;
}

uint32_t
BiometricEvaluation::Finger::INCITSView::getViewNumber()
    const
{
	return (this->_viewNumber);
}

void
BiometricEvaluation::Finger::INCITSView::setCaptureEquipmentID(uint16_t id)
{
	_captureEquipmentID = id;
}

void
BiometricEvaluation::Finger::INCITSView::setCBEFFProductIDs(
    uint16_t owner, uint16_t type)
{
	_productIDOwner = owner;
	_productIDType = type;
}

void
BiometricEvaluation::Finger::INCITSView::setAppendixFCompliance(bool flag)
{
	_appendixFCompliance = flag;
}

uint32_t
BiometricEvaluation::Finger::INCITSView::getRecordLength()
    const
{
	return (this->_recordLength);
}

uint8_t
BiometricEvaluation::Finger::INCITSView::getNumFingerViews()
    const
{
	return (this->_numFingerViews);
}

uint8_t
BiometricEvaluation::Finger::INCITSView::getFMRReservedByte()
    const
{
	return (this->_fmrReservedByte);
}

uint16_t
BiometricEvaluation::Finger::INCITSView::getEDBLength()
    const
{
	return (this->_edbLength);
}

void
BiometricEvaluation::Finger::INCITSView::readFMRHeader(
    BiometricEvaluation::Memory::IndexedBuffer &buf,
    const uint32_t formatStandard)
{
	static const uint16_t HDR_SCANNER_ID_MASK = 0x0FFF;
	static const uint16_t HDR_COMPLIANCE_MASK = 0xF000;
	static const uint8_t HDR_COMPLIANCE_SHIFT = 12;

	if ((formatStandard != Finger::INCITSView::ANSI2004_STANDARD) &&
	    (formatStandard != Finger::INCITSView::ISO2005_STANDARD))
		throw (Error::ParameterError("Invalid standard parameter"));

	/* Record length, 2/4/6 bytes */
	if (formatStandard == Finger::INCITSView::ANSI2004_STANDARD) {
		this->_recordLength = buf.scanBeU16Val();
		if (this->_recordLength == 0) {
			this->_recordLength = buf.scanBeU32Val();
		}
	} else {
		this->_recordLength = buf.scanBeU32Val();
	}
	
	/* CBEFF Product ID */
	if (formatStandard == Finger::INCITSView::ANSI2004_STANDARD) {
		_productIDOwner = buf.scanBeU16Val();
		_productIDType = buf.scanBeU16Val();
	}

	/* Capture equipment compliance/scanner ID */
	uint16_t sval = buf.scanBeU16Val();
	_captureEquipmentID = sval & HDR_SCANNER_ID_MASK;
	_appendixFCompliance =
	    (sval & HDR_COMPLIANCE_MASK) >> HDR_COMPLIANCE_SHIFT;

	/* Image size and resolution */
	uint16_t xval, yval;
	xval = buf.scanBeU16Val();
	yval = buf.scanBeU16Val();
	this->setImageSize(Image::Size(xval, yval));
	xval = buf.scanBeU16Val();
	yval = buf.scanBeU16Val();
	auto resolution = BE::Image::Resolution(xval, yval,
	    BE::Image::Resolution::Units::PPCM);
	this->setImageResolution(resolution);
	setScanResolution(resolution);

	/* Number of views and reserved field */
	this->_numFingerViews = buf.scanU8Val();
	this->_fmrReservedByte = buf.scanU8Val();
}

void
BiometricEvaluation::Finger::INCITSView::readFVMR(
    BiometricEvaluation::Memory::IndexedBuffer &buf)
{
	static const uint8_t FVMR_VIEW_NUMBER_MASK = 0xF0;
	static const uint8_t FVMR_VIEW_NUMBER_SHIFT  = 4;
	static const uint8_t FVMR_IMPRESSION_MASK = 0x0F;

	uint8_t cval = buf.scanU8Val();
	_position = BE::Finger::INCITSView::convertPosition(cval);

	cval = buf.scanU8Val();
	_viewNumber = (cval & FVMR_VIEW_NUMBER_MASK) >> FVMR_VIEW_NUMBER_SHIFT;
	_impression = BE::Finger::INCITSView::convertImpression(
	    cval & FVMR_IMPRESSION_MASK);

	_quality = (uint32_t)buf.scanU8Val();

	/* Read the minutiae data items. */
	cval = buf.scanU8Val();		/* Number of minutiae */
	auto minutiaData = this->readMinutiaeDataPoints(buf, cval);
	this->_minutiae.setMinutiaPoints(std::get<0>(minutiaData));
	this->_fmdReserved = std::get<1>(minutiaData);
	this->readExtendedDataBlock(buf);
}

std::tuple<BiometricEvaluation::Feature::MinutiaPointSet, std::vector<uint8_t>>
BiometricEvaluation::Finger::INCITSView::readMinutiaeDataPoints(
    BiometricEvaluation::Memory::IndexedBuffer &buf,
    uint32_t count)
{
	BE::Feature::MinutiaPointSet mps(count);
	std::vector<uint8_t> reserved(count);
	for (uint32_t i = 0; i < count; i++) {
		std::tie(mps[i], reserved[i]) = scanFMD(buf);
		mps[i].index = i;
	}
	return (std::make_tuple(mps, reserved));
}

BiometricEvaluation::Feature::RidgeCountItemSet
BiometricEvaluation::Finger::INCITSView::readRidgeCountData(
    BiometricEvaluation::Memory::IndexedBuffer &buf,
    uint32_t dataLength)
{
	uint8_t nativeExtrMethod = buf.scanU8Val();
	int32_t remLength = dataLength -
	    BE::Feature::INCITSMinutiae::FED_HEADER_LENGTH - 1;
	if ((remLength % BE::Feature::INCITSMinutiae::FED_RCD_ITEM_LENGTH) != 0)
		throw (Error::DataError(
		    "Ridge count data block has bad length"));
	BE::Feature::RidgeCountItemSet rcis;
	while (remLength > 0) {
		rcis.push_back(scanRCD(buf, nativeExtrMethod));
		remLength -= BE::Feature::INCITSMinutiae::FED_RCD_ITEM_LENGTH;
	}
	return (rcis);
}

void
BiometricEvaluation::Finger::INCITSView::readExtendedDataBlock(
    BiometricEvaluation::Memory::IndexedBuffer &buf)
{
	/* Extended data block length */
	this->_edbLength = buf.scanBeU16Val();
	if (this->_edbLength == 0)
		return;

	int32_t blockLength = static_cast<int32_t>(this->_edbLength);
	while (blockLength > 0) {
		uint16_t typeID = buf.scanBeU16Val();
		uint16_t dataLength = buf.scanBeU16Val();
		if (dataLength == 0)
			throw (Error::DataError("Extended data length is 0"));
		if (dataLength > blockLength) {
			std::ostringstream sstr;
			sstr << "Extended data length " << dataLength <<
			    " is greater than remaining block length of " <<
			    blockLength;
			throw (Error::DataError(sstr.str()));
		}
		switch (typeID) {
		case BE::Feature::INCITSMinutiae::FED_RIDGE_COUNT:
		{
			BE::Feature::RidgeCountItemSet rcis =
			    this->readRidgeCountData(buf, dataLength);
			_minutiae.setRidgeCountItems(rcis);
		}
			break;

		case BE::Feature::INCITSMinutiae::FED_CORE_AND_DELTA:
		{
			BE::Feature::CorePointSet cps;
			BE::Feature::DeltaPointSet dps;
			this->readCoreDeltaData(buf, dataLength, cps, dps);
			_minutiae.setCorePointSet(cps);
			_minutiae.setDeltaPointSet(dps);
		}
			break;

		default:
			//XXX read an opaque object
			break;
		}
		blockLength -= dataLength;
	}
}
/******************************************************************************/
/* Private functions.                                                         */
/******************************************************************************/

