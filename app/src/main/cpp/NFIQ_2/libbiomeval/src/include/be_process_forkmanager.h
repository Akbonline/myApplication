/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifndef __BE_PROCESS_FORKMANAGER_H__
#define __BE_PROCESS_FORKMANAGER_H__

#include <unistd.h>

#include <list>

#include <be_process_manager.h>

namespace BiometricEvaluation
{
	namespace Process
	{
		/* Forward declaration */
		class ForkWorkerController;

		/**
		 * @brief
		 * Manager implementation that starts Workers by calling
		 * fork(2).
		 */
		class ForkManager : public Manager
		{
		public:
			/**
			 * @brief
			 * List of all instantiated ForkManagers.
			 * @details
			 * This is not a list of managed pointers to
			 * ForkManagers.  If it was, the smart pointer's
			 * destructor would attempt to delete the object
			 * being pointed to at program termination, which is
			 * ultimately sometime after the destructor of the
			 * ForkManager itself was called.
			 */
			static std::list<ForkManager*> FORKMANAGERS;

			/**
			 * ForkManager constructor.
			 */
			ForkManager();

			/**
			 * @brief
			 * Adds a Worker to be managed by this Manager.
			 *
			 * @param worker
			 *	A Worker instance to run.
			 *
			 * @return
			 *	shared_ptr to worker.
			 */
			std::shared_ptr<WorkerController>
			addWorker(
			    std::shared_ptr<Worker> worker);

			/**
			 * @brief
			 * Begin Worker's work.
			 *
			 * @param[in] wait
			 *	Whether or not to wait for all Workers to
			 *	return before returning.
			 * @param[in] communicate
			 *	Whether or not to enable communication
			 *	among the Workers and Managers.
			 *
			 * @throw Error::ObjectExists
			 *	At least one Worker is already working.
			 * @throw Error::StrategyError
			 *	Problem forking.
			 */
			void
			startWorkers(
			    bool wait = true,
			    bool communicate = false);
   
			/**
			 * @brief
			 * Start a worker
			 *
			 * @param worker
			 *	Pointer to a WorkerController that is being
			 *	managed by this Manager instance.
			 * @param wait
			 *	Whether or not to wait for this Worker to 
			 *	exit before returning control to the caller.
 			 * @param[in] communicate
			 *	Whether or not to enable communication
			 *	among the Workers and Managers.
			 *
			 * @throw Error::ObjectExists
			 *	worker is already working.
			 * @throw Error::StrategyError
			 *	worker is not managed by this Manager instance.
			 */
			void
			startWorker(
			    std::shared_ptr<WorkerController> worker,
			    bool wait = true,
			    bool communicate = false);

			/**
			 * @brief
			 * Ask Worker to exit.
			 * @details
			 * Sends SIGUSR1 to the Worker, which ForkManager will
			 * handle automatically.
			 *
			 * @param workerController
			 *	Pointer to the ForkWorkerController that should
			 *	be stopped.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	worker is not working.
			 * @throw Error::StrategyError
			 *	Problem sending the signal.
			 *
			 * @attention
			 * Do not call stopWorker() when communication is
			 * enabled unless you will be finished with
			 * communication for all Workers at that point.  This
			 * creates a race condition for reads()/writes() when
			 * the Worker exits.
			 */
			void
			stopWorker(
			    std::shared_ptr<WorkerController> workerController);

			/**
			 * @brief
			 * Send a POSIX signal to all workers.
			 *
			 * @param[in] signo
			 * 	The signal to send.
			 */
			void broadcastSignal(int signo);

			/**
			 * @brief
			 * Obtain whether or not this ForkManager is
			 * responsbile for a particular PID.
			 *
			 * @param[in] pid
			 *	PID in question
			 *
			 * @return
			 *	true if this ForkManager spawned pid,
			 *	false otherwise.
			 */
			bool
			responsibleFor(
			    const pid_t pid)
			    const;

			/**
			 * @brief
			 * Set Status.isWorking for PID to false.
			 *
			 * @param[in] pid
			 *	PID whose inWorking flag should be set to false
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	PID not under this manager's control.
			 */
			void
			setNotWorking(
			    const pid_t pid);

			/**
			 * @brief
			 * Call setNotWorking() for all PIDs known to this
			 * ForkManager.
			 */
			void
			markAllFinished();

			/**
			 * @brief
			 * Get Status.isWorking for PID.
			 *
			 * @param[in] pid
			 *	PID whose inWorking flag should be queried
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	PID not under this manager's control.
			 */
			bool
			getIsWorkingStatus(
			    const pid_t pid)
			    const;

