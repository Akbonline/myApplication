/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_FEATURE_AN2K11EFS_H__
#define __BE_FEATURE_AN2K11EFS_H__

#include <be_image.h>
#include <be_finger.h>
#include <be_palm.h>
#include <be_plantar.h>
#include <be_feature.h>
#include <be_feature_minutiae.h>
#include <be_framework_enumeration.h>
#include <be_memory_autoarray.h>

namespace BiometricEvaluation 
{
	namespace Feature {
	namespace AN2K11EFS {

		/**
		 * @brief
		 * Representation of orientation (deviation from upright)
		 * and its uncertainty.
		 */
		struct Orientation {
			/** Interpretation of encoded orientation values. */
			enum class EncodingMethod {
				/** No orientation was encoded */
				Default,
				/**
				 * Encoded value indicates orientation was not
				 * determined.
				 */
				Indeterminate,
				/** Value was encoded */
				UserDefined
			};

			/** ANSI/NIST default direction */
			static const int	EODDefault = 0;
			/** ANSI/NIST default uncertainty */
			static const int	EUCDefault = 15;
			/** ANSI/NIST indeterminate uncertainty */
			static const int	EUCIndeterminate = 180;

			/** Interpretation of encoded values. */
			EncodingMethod encodingMethod;

			/** Direction */
			int	eod;
			bool	has_euc;
			/** Uncertainty */
			int	euc;
		};
		std::ostream& operator<< (std::ostream&, const Orientation&);

		/**
		 * @brief
		 * The finger segment positions.
		 */
		enum class FingerprintSegment {
			PRX = 0,
			DST,
			MED,
			UNK
		};

		/**
		 * @brief
		 * The finger segment positions.
		 */
		enum class OffCenterFingerPosition {
			T = 0,
			R,
			L
		};
		using OCF = OffCenterFingerPosition;

		/**
		 * @brief
		 * Representation of finger-palm-plantar position.
		 * @details
		 * Contains one or more possible physical positions that
		 * correspond to the region of interest. Clients of this
		 * structure must check the fgp value to determine which
		 * of the position codes (Finger/Palm/Plantar) applies.
		 */
		struct FPPPosition {
			/** The friction ridge generalized position */
			Feature::FGP			fgp;

			bool				has_fsm;
			/** The finger segment position */
			FingerprintSegment		fsm;

			bool				has_ocf;
			/** The off-center fingerprint position */
			OffCenterFingerPosition		ocf;

			bool				has_sgp;
			/** The segment polygon */
			BiometricEvaluation::Image::CoordinateSet sgp;
			
		};
		std::ostream& operator<< (std::ostream&, const FPPPosition&);

		/**
		 * @brief
		 * Representation of tonal reversal.
		 */
		enum class TonalReversal {
			/**
			 * Negative, ridges are light and valleys are dark
			 * throughout the image.
			 */
			N = 0,
			/**
			 * Partial, ridges are light and valleys are dark
			 * only in portions of the image.
			 */
			P,
			/**
			 * Unknown
			 */
			U
		};

		/**
		 * @brief
		 * Representation of possible latent reversal information.
		 */
		enum class LateralReversal {
			/** Image is known to be laterally reversed. */
			L = 0,
			/** Image may be be laterally reversed. */
			U
		};

		/**
		 * @brief
		 * A structure representing information about the image and
		 * extended feature set region.
		 */
		struct ImageInfo {
			/**
			 * The region of interest: A mandatory field.
			 */
			BiometricEvaluation::Image::ROI	roi;

			/**
			 * The Finger/Palm/Plantar Position: Mandatory field.
			 */
			FPPPosition			fpp;

			/**
			 * The image orientation. Optional but always present
			 * due to default value.
			 */
			Orientation			ort;

			bool				has_trv;
			/**
			 * The tonal reversal information. Optional.
			 */
			TonalReversal			trv;

			bool				has_plr;
			/**
			 * The possible latent reversal information. Optional.
			 */
			LateralReversal			plr;
		};
		std::ostream& operator<< (std::ostream&,
		    const ImageInfo&);
 
