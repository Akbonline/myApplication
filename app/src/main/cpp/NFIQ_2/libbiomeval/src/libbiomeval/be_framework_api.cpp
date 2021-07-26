/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to Title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <be_framework_api.h>

const std::map<BiometricEvaluation::Framework::APICurrentState, std::string>
    BE_Framework_APICurrentState_EnumToStringMap =
{
    {BiometricEvaluation::Framework::APICurrentState::NeverCalled,
        "Never Called"},
    {BiometricEvaluation::Framework::APICurrentState::WatchdogExpired,
        "Watchdog Expired"},
    {BiometricEvaluation::Framework::APICurrentState::SignalCaught,
        "Signal Caught"},
    {BiometricEvaluation::Framework::APICurrentState::ExceptionCaught,
        "Exception Caught"},
    {BiometricEvaluation::Framework::APICurrentState::Running,
        "Running"},
    {BiometricEvaluation::Framework::APICurrentState::Completed,
        "Completed"}
};

BE_FRAMEWORK_ENUMERATION_DEFINITIONS(
    BiometricEvaluation::Framework::APICurrentState,
    BE_Framework_APICurrentState_EnumToStringMap);