			void
			waitForWorkerExit();

			/**
			 * @brief
			 * ForkManager destructor.
			 */
			~ForkManager();

			/**
			 * @brief
			 * Call a function in your program when a child
			 * exits.
			 * 
			 * @param exitCallback
			 *	Function pointer to a method that takes a
			 *	shared_ptr to a ForkWorkerController and the
			 *	integer status information.
			 *
			 * @note
			 *	The exit callback will not have any effect if
			 *	the Manager is not set to wait for Workers.
			 */
			void
			setExitCallback(
			    void (*exitCallback)
			    (std::shared_ptr<ForkWorkerController> worker,
			    int stat_loc));

			/**
			 * @brief
			 * A default exit callback function.
			 * @details
			 * Writes to stdout in the form:
			 *	PID #: Exited <description>.
			 *
			 * @param worker
			 *	The ForkWorkerController object that exited.
			 * @param status
			 *	The status of the Worker that exited 
			 *	(from wait(2)).
			 */
			static void
			defaultExitCallback(
			    std::shared_ptr<ForkWorkerController> worker,
			    int status);

			/**
			 * @brief 
			 * Set the exit status in the WorkerController for given
			 * process ID.
			 *
			 * @param[in] pid
			 * PID whose exit status should be set.
			 * @param[in] status
			 * Status, as returned from wait(2).
			 *
			 * @throw Error::ObjectDoesNotExist
			 * PID not under this manager's control.
			 *
			 * @note
			 * Exit status is only set if process exited cleanly.
			 */
			void
			setExitStatus(
			    const pid_t pid,
			    const int32_t waitStatus);
			
		private:
			/**
			 * @brief
			 * Obtain a shared_ptr to a running Worker with pid.
			 *
			 * @param pid
			 *	The PID of the Worker in question.
			 *
			 * @return
			 *	shared_ptr to pid's ForkWorkerController
			 *	representation.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	No process with PID pid found.
			 */
			std::shared_ptr<ForkWorkerController>
			getProcessWithPID(
			    pid_t pid);

			/** 
			 * @brief
			 * Do not return until all Workers exit.
			 */
			void
			_wait();

			/**
			 * @brief
			 * Clean-up zombie children.
			 *
			 * @param signal
			 *	The signal caught that prompted this function
			 *	to be called (SIGCHLD).
			 */
			static void
			reap(
			    int signal);

			/**
			 * @brief
			 * Private wrapper containing details about the Worker.
			 * @details
			 * We need to record whether or not a process is
			 * active based on the API activity, not by querying
			 * the system, since PIDs can be reused, etc.
			 */
			class Status {
			public:
				/** Status default constructor */
				Status();

				/** PID of the process */
				pid_t pid;
				/** Whether or not the PID is active */
				bool isWorking;
			};
			    
			/**
			 * @brief
			 * Function pointer invoked when child exits.
			 *
			 * @param wc
			 *	Shared pointer to the WorkerController, 
			 * 	or pointer to NULL if the WorkerController
			 *	could not be identified.
			 * @param stat_loc
			 *	Termination information about the processs,
			 *	which is undefined when wc is a pointer
			 *	to NULL.
			 */
			void
			(*_exitCallback)
			    (std::shared_ptr<ForkWorkerController> wc,
			    int stat_loc);
		
			/** Whether or not this process is a parent process */
			bool _parent;

			/** Map of WorkerController statuses */
			std::map<
			    std::shared_ptr<ForkWorkerController>, Status>
			    _wcStatus;
		};
		
		
		/**
		 * @brief
		 * Wrapper of a Worker returned from a Process::ForkManager
		 */
		class ForkWorkerController : public WorkerController
		{
		public:
			/**
			 * @brief
			 * Obtain whether or not Worker is working.
			 *
			 * @return
			 *	Whether or not the Worker is working.
			 */
			bool
			isWorking()
			    const;

			bool
			everWorked()
			    const;
			    
			/**
			 * @brief
			 * Reuse the Worker.
			 *
			 * @throw Error::ObjectExists
			 *	The previously started Worker is still
			 *	running.
			 */
			void
			reset();

			/**
			 * @brief
			 * Obtain the PID of this process this instance 
			 * represents.
			 *
			 * @return
			 *	pid of the process this instance represents.
			 *
			 * @note
			 *	Call isRunning() before doing anything with 
			 *	the PID returned from this function.
			 */
			pid_t
			getPID()
			    const;
			