		/**
		 * @brief
		 * Representation of an extended feature set minutia
		 * data point.
		 */
		struct MinutiaPoint : public Feature::MinutiaPoint {
			bool	has_mru;
			/** radius of position uncertainty */
			int	mru;
			bool	has_mdu;
			/** minutiae direction uncertainty */
			int	mdu;
		};
		std::ostream& operator<< (std::ostream&,
		    const MinutiaPoint&);
		using MinutiaPointSet = std::vector<MinutiaPoint>;

		/**
		 * @brief
		 * Representation of an extended feature set ridge count info.
		 */
		struct MinutiaeRidgeCount {
			/** minutia index A */
			int	mia;
			/** minutia index B */
			int	mib;
			/** ridge count */
			int	mir;
			bool	has_mrn;
			/** reference number, optional */
			int	mrn;
			bool	has_mrs;
			/** residual, optional */
			int	mrs;
		};
		std::ostream& operator<< (std::ostream&,
		    const MinutiaeRidgeCount&);
		using MinutiaeRidgeCountSet =
		    std::vector<MinutiaeRidgeCount>;

		/**
		 * @brief
		 * The type of EFS ridge count method.
		 */
		enum class MethodOfRidgeCounting {
			/** Auto */
			A = 0,
			/** Manual tracing */
			T,
			/** Manual ridge count */
			M
		};
		using MORC = MethodOfRidgeCounting;

		/**
		 * @brief
		 * Representation of an extended feature set minutiae ridge
		 * count confidence item.
		 */
		struct MinutiaeRidgeCountConfidence {
			Image::Coordinate	pointA;
			Image::Coordinate	pointB;
			MethodOfRidgeCounting	morc;
			int			mcv;
		};
		using MRCC = struct MinutiaeRidgeCountConfidence;
		std::ostream& operator<< (std::ostream&,
		    const MinutiaeRidgeCountConfidence&);
		using MinutiaeRidgeCountConfidenceSet =
		    std::vector<MinutiaeRidgeCountConfidence>;

		/**
		 * @brief
		 * The type of EFS ridge count algorithm.
		 */
		enum class MinutiaeRidgeCountAlgorithm {
			OCTANT = 0,
			EFTS7,
			QUADRANT
		};
		using MRA = MinutiaeRidgeCountAlgorithm;

		/**
		 * @brief
		 * All the ridge count information in one place.
		 */
		struct MinutiaeRidgeCountInfo {
			bool				has_mra;
			MinutiaeRidgeCountAlgorithm	mra;
			bool				has_mrcs;
			MinutiaeRidgeCountSet		mrcs;
			bool				has_rccs;
			MinutiaeRidgeCountConfidenceSet	rccs;
		};
		std::ostream& operator<< (std::ostream&,
		    const MinutiaeRidgeCountInfo&);

		 /*
		 * @brief
		 * Representation of an extended feature set core.
		 */
		struct CorePoint {
			Image::Coordinate	location;
			bool			has_cdi;
			int			cdi;
			bool			has_rpu;
			int			rpu;
			bool			has_duy;
			int			duy;
		};
		std::ostream& operator<< (std::ostream&, const CorePoint&);
		using CorePointSet = std::vector<CorePoint>;

		/**
		 * @brief
		 * The type of extended feature set delta.
		 */
		enum class DeltaType {
			/** Left fingerprint delta */
			L,
			/** Right fingerprint delta */
			R,
			/** Unknown finger */
			I00,
			/** Right index */
			I02,
			/** Right middle */
			I03,
			/** Right ring */
			I04,
			/** Right little */
			I05,
			/** Left index */
			I07,
			/** Left middle */
			I08,
			/** Left ring */
			I09,
			/** Left little */
			I10,
			/** Right extra digit */
			I16,
			/** Left extra digit */
			I17,
			/** Carpal */
			C,
			/** Other */
			Other
		};

