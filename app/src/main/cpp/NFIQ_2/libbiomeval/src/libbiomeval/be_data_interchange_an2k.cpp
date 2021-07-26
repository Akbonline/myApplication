/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <array>
#include <cstdio>
#include <set>

#include <be_data_interchange_an2k.h>
extern "C" {
#include <an2k.h>
}

namespace BE = BiometricEvaluation;

/**
 * @brief
 * Fixed resolution types.
 * @description
 * Types supported by AN2K::Finger::AN2KViewFixedResolution.
 *
 * @note
 * EFS treats Type_7 has Fixed Resolution (latent) because this existed before
 * Type 13. However, because this is DataInterchange::AN2K, not
 * DataInterchange::EFS, we are not parsing that value here.
 */
static const std::array<BiometricEvaluation::View::AN2KView::RecordType, 4>
    FingerFixedResolutionTypes = {
    BiometricEvaluation::View::AN2KView::RecordType::Type_3,
    BiometricEvaluation::View::AN2KView::RecordType::Type_4,
    BiometricEvaluation::View::AN2KView::RecordType::Type_5,
    BiometricEvaluation::View::AN2KView::RecordType::Type_6};

/******************************************************************************/
/* Private functions.                                                         */
/******************************************************************************/
std::set<int>
BiometricEvaluation::DataInterchange::AN2KRecord::recordLocations(
    Memory::uint8Array &buf,
    View::AN2KView::RecordType recordType)
{
	Memory::AutoBuffer<ANSI_NIST> an2k(&biomeval_nbis_alloc_ANSI_NIST,
	    &biomeval_nbis_free_ANSI_NIST, &biomeval_nbis_copy_ANSI_NIST);
	AN2KBDB bdb;
        INIT_AN2KBDB(&bdb, buf, buf.size());
	if (biomeval_nbis_scan_ANSI_NIST(&bdb, an2k) != 0)
		throw Error::DataError("Could not read AN2K buffer");

	return (recordLocations(an2k, recordType));
}

std::set<int>
BiometricEvaluation::DataInterchange::AN2KRecord::recordLocations(
    const ANSI_NIST *an2k,
    View::AN2KView::RecordType recordType)
{
	std::set<int> locations;
	for (int i = 0; i < an2k->num_records; i++)
		if (an2k->records[i]->type == static_cast<
		    std::underlying_type<
		    View::AN2KView::RecordType>::type>(recordType))
			locations.insert(i);

	return (locations);
}