			/**
			 * @brief
			 * Tell _staticWorker to stop.
			 * @details
			 * Called by the child process instance when SIGUSR1
			 * is received.
			 *
			 * @param signal
			 *	The signal caught that prompted this function
			 *	to be called (SIGUSR1).
			 */
			static void
			_stop(
			    int signal);

			/**
			 * @brief
			 * ForkWorkerController destructor.
			 */
    			~ForkWorkerController();
			
		protected:
		
		private:
		
			/**
			 * ForkWorkerController constructor.
			 *
			 * @param worker
			 *	The Worker instance to wrap.
			 */
			ForkWorkerController(
			    std::shared_ptr<Worker> worker);
			    
			/**
			 * @brief
			 * Start the Worker decorated by this instance.
			 *
			 * @param communicate
			 *	Whether or not to enable communication between
			 *	Worker and Manager.
			 *
			 * @throw Error::ObjectExists
			 *	The decorated Worker is already working.
			 * @throw Error::StrategyError
			 *	Error in starting Worker.
			 *
			 * @note
			 *	Method is private so that it can only be
			 *	called from a friend Process::Manager.
			 */
			void
			start(
			    bool communicate = false);

			/**
			 * @brief
			 * Tell the Worker to stop.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	Worker is not working.
			 * @throw Error::StrategyError
			 *	Error asking Worker to stop.
			 */
			void
			stop();

			/** PID of the process represented by _worker */
    			pid_t _pid;
			
			/**
			 * A static pointer to "this", as there can only ever
			 * be one instance of a ForkedChild per Child, because
			 * of the way fork() copies memory.
			 */
			static std::shared_ptr<Worker> _staticWorker;

			/*
			 * Friends.
			 */
			
			/**
			 * @brief
			 * Begin Worker's work.
			 *
			 * @param[in] wait
			 *	Whether or not to wait for all Workers to
			 *	return before returning.
 			 * @param[in] communicate
			 *	Whether or not to enable communication
			 *	among the Workers and Managers.
			 *
 			 * @throw Error::ObjectExists
			 *	One or more of the Workers is already working.
			 * @throw Error::StrategyError
			 *	Problem forking.
			 */
			friend void
			ForkManager::startWorkers(
			    bool wait,
			    bool communicate);

			/**
			 * @brief
			 * Restart a completed Worker.
			 *
			 * @param worker
			 *	Pointer to a WorkerController that is being
			 *	managed by this Manager instance.
			 * @param wait
			 *	Whether or not to wait for this Worker to 
			 *	exit before returning control to the caller.
 			 * @param[in] communicate
			 *	Whether or not to enable communication
			 *	among the Workers and Managers.
			 *
			 * @throw Error::ObjectExists
			 *	worker is already working.
			 * @throw Error::StrategyError
			 *	worker is not managed by this Manager instance.
			 */
			friend void
			ForkManager::startWorker(
			    std::shared_ptr<WorkerController> worker,
			    bool wait,
			    bool communicate);

			/**
			 * @brief
			 * Ask Worker to exit.
			 * @details
			 * Sends SIGUSR1 to the Worker, which ForkManager will
			 * handle automatically.
			 *
			 * @param workerController
			 *	Pointer to the ForkWorkerController that should
			 *	be stopped.
			 *
			 * @throw Error::ObjectDoesNotExist
			 *	worker is not working.
			 * @throw Error::StrategyError
			 *	Problem sending the signal.
			 */
			friend void
			ForkManager::stopWorker(
			    std::shared_ptr<WorkerController> workerController);

			/**
			 * @brief
			 * Adds a Worker to be managed by this Manager.
			 *
			 * @param worker
			 *	A Worker instance to run.
			 *
			 * @return
			 *	shared_ptr to worker.
			 */
			friend std::shared_ptr<WorkerController>
			ForkManager::addWorker(
			    std::shared_ptr<Worker> worker);

			/**
			 * @brief
			 * Set the exit status in the WorkerController for given
			 * process ID.
			 *
			 * @param[in] pid
			 * PID whose exit status should be set.
			 * @param[in] status
			 * Status, as returned from wait(2).
			 *
			 * @throw Error::ObjectDoesNotExist
			 * PID not under this manager's control.
			 *
			 * @note
			 * Exit status is only set if process exited cleanly.
			 */
			friend void
			ForkManager::setExitStatus(
			    const pid_t pid,
			    const int32_t waitStatus);
		};
	}
}

#endif /* __BE_PROCESS_FORKMANAGER_H__ */
