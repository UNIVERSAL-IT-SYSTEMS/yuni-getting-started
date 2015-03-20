#pragma once
#include "process-info.h"



namespace Yuni
{
namespace Process
{


	inline Program::ProcessSharedInfo::ProcessSharedInfo()
		: running(false)
		, processID(-1)
		, processInput(-1)
		, duration(0)
		, durationPrecision(dpSeconds)
		, timeout()
		, exitstatus(-1)
		, redirectToConsole(false)
		, timeoutThread(nullptr)
	{}


	inline Program::ProcessSharedInfo::~ProcessSharedInfo()
	{
		if (YUNI_UNLIKELY(timeoutThread))
		{
			// should never go in this section
			assert(false and "the thread for handling the timeout has not been properly stopped");
			timeoutThread->stop();
			delete timeoutThread;
		}
	}


	template<bool WithLock>
	inline bool Program::ProcessSharedInfo::sendSignal(int sigvalue)
	{
		if (WithLock)
			mutex.lock();
		if (0 == running)
		{
			if (WithLock)
				mutex.unlock();
			return false;
		}

		# ifndef YUNI_OS_WINDOWS
		{
			const pid_t pid = static_cast<pid_t>(processID);
			if (WithLock)
				mutex.unlock();
			if (pid > 0)
				return (0 == ::kill(pid, sigvalue));
		}
		# else
		{
			# warning missing implementation on windows
			switch (sigvalue)
			{
				case SIGKILL:
					break;
			}
			(void) sigvalue;

			if (WithLock)
				mutex.unlock();
		}
		# endif

		return false;
	}




	namespace // anonymous
	{

		class TimeoutThread final : public Thread::IThread
		{
		public:
			TimeoutThread(int pid, uint timeout)
				: timeout(timeout)
				, pid(pid)
			{
				assert(pid > 0);
			}

			~TimeoutThread() {}


		protected:
			virtual bool onExecute() override
			{
				// wait for timeout... (note: the timeout is in seconds)
				if (not suspend(timeout * 1000))
				{
					// the timeout has been reached

					#ifdef YUNI_OS_UNIX
					::kill(pid, SIGKILL);
					#else
					#warning not implemented
					#endif
				}
				return false; // stop the thread, does not suspend it
			}


		private:
			//! Timeout in seconds
			uint timeout;
			//! PID of the sub process
			int pid;
		};



	} // anonymous namespace


	inline void Program::ProcessSharedInfo::createThreadForTimeoutWL()
	{
		delete timeoutThread; // for code safety

		if (processID > 0 and timeout > 0)
		{
			timeoutThread = new TimeoutThread(processID, timeout);
			timeoutThread->start();
		}
		else
		{
			// no valid pid, no thread required for a timeout
			timeoutThread = nullptr;
		}
	}







} // namespace Process
} // namespace Yuni