void
BiometricEvaluation::DataInterchange::AN2KRecord::readType1Record(
    Memory::uint8Array &buf)
{
	Memory::AutoBuffer<ANSI_NIST> an2k(&biomeval_nbis_alloc_ANSI_NIST,
	    &biomeval_nbis_free_ANSI_NIST, &biomeval_nbis_copy_ANSI_NIST);
	AN2KBDB bdb;
        INIT_AN2KBDB(&bdb, buf, buf.size());
	if (biomeval_nbis_scan_ANSI_NIST(&bdb, an2k) != 0)
		throw Error::DataError("Could not read AN2K buffer");

	/* The Type-1 record is always first, but check anyway. */
	RECORD *rec;
	rec = an2k->records[0];
	if (rec->type != TYPE_1_ID)
		throw Error::DataError("Invalid AN2K Record");

	/*
	 * Retrieve the mandatory information from the Type-1 record.
	 * If a required field is not present, then the object string
	 * will be empty, but applications might want to open a record
	 * that is non-conformant.
	 */
	FIELD *field;
	int field_idx;

	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, VER_ID, rec) == TRUE)
		_version = (char *)field->subfields[0]->items[0]->value;

	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, DAT_ID, rec) == TRUE)
		_date = (char *)field->subfields[0]->items[0]->value;

	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, DAI_ID, rec) == TRUE)
		_dai = (char *)field->subfields[0]->items[0]->value;

	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, ORI_ID, rec) == TRUE)
		_ori = (char *)field->subfields[0]->items[0]->value;

	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, TCN_ID, rec) == TRUE)
		_tcn = (char *)field->subfields[0]->items[0]->value;

	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, NSR_ID, rec) == TRUE)
		_nsr = (char *)field->subfields[0]->items[0]->value;

	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, NTR_ID, rec) == TRUE)
		_ntr = (char *)field->subfields[0]->items[0]->value;

	/*
	 * Optional fields.
	 */
	/* Priority */
	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, PRY_ID, rec) == TRUE)
		_pry = atoi((char *)field->subfields[0]->items[0]->value);
	else
		_pry = 0;
	/* Transaction control reference */
	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, TCR_ID, rec) == TRUE)
		_tcr = (char *)field->subfields[0]->items[0]->value;
	else
		_tcr = "";
	/* Domain Name*/
	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, DOM_ID, rec) == TRUE) {
		switch (field->subfields[0]->num_items) {
		case 2:	/* Version and identifier present */
			_domainName.version = 
			    (char *)field->subfields[0]->items[1]->value;
			/* FALLTHROUGH */
		case 1:	/* Only identifier present */
			_domainName.identifier = 
			    (char *)field->subfields[0]->items[0]->value;
			break;
		default:
			throw Error::DataError("Invalid number of items in "
			    "field DOM");
		}
	}
	/* Greenwich Mean Time */
	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, GMT_ID, rec) == TRUE) {
		std::string gmt = (char *)field->subfields[0]->items[0]->value;
		if (gmt.length() != 15)
			throw Error::DataError("Field GMT has invalid length");
		_gmt.tm_year = atoi(gmt.substr(0, 4).c_str());
		_gmt.tm_mon = atoi(gmt.substr(4, 2).c_str());
		_gmt.tm_mday = atoi(gmt.substr(6, 2).c_str());
		_gmt.tm_hour = atoi(gmt.substr(8, 2).c_str());
		_gmt.tm_min = atoi(gmt.substr(10, 2).c_str());
		_gmt.tm_sec = atoi(gmt.substr(12, 2).c_str());
	}
	/* Directory of character sets */
	if (biomeval_nbis_lookup_ANSI_NIST_field(&field, &field_idx, DCS_ID, rec) == TRUE) {
		for (int i = 0; i < field->num_subfields; i++) {
			CharacterSet cs;
			switch (field->subfields[i]->num_items) {
			case 3:
				cs.version = (char *)field->subfields[i]->
				    items[2]->value;
				/* FALLTHROUGH */
			case 2:
				cs.commonName = (char *)field->subfields[i]->
				    items[1]->value;
				cs.identifier = atoi((char *)field->
				    subfields[i]->items[0]->value);
				break;
			default:
				throw Error::DataError("Invalid number of "
				    "items in field DCS");
			}
			_dcs.push_back(cs);
		}
	}
}

void
BiometricEvaluation::DataInterchange::AN2KRecord::readPalmCaptures(
    Memory::uint8Array &buf)
{
	int i{1};
	while (true) {
		try {
			this->_palmCaptures.emplace_back(buf, i);
		} catch (const Error::DataError&) {
			break;
		}
		++i;
	}
}

void
BiometricEvaluation::DataInterchange::AN2KRecord::readFingerCaptures(
    Memory::uint8Array &buf)
{
	int i = 1;
	while(true) {
		try {
			BE::Finger::AN2KViewCapture an2kv(buf, i);
			_fingerCaptures.push_back(an2kv);
		} catch (Error::DataError &e) {
			break;
		}
		i++;
	}
}

void
BiometricEvaluation::DataInterchange::AN2KRecord::readFingerLatents(
    Memory::uint8Array &buf)
{
	int i = 1;
	while(true) {
		try {
			BE::Latent::AN2KView an2kv(buf, i);
			_fingerLatents.push_back(an2kv);
		} catch (Error::DataError &e) {
			break;
		}
		i++;
	}
}

