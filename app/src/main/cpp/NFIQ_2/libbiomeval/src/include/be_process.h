/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_PROCESS_H__
#define __BE_PROCESS_H__

#include <map>
#include <memory>
#include <string>

namespace BiometricEvaluation 
{
	/**
	 * @brief
	 * Process information and controls.
	 * @details
	 * The Process package gathers all process related matters,
	 * including a class to obtain resource usage statistics.
	 */
	namespace Process
	{
		/** Convenience alias for parameter lists to child routines */
		using ParameterList =
		    std::map<std::string, std::shared_ptr<void>>;
	}
}
#endif /* __BE_PROCESS_H__ */
