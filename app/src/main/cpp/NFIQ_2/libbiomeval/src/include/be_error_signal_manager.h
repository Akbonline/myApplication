/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/
#ifndef __BE_ERROR_SIGNAL_MANAGER_H__
#define __BE_ERROR_SIGNAL_MANAGER_H__

#include <csetjmp>
#include <csignal>

#include <be_error_exception.h>

/*
 * Macros that are used by applications to indicate the start and end of
 * a signal handling block.
 */
#define BEGIN_SIGNAL_BLOCK(_sigmgr, _blockname) do {			\
	(_sigmgr)->clearSigHandled();					\
	(_sigmgr)->stop();						\
	if (sigsetjmp(							\
	    BiometricEvaluation::Error::SignalManager::_sigJumpBuf, 1) != 0) \
	 {								\
		(_sigmgr)->setSigHandled();				\
		goto _blockname ## _end;				\
	}								\
	(_sigmgr)->start();						\
} while (0)

#define END_SIGNAL_BLOCK(_sigmgr, _blockname) do {			\
	_blockname ## _end:						\
	(_sigmgr)->stop();						\
} while (0);

#define ABORT_SIGNAL_MANAGER(_sigmgr) do {				\
	(_sigmgr)->stop();						\
} while (0);

namespace BiometricEvaluation {

	namespace Error {

/**
 * @brief
 * A SignalManager object is used to handle signals that come from the operating
 * system.

 * @details
 * Applications typically do not invoke most methods of a SignalManager, except
 * the setSignalSet(), setDefaultSignalSet(), and sigHandled(). An application
 * wishing to just catch memory errors can simply construct a SignalManager
 * object, and invoke sigHandled() at the end of the signal block to detect
 * whether a signal was handled.
 *
 * The BEGIN_SIGNAL_BLOCK macro sets up the jump block and
 * tells the SignalManager object to start handling signals. Applications
 * can call either setSignalSet() or setDefaultSignalSet() before invoking
 * these macros to indicate which signals are to be handled.
 *
 * The END_SIGNAL_BLOCK() macro clears the signal set, so from that point
 * forward application code signals will be handled in the system's default
 * manner until another signal block is created.
 *
 * The ABORT_SIGNAL_MANAGER() macro also disables the watchdog timer but does
 * not create the code point destination for the jump point. This macro should
 * be used to disable a SignalManager object when the application is no longer
 * interested in the signal handling.
 *
 * @attention
 * The BEGIN_SIGNAL_BLOCK() macro must be paired with either the
 * END_SIGNAL_BLOCK() macro or ABORT_SIGNAL_MANAGER() macro. Failure
 * to do so may result in undefined behavior as an active SignalManager
 * may be invoked, forcing a jump into an incompletely initialized function.
 *
 * A SignalManager is passive (i.e. no signal handlers are installed)
 * until that start() method is called, and becomes passive when stop() is
 * invoked. The signals that are to be handled by the object are maitained as
 * state, and the set of signals can be changed at any time, but are not in
 * effect until start() is called.
 *
 * @attention
 * The start(), stop(), setSigHandled() and clearSigHandled() methods are not
 * meant to be used directly by applications, which should use the 
 * BEGIN_SIGNAL_BLOCK()/END_SIGNAL_BLOCK() macro pair.
 */
		class SignalManager {

		public:
			
			/**
			 * Construct a new SignalManager object with the default
			 * signal handling: SIGSEGV and SIGBUS.
			 *
			 * @throw
			 *      Error::StrategyError
			 *		Could not register the signal handler.
			 */
			SignalManager();

			/**
			 * Construct a new SignalManager object with the
			 * specified signal handling, no defaults.
			 *
			 * @param
			 *	signalSet (in)
			 *              The signal set; see sigaction(2),
			 *		sigemptyset(3) and sigaddset(3).
			 * @throw
			 *	Error::ParameterError
			 *		One of the signals in signalSet cannot
			 * 		be handled (SIGKILL, SIGSTOP.).
			 */
			SignalManager(
			    const sigset_t signalSet);

			/**
			 * Set the signals this object will manage.
			 *
			 * @param
			 *      signalSet (in)
			 *              The signal set; see sigaction(2),
			 *		sigemptyset(3) and sigaddset(3).
			 * @throw
			 *	Error::ParameterError
			 *		One of the signals in signalSet cannot
			 * 		be handled (SIGKILL, SIGSTOP.).
			 */
			void setSignalSet(
			    const sigset_t signalSet);

			/**
			 * Clear all signal handling.
			 */
			void clearSignalSet();

			/**
			 * Set the default signals this object will manage:
			 * SIGSEGV and SIGBUS.
			 */
			void setDefaultSignalSet();

			/**
			 * Indicate whether a signal was handled.
			 * @return
			 *      true if a signal was handled, false otherwise.
			 */
			bool sigHandled();

			 /**
			 * Start handling signals of the current signal set.
			 * @throw
			 *	Error::StrategyError
			 *		Could not register the signal handler.
			 *
			 * @note If an application invokes start() 
			 * without setting up a signal jump block, behavior
 			 * is undefined, and can result in an infinite loop
			 * if further processing causes a signal to be raised.
			 */
			void start();

			 /**
			 * Stop handling signals of the current signal set.
			 * @throw
			 *	Error::StrategyError
			 *		Could not register the signal handler.
			 */
			void stop();

			/**
			 * Set a flag to indicate a signal was handled.
			 */
			void setSigHandled();

			/**
			 * Clear the indication that a signal was handled.
			 */
			void clearSigHandled();

			/**
			 * Flag indicating can jump after handling a signal.
			 * @note Should not be directly used by applications.
			 */
			static bool _canSigJump;
			/**
			 * The jump buffer used by the signal handler.
			 * @note Should not be directly used by applications.
			 */
			static sigjmp_buf _sigJumpBuf;

		protected:

		private:
			/**
			 * Current signal set.
			 */
			sigset_t _signalSet;

			/**
			 * Flag indicated that a signal was handled.
			 */
			bool _sigHandled{false};
		};

		/*
 		* Declaration of the signal handler, a function with C linkage
		* that will handle all signals managed by this object,
		* conditionally jumping to a jump block within the application
		* process. This function is of no interest to applications,
		* which should use the BEGIN_SIGNAL_BLOCK()/END_SIGNAL_BLOCK()
		* macro pair to take advantage of signal handling.
 		*/
		extern "C" {
			void SignalManagerSighandler(int signo,
			    siginfo_t *info, void *uap);
		}
	}
}
#endif	/* __BE_ERROR_SIGNAL_MANAGER_H__ */