void
BiometricEvaluation::DataInterchange::AN2KRecord::readFixedResolutionCaptures(
    Memory::uint8Array &buf)
{
	for (const auto type : FingerFixedResolutionTypes) {
		for (int i{1}; ; ++i) {
			try {
				this->_fingerFixedResolutionCaptures[type].
				    emplace_back(buf, type, i);
			} catch (const Error::DataError&) {
				break;
			}
		}
	}
}

void
BiometricEvaluation::DataInterchange::AN2KRecord::readMinutiaeData(
    Memory::uint8Array &buf)
{
	std::set<int> loc = recordLocations(
	    buf, View::AN2KView::RecordType::Type_9);
	for (std::set<int>::const_iterator it = loc.begin();
	    it != loc.end(); it++) {
		try {
			_minutiaeDataRecordSet.push_back(
			    BE::Finger::AN2KMinutiaeDataRecord(buf, *it));
		} catch (Error::DataError &e) {
			break;
		}	
	}
}

/******************************************************************************/
/* Public functions.                                                          */
/******************************************************************************/
BiometricEvaluation::DataInterchange::AN2KRecord::AN2KRecord(
    const std::string filename)
{
	if (!IO::Utility::fileExists(filename))
		throw Error::FileError("File not found.");

	FILE *fp = std::fopen(filename.c_str(), "rb");
	if (fp == nullptr)
                throw (Error::FileError("Could not open file."));

	uint64_t sz = IO::Utility::getFileSize(filename);
	Memory::uint8Array buf(sz);
	if (fread(buf, 1, sz, fp) != sz) {
		fclose(fp);
		throw Error::FileError("Could not read AN2K file");
	}
	fclose(fp);

	readAN2KRecord(buf);
}

BiometricEvaluation::DataInterchange::AN2KRecord::AN2KRecord(
    Memory::uint8Array &buf)
{
	readAN2KRecord(buf);
}

void
BiometricEvaluation::DataInterchange::AN2KRecord::readAN2KRecord(
    Memory::uint8Array &buf)
{
	readType1Record(buf);
	readMinutiaeData(buf);
	readFingerCaptures(buf);
	readFingerLatents(buf);
	readFixedResolutionCaptures(buf);
	readPalmCaptures(buf);
}

std::string
BiometricEvaluation::DataInterchange::AN2KRecord::getDate() const
{
	return (_date);
}

std::string
BiometricEvaluation::DataInterchange::AN2KRecord::getVersionNumber() const
{
	return (_version);
}

std::string
BiometricEvaluation::DataInterchange::AN2KRecord::getDestinationAgency() const
{
	return (_dai);
}

std::string
BiometricEvaluation::DataInterchange::AN2KRecord::getOriginatingAgency() const
{
	return (_ori);
}

std::string
BiometricEvaluation::DataInterchange::AN2KRecord::getTransactionControlNumber() const
{
	return (_tcn);
}

std::string
BiometricEvaluation::DataInterchange::AN2KRecord::getNativeScanningResolution() const
{
	return (_nsr);
}

std::string
BiometricEvaluation::DataInterchange::AN2KRecord::getNominalTransmittingResolution() const
{
	return (_ntr);
}

uint32_t
BiometricEvaluation::DataInterchange::AN2KRecord::getFingerLatentCount() const
{
	return (_fingerLatents.size());
}

std::vector<BE::Finger::AN2KMinutiaeDataRecord>
BiometricEvaluation::DataInterchange::AN2KRecord::getMinutiaeDataRecordSet()
    const
{
	return (_minutiaeDataRecordSet);
}

std::vector<BE::Latent::AN2KView>
BiometricEvaluation::DataInterchange::AN2KRecord::getFingerLatents() const
{
	return (_fingerLatents);
}

uint32_t
BiometricEvaluation::DataInterchange::AN2KRecord::getFingerCaptureCount() const
{
	return (_fingerCaptures.size());
}

