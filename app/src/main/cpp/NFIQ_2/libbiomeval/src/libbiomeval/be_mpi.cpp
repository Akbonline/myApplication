/**
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties.  Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain.  NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#include <sys/types.h>
#include <mpi.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include <be_io_filelogsheet.h>
#include <be_io_syslogsheet.h>
#include <be_error_exception.h>
#include <be_mpi.h>

namespace BE = BiometricEvaluation;

const std::map<BiometricEvaluation::MPI::TaskCommand, std::string>
BE_MPI_TaskCommand_EnumToStringMap = {
	{BiometricEvaluation::MPI::TaskCommand::Continue, "Continue"},
	{BiometricEvaluation::MPI::TaskCommand::Ignore, "Ignore"},
	{BiometricEvaluation::MPI::TaskCommand::Exit, "Exit"},
	{BiometricEvaluation::MPI::TaskCommand::QuickExit, "Quick Exit"},
	{BiometricEvaluation::MPI::TaskCommand::TermExit, "Term Exit"}
};
BE_FRAMEWORK_ENUMERATION_DEFINITIONS(
    BiometricEvaluation::MPI::TaskCommand,
    BE_MPI_TaskCommand_EnumToStringMap);

const std::map<BiometricEvaluation::MPI::TaskStatus, std::string>
BE_MPI_TaskStatus_EnumToStringMap = {
	{BiometricEvaluation::MPI::TaskStatus::OK, "OK"},
	{BiometricEvaluation::MPI::TaskStatus::Failed, "Failed"},
	{BiometricEvaluation::MPI::TaskStatus::Exit, "Exit"},
	{BiometricEvaluation::MPI::TaskStatus::RequestJobTermination,
	    "Request Job Termination"}
};
BE_FRAMEWORK_ENUMERATION_DEFINITIONS(
    BiometricEvaluation::MPI::TaskStatus,
    BE_MPI_TaskStatus_EnumToStringMap);

const std::map<BiometricEvaluation::MPI::MessageTag, std::string>
BE_MPI_MessageTag_EnumToStringMap  = {
	{BiometricEvaluation::MPI::MessageTag::Control, "Control"},
	{BiometricEvaluation::MPI::MessageTag::Data, "Data"},
	{BiometricEvaluation::MPI::MessageTag::OOB, "Out-of-band"}
};
BE_FRAMEWORK_ENUMERATION_DEFINITIONS(
    BiometricEvaluation::MPI::MessageTag,
    BE_MPI_MessageTag_EnumToStringMap);

std::string
BiometricEvaluation::MPI::generateUniqueID()
{
	char hn[MPI_MAX_PROCESSOR_NAME];
	int hlen;
	(void)MPI_Get_processor_name(hn, &hlen);
	std::string hostname((char *)hn);
	std::ostringstream oss;
#if 0
	oss << hostname << ' ' 
	    << ::MPI::COMM_WORLD.Get_rank()
	    << '[' << getpid() << ']';
#endif
	oss << hostname << '-' 
	    << ::MPI::COMM_WORLD.Get_rank()
	    << '-' << getpid();
	return (oss.str());
}

void
BiometricEvaluation::MPI::printStatus(const std::string &message)
{
	std::cout << BiometricEvaluation::MPI::generateUniqueID() << ": "
	     << message << "." << std::endl;
}

static void writeToLogsheet(
    BE::IO::Logsheet &logsheet,
    std::string message)
{
	static bool displayedMessage = false;

	/*
	 * If logging should fail at some point, errors are
	 * not propagated, but a message is displayed.
	 */
	try {
		logsheet.writeDebug(message);
	} catch (BE::Error::Exception &e) {
		if (!displayedMessage) {
			BE::MPI::printStatus("Log failure: " + e.whatString()
			    + "; logging disabled");
			displayedMessage = true;
			logsheet.setCommit(false);
			logsheet.setDebugCommit(false);
			logsheet.setCommentCommit(false);
		}
	}
}

void
BiometricEvaluation::MPI::logEntry(
    IO::Logsheet &logsheet)
{
	writeToLogsheet(logsheet, logsheet.getCurrentEntry());
	logsheet.resetCurrentEntry();
}

void
BiometricEvaluation::MPI::logMessage(
    IO::Logsheet &logsheet,
    const std::string &message)
{
	writeToLogsheet(logsheet, message);
}

std::shared_ptr<BiometricEvaluation::IO::Logsheet>
BiometricEvaluation::MPI::openLogsheet(
    const std::string &url,
    const std::string &description)
{
	std::shared_ptr<BE::IO::Logsheet> logsheet;
	if (url == "") {
		logsheet.reset(new BE::IO::Logsheet());
		return (logsheet);
	}
	BE::IO::Logsheet::Kind lsKind = BE::IO::Logsheet::getTypeFromURL(url);
	switch (lsKind) {
		case BE::IO::Logsheet::Kind::File: {
			std::string locURL = url + "-"
			    + BE::MPI::generateUniqueID();
			try {
				logsheet.reset(new BE::IO::FileLogsheet(
			    	locURL,
			    	description));
			} catch (const BE::Error::ObjectExists&) {
				try {
					logsheet.reset(
					    new BE::IO::FileLogsheet(locURL));
				} catch (BE::Error::Exception &e) {
					throw BE::Error::Exception(
					    "Could not open FileLogsheet: " 
					    + e.whatString());
				}
			}
			break;
		}
		case BE::IO::Logsheet::Kind::Syslog: {
			/* Use the MPI rank as the application name */
			try {
				logsheet.reset(new BE::IO::SysLogsheet(
				    url,
				    description,
				    std::to_string(::MPI::COMM_WORLD.Get_rank()),
				    true, true));
			} catch (BE::Error::Exception &e) {
				throw BE::Error::Exception(
				    "Could not open SysLogsheet: " 
				    + e.whatString());
			}
			break;
		}
		default:
			throw (
			    BE::Error::ParameterError("Invalid Logsheet URL"));
	}
	return (logsheet);
}

