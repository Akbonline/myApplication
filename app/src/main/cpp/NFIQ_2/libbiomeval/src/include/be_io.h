/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/
#ifndef __BE_IO_H__
#define __BE_IO_H__

#include <cstdint>

#include <be_framework_enumeration.h>

/*
 * This file contains items that are used within the Biometric Evaluation
 * IO framework.
 */
namespace BiometricEvaluation {

	/**
	 * @brief
	 * Input/Output functionality.
	 * @details
	 * The IO package contains classes and functions used to abstract
	 * input and output operations and provide for robust error handling
	 * on behalf of the application.
	 */
	namespace IO
	{
		/** Accessibility of object. */
		enum class Mode
		{
			/**
			 * Constant indicating the state of an object
			 * that manages some underlying file is accessible
			 * for reading and writing.
			 */
			ReadWrite = 0,

			/**
			 * Constant indicating the state of an object
			 * that manages some underlying file is accessible
			 * for reading only.
			 */
			ReadOnly = 1
		};
	}
}

BE_FRAMEWORK_ENUMERATION_DECLARATIONS(
    BiometricEvaluation::IO::Mode,
    BE_IO_Mode_EnumToStringMap);

#endif	/* __BE_IO_H__ */