		/**
		 * @brief
		 * Representation of an extended feature set delta.
		 */
		struct DeltaPoint {
			Image::Coordinate	location;
			bool		has_dup;
			int		dup;
			bool		has_dlf;
			int		dlf;
			bool		has_drt;
			int		drt;
			bool		has_dtp;
			DeltaType	dtp;
			bool		has_rpu;
			int		rpu;
			bool		has_duu;
			int		duu;
			bool		has_dul;
			int		dul;
			bool		has_dur;
			int		dur;
		};
		using DeltaPointSet = std::vector<DeltaPoint>;
		std::ostream& operator<< (std::ostream&, const DeltaPoint&);

		/**
		 * @brief
		 * A set of flags indicating "No features present" indicators
		 * contained within the extended feature set.
		 * @details
		 * A flag is set to true when the Type-9 field is set to 'Y',
		 * indicating that analysis of the image has determined that
		 * there are no instances of that feature present in the image.
		 * Otherwise the Type-9 field is is not present and the flag
		 * will be false.
		 */
		struct NoFeaturesPresent {
			bool	cores;
			bool	deltas;
			bool	minutiae;
		};
		std::ostream& operator<< (
		     std::ostream&, const NoFeaturesPresent&);

		/** Representation of latent processing method (9.352) */
		enum class LatentProcessingMethod
		{
			I12,
			ADX,
			ALS,
			AMB,
			AY7,
			BAR,
			BLE,
			BLP,
			BPA,
			BRY,
			CBB,
			CDS,
			COG,
			DAB,
			DFO,
			FLP,
			GEN,
			GRP,
			GTV,
			HCA,
			IOD,
			ISR,
			LAS,
			LCV,
			LIQ,
			LQD,
			MBD,
			MBP,
			MGP,
			MPD,
			MRM,
			NIN,
			OTH,
			PDV,
			R6G,
			RAM,
			RUV,
			SAO,
			SDB,
			SGF,
			SPR,
			SSP,
			SVN,
			TEC,
			TID,
			VIS,
			WHP,
			ZIC
		};
		using LPM = LatentProcessingMethod;

		/** Code indicating the value of a print */
		enum class ValueAssessmentCode
		{
			Value,
			ValueForIndividualization = Value,
			VID = Value,
			Limited,
			ValueForExclusionOnly = Limited,
			VEO = Limited,
			NoValue,
			NV = NoValue,
			NonPrint
		};

		/** Examiner's assessment of an impression */
		struct ExaminerAnalysisAssessment
		{
			/** Whether this field was present */
			bool present{false};

			/** Value of impression (required) */
			ValueAssessmentCode aav;
			/** Examiner's surname (required) */
			std::string aln;
			/** Examiner's first and middle names (required) */
			std::string afn;
			/** Examiner's employer/affiliation (required) */
			std::string aaf;
			/** Date and time determination made (GMT, required) */
			std::string amt;
			/** Comment (optional) */
			std::string acm{};
			/** Whether `cxf` is populated (required) */
			bool has_cxf{false};
			/** Whether analysis was complex (optional) */
			bool cxf{};
		};
		std::ostream&
		operator<<(
		     std::ostream&,
		     const ExaminerAnalysisAssessment&);

		/** Substrates on which latent impression was deposited */
		enum class SubstrateCode
		{
			Paper,
			Cardboard,
			UnfinishedWood,
			OtherOrUnknownPorous,

			Plastic,
			Glass,
			PaintedMetal,
			UnpaintedMetal,
			GlossyPaintedSurface,
			AdhesiveSideTape,
			NonAdhesiveSideTape,
			AluminumFoil,
			OtherOrUnknownNonporous,

			Rubber,
			Leather,
			EmulsionSidePhotograph,
			PaperSidePhotograph,
			GlossyOrSemiglossyPaperOrCardboard,
			SatinOrFlatFinishedPaintedSurface,
			OtherOrUnknownSemiporous,

			Other,
			Unknown
		};

		/** Description of surface on which latent was deposited */
		struct Substrate
		{
			/** Whether this field was present */
			bool present{false};

			/** Type of substrate (required) */
			SubstrateCode cls{SubstrateCode::Unknown};
			/** Description and/or clarification (optional) */
			std::string osd{};
		};
		std::ostream&
		operator<<(
		     std::ostream&,
		     const Substrate&);