std::vector<BE::Finger::AN2KViewCapture>
BiometricEvaluation::DataInterchange::AN2KRecord::getFingerCaptures() const
{
	return (_fingerCaptures);
}

uint32_t
BiometricEvaluation::DataInterchange::AN2KRecord::
    getFingerFixedResolutionCaptureCount(
    const View::AN2KView::RecordType type)
    const
{
	try {
		return (static_cast<uint32_t>(
		    this->_fingerFixedResolutionCaptures.at(type).size()));
	} catch (const std::out_of_range&) {
		return (0);
	}
}

std::vector<BiometricEvaluation:: Finger::AN2KViewFixedResolution>
BiometricEvaluation::DataInterchange::AN2KRecord::
getFingerFixedResolutionCaptures(
    const View::AN2KView::RecordType type)
    const
{
	try {
		return (this->_fingerFixedResolutionCaptures.at(type));
	} catch (const std::out_of_range&) {
		return {};
	}
}

uint32_t
BiometricEvaluation::DataInterchange::AN2KRecord::
    getFingerFixedResolutionCaptureCount()
    const
{
	uint32_t counter{0};

	for (const auto type : FingerFixedResolutionTypes) {
		try {
			counter += static_cast<uint32_t>(
			    this->_fingerFixedResolutionCaptures.at(type).
			    size());
		} catch (const std::out_of_range&) {}
	}

	return (counter);
}

std::vector<BiometricEvaluation::Finger::AN2KViewFixedResolution>
BiometricEvaluation::DataInterchange::AN2KRecord::
    getFingerFixedResolutionCaptures()
    const
{
	std::vector<Finger::AN2KViewFixedResolution> captures{};
	captures.reserve(this->getFingerFixedResolutionCaptureCount());

	for (const auto type : FingerFixedResolutionTypes)
		captures.insert(captures.end(),
		    this->_fingerFixedResolutionCaptures.at(type).cbegin(),
		    this->_fingerFixedResolutionCaptures.at(type).cend());


	return (captures);
}

uint32_t
BiometricEvaluation::DataInterchange::AN2KRecord::getPalmCaptureCount()
   const
{
	return (this->_palmCaptures.size());
}

std::vector<BE::Palm::AN2KView>
BiometricEvaluation::DataInterchange::AN2KRecord::getPalmCaptures()
    const
{
	return (this->_palmCaptures);
}

uint8_t
BiometricEvaluation::DataInterchange::AN2KRecord::getPriority()
    const
{
	return (_pry);
}

BiometricEvaluation::DataInterchange::AN2KRecord::DomainName
BiometricEvaluation::DataInterchange::AN2KRecord::getDomainName()
    const
{
	return (_domainName);
}

struct tm
BiometricEvaluation::DataInterchange::AN2KRecord::getGreenwichMeanTime()
    const
{
	return (_gmt);
}

std::vector<BiometricEvaluation::DataInterchange::AN2KRecord::CharacterSet>
BiometricEvaluation::DataInterchange::AN2KRecord::getDirectoryOfCharacterSets()
    const
{
	return (_dcs);
}

bool
BiometricEvaluation::DataInterchange::AN2KRecord::isAN2KRecord(
    const std::string &filename)
{
	/*
	 * Do some checks, since failing these will cause liban2k to print to
	 * the console.
	 */
	if (!BE::IO::Utility::fileExists(filename))
		return (false);
	if (BE::IO::Utility::getFileSize(filename) < ((2 * FIELD_NUM_LEN) + 2))
		return (false);

	return (biomeval_nbis_is_ANSI_NIST_file(filename.c_str()) == TRUE);
}

bool
BiometricEvaluation::DataInterchange::AN2KRecord::isAN2KRecord(
    BiometricEvaluation::Memory::uint8Array &buf)
{
	return (biomeval_nbis_is_ANSI_NIST(buf, buf.size()) == TRUE);
}
