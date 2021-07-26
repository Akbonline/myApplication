/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_FINGER_H__
#define __BE_FINGER_H__

#include <iostream>
#include <map>
#include <vector>

#include <be_framework_enumeration.h>

namespace BiometricEvaluation
{
	/**
	 * @brief
	 * Biometric information relating to finger images and derived
	 * information.
	 * @details
	 * The Finger package gathers all finger related matters,
	 * including classes to represent finger minutiae and helper
	 * functions for conversion between biometric representations.
	 * Contained within this namespace are classes to represent specific
	 * record formats, such as ANSI/NIST finger image records.
	 */
	namespace Finger
	{
		/** Pattern classification codes. */
		enum class PatternClassification
		{
			PlainArch = 0,
			TentedArch,
			RadialLoop,
			UlnarLoop,
			PlainWhorl,
			CentralPocketLoop,
			DoubleLoop,
			AccidentalWhorl,
			Whorl,
			RightSlantLoop,
			LeftSlantLoop,
			Scar,
			Amputation,
			Unknown
		};

		/**
		 * @brief
		 * Finger position codes.
		 * @details
		 * These codes match those in ANSI/NIST. Other minutiae
		 * formats may have to map codes into this set.
		 */
		enum class Position
		{
			Unknown			= 0,
			RightThumb		= 1,
			RightIndex		= 2,
			RightMiddle		= 3,
			RightRing		= 4,
			RightLittle		= 5,
			LeftThumb		= 6,
			LeftIndex		= 7,
			LeftMiddle		= 8,
			LeftRing		= 9,
			LeftLittle		= 10,
			PlainRightThumb		= 11,
			PlainLeftThumb		= 12,
			PlainRightFourFingers	= 13,
			PlainLeftFourFingers	= 14,
			LeftRightThumbs		= 15,
			RightExtraDigit		= 16,
			LeftExtraDigit		= 17,
			UnknownFrictionRidge	= 18,
			EJI			= 19,
			RightIndexMiddle	= 40,
			RightMiddleRing		= 41,
			RightRingLittle		= 42,
			LeftIndexMiddle		= 43,
			LeftMiddleRing		= 44,
			LeftRingLittle		= 45,
			RightIndexLeftIndex	= 46,
			RightIndexMiddleRing	= 47,
			RightMiddleRingLittle	= 48,
			LeftIndexMiddleRing	= 49,
			LeftMiddleRingLittle	= 50,
			PlainRightFourTips	= 51,
			PlainLeftFourTips	= 52,
			PlainRightFiveTips	= 53,
			PlainLeftFiveTips	= 54,
		};
		using PositionSet = std::vector<Position>;

		/** Finger, palm, and latent impression types. */
		enum class Impression
		{
			PlainContact				= 0,
			LiveScanPlain				= 0,
			RolledContact				= 1,
			LiveScanRolled				= 1,
			NonLiveScanPlain			= 2,
			NonLiveScanRolled			= 3,
			LatentImage				= 4,
			LatentImpression			= 4,
			LatentTracing				= 5,
			LatentPhoto				= 6,
			LatentLift				= 7,
			LiveScanSwipe				= 8,
			LiveScanVerticalSwipe			= 8,
			LiveScanPalm				= 10,
			NonLiveScanPalm				= 11,
			LatentPalmImpression			= 12,
			LatentPalmTracing			= 13,
			LatentPalmPhoto				= 14,
			LatentPalmLift				= 15,
			LiveScanOpticalContactPlain		= 20,
			LiveScanOpticalContactRolled		= 21,
			LiveScanNonOpticalContactPlain		= 22,
			LiveScanNonOpticalContactRolled		= 23,
			ContactlessPlainStationarySubject	= 24,
			LiveScanOpticalContactlessPlain		= 24,
			ContactlessRolledStationarySubject	= 25,
			LiveScanOpticalContactlessRolled	= 25,
			LiveScanNonOpticalContactlessPlain	= 26,
			LiveScanNonOpticalContactlessRolled	= 27,
			Other					= 28,
			Unknown					= 29,
			ContactlessRolledMovingSubject		= 41,
			ContactlessPlainMovingSubject		= 42
		};

		/** Joint and tip codes. */
		enum class FingerImageCode {
			EJI = 0,
			RolledTip,
			FullFingerRolled,
			FullFingerPlainLeft,
			FullFingerPlainCenter,
			FullFingerPlainRight,
			ProximalSegment,
			DistalSegment,
			MedialSegment,
			NA
		};
		using PositionDescriptors = std::map<Position, FingerImageCode>;

		/** Friction Ridge Capture Technology codes. */
		enum class CaptureTechnology
		{
			Unknown					= 0,
			Other					= 1,
			ScannedInkOnPaper			= 2,
			OpticalTIRBright			= 3,
			OpticalTIRDark				= 4,
			OpticalDINative				= 5,
			OpticalDILowFrequenceyUnwrapped		= 6,
			ThreeDimensionalHighFrequencyUnwrapped	= 7,
			Capacitive				= 9,
			CapacitiveRF				= 10,
			Electroluminescent			= 11,
			ReflectedUltrasonic			= 12,
			UltrasonicImpediography			= 13,
			Thermal					= 14,
			DirectPressureSensitive			= 15,
			IndirectPressure			= 16,
			LiveTape				= 17,
			LatentImpression			= 18,
			LatentPhoto				= 19,
			LatentMold				= 20,
			LatentTracing				= 21,
			LatentLift				= 22
		};
	}
}

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Finger::PatternClassification,
    BE_Finger_PatternClassification_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Finger::Position,
    BE_Finger_Position_EnumToStringMap);

 BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Finger::Impression,
    BE_Finger_Impression_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Finger::FingerImageCode,
    BE_Finger_FingerImageCode_EnumToStringMap);

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::Finger::CaptureTechnology,
    BE_Finger_CaptureTechnology_EnumToStringMap);

#endif /* __BE_FINGER_H__ */