		/** Fingerprint classification. */
		struct Pattern
		{
			/**
			 * @brief
			 * General pattern classification
			 *
			 * @seealso
			 * BiometricEvaluation::Finger::PatternClassification
			 */
			enum class GeneralClassification
			{
				Arch,
				Whorl,
				RightSlantLoop,
				LeftSlantLoop,
				Amputation,
				TemporarilyUnavailable,
				Unclassifiable,
				Scar,
				DissociatedRidges
			};

			/* Details subclassification for arches */
			enum class ArchSubclassification
			{
				Plain,
				Tented
			};

			/* Details subclassification for whorls */
			enum class WhorlSubclassification
			{
				Plain,
				CentralPocketLoop,
				DoubleLoop,
				Accidental
			};

			/** Relationship between multiple deltas in a whorl */
			enum class WhorlDeltaRelationship
			{
				Inner,
				Outer,
				Meeting
			};

			bool present{false};

			GeneralClassification general;

			bool hasSubclass{false};
			union
			{
				ArchSubclassification arch;
				WhorlSubclassification whorl;
			} subclass;

			bool hasWhorlDeltaRelationship{false};
			WhorlDeltaRelationship whorlDeltaRelationship;
		};
		std::ostream&
		operator<<(
		     std::ostream&,
		     const Pattern&);

		/**
		 * @brief
		 * A class to represent the Extended Feature Set optionally
		 * present in an ANSI/NIST Type-9 record.
		 * @details
		 * Each minutiae point, ridge count item, core, and delta
		 * is represented in the native ANSI/NIST format.
		 * Conforms with ANSI/NIST-ITL-2011: Update 2015 standard.
		 */
		class ExtendedFeatureSet {
		public:
			/**
			 * @brief
			 * Construct an AN2K11 EFS object from file data.
			 * @details
			 * The file contains a complete ANSI/NIST record,
			 * and an object of this class represents a single
			 * Type-9 extended feature set structure. 
			 *
			 * @param[in] filename
			 * 	The name of the file containing the complete
			 *	ANSI/NIST record.
			 * @param[in] recordNumber
			 *	Which fingerprint minutiae record to read
			 *	from the complete AN2K record.
			 *
			 * @throw Error::ObjectDoesNotExist
			 * 	The named file does not exist.
			 * @throw Error::StrategyError
			 * 	An error occurred when opening or reading
			 *	from the file.
			 * @throw Error::DataError
			 *	An error occurred reading the AN2K record,
			 *	or there is no fingerprint minutiae record
			 *	for the requested number.
			 */
			ExtendedFeatureSet(
			    const std::string &filename,
			    int recordNumber);

			/**
			 * @brief
			 * Construct an AN2K11 EFS object from data contained
			 * in a memory buffer.
			 * @details
			 * The buffer contains a complete ANSI/NIST record,
			 * and an object of this class represents a single
			 * Type-9 extended feature set structure. 
			 *
			 * @param[in] buf
			 * 	The memory buffer containing the complete
			 *	ANSI/NIST record.
			 * @param[in] recordNumber
			 *	Which fingerprint minutiae record to read
			 *	from the complete AN2K record.
			 * @throw Error::DataError
			 *	An error occurred reading the AN2K record,
			 *	or there is no fingerprint minutiae record
			 *	for the requested number.
			 */
			ExtendedFeatureSet(
			    Memory::uint8Array &buf,
			    int recordNumber);

			/**
			 * @brief
			 * Obtain the structure containing information about
			 * the image and Extended Feature Set.
			 * 
			 * @return
			 * The information about the image.
			 */
			ImageInfo
			getImageInfo()
			    const;

			/**
			 * @brief
			 * Obtain the minutiae point set.
			 * @details
			 * The set may be empty as this Type-9 field is
			 * optional.
			 * 
			 * @return
			 * The set of minutia points.
			 */
			BiometricEvaluation::Feature::AN2K11EFS::MinutiaPointSet
		 	getMPS()
		 	    const;

			/**
			 * @brief
			 * Obtain all the information relating to minutiae
			 * ridge count information.
			 * @details
			 * Some of the information may not be present for
			 * the optional fields in the AN2k11 extended feature
			 * set.
			 * 
			 * @return
			 * The minutiae ridge count information structure.
			 */
			BiometricEvaluation::Feature::AN2K11EFS::MinutiaeRidgeCountInfo
			getMRCI()
			    const;

			/**
			 * @brief
			 * Obtain the core point set.
			 * @details
			 * The set may be empty as this Type-9 field is
			 * optional.
			 * 
			 * @return
			 * The set of core points.
			 */
			BiometricEvaluation::Feature::AN2K11EFS::CorePointSet
		 	getCPS()
		 	    const;

			/**
			 * @brief
			 * Obtain the delta point set.
			 * @details
			 * The set may be empty as this Type-9 field is
			 * optional.
			 * 
			 * @return
			 * The set of delta points.
			 */
			BiometricEvaluation::Feature::AN2K11EFS::DeltaPointSet
		 	getDPS()
		 	    const;

		 	/**
			 * @brief
			 * Obtain set of methods used to process this latent.
			 * @details
			 * The set may be empty as this Type-9 field is
			 * optional.
			 *
			 * @return
			 * The set of latent processing methods.
			 */
			std::vector<LatentProcessingMethod>
		 	getLPM()
		 	    const;

			/**
			 * Obtain the No Features Present indicators.
			 * @return
			 * The flags for No Features Present.
			 */
			BiometricEvaluation::Feature::AN2K11EFS::NoFeaturesPresent
			getNFP()
			    const;

			/**
			 * @brief
			 * Obtain the examiner's analysis assessment of the
			 * print.
			 *
			 * @return
			 * Examiner's analysis assessment.
			 */
			ExaminerAnalysisAssessment
			getEAA()
			    const;

			/**
			 * @return
			 * Substrate/surface on which the impression was
			 * deposited.
			 */
			Substrate
			getLSB()
			    const;

			/**
			 * @return
			 * Collection of possible pattern classifications.
			 */
			std::vector<Pattern>
			getPAT()
			    const;

			~ExtendedFeatureSet();

		private:
			class Impl;
			std::unique_ptr<ExtendedFeatureSet::Impl> pimpl;
		};
	} /* Namespace AN2K11EFS */
	} /* Namespace Feature */
}

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::Orientation::EncodingMethod,
    BE_Feature_AN2K11EFS_Orientation_EncodingMethod_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::FingerprintSegment,
    BE_Feature_AN2K11EFS_FingerprintSegment_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::OCF,
    BE_Feature_AN2K11EFS_OCF_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::TonalReversal,
    BE_Feature_AN2K11EFS_TonalReversal_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::LateralReversal,
    BE_Feature_AN2K11EFS_LateralReversal_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::MethodOfRidgeCounting,
    BE_Feature_AN2K11EFS_MethodOfRidgeCounting_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::MRA,
    BE_Feature_AN2K11EFS_MRA_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::DeltaType,
    BE_Feature_AN2K11EFS_DeltaType_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::LatentProcessingMethod,
    BE_Feature_AN2K11EFS_LatentProcessingMethod_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::ValueAssessmentCode,
    BE_Feature_AN2K11EFS_ValueAssessmentCode_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::SubstrateCode,
    BE_Feature_AN2K11EFS_SubstrateCode_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::Pattern::GeneralClassification,
    BE_Feature_AN2K11EFS_Pattern_GeneralClassification_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::Pattern::ArchSubclassification,
    BE_Feature_AN2K11EFS_Pattern_ArchSubclassification_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::Pattern::WhorlSubclassification,
    BE_Feature_AN2K11EFS_Pattern_WhorlSubclassification_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Feature::AN2K11EFS::Pattern::WhorlDeltaRelationship,
    BE_Feature_AN2K11EFS_Pattern_WhorlDeltaRelationship_EnumToStringMap);

#endif /* __BE_FEATURE_AN2K11EFS_H__ */

