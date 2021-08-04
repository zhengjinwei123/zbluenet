#include <zbluenet/thread.h>
#include <thread>

namespace zbluenet {
	Thread::Thread() :
		on_create_cb_(nullptr), on_run_cb_(nullptr), on_destroy_cb_(nullptr)
	{

	}
	Thread::~Thread()
	{

	}

	void Thread::sleep(time_t deltaTime)
	{
		std::chrono::milliseconds t(deltaTime);
		std::this_thread::sleep_for(t);
	}

	std::thread::id Thread::getId()
	{
		return std::this_thread::get_id();
	}

	void Thread::start(EventCallback on_create, EventCallback on_run, EventCallback on_destroy)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (is_run_ == false) {
			is_run_ = true;

			if (on_create) {
				on_create_cb_ = on_create;
			}
			if (on_run) {
				on_run_cb_ = on_run;
			}
			if (on_destroy) {
				on_destroy_cb_ = on_destroy;
			}
			std::thread mythread(std::mem_fun(&Thread::onRun), this);
			mythread.detach();
		}
	}
	void Thread::close()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (is_run_) {
			is_run_ = false;
			sem_.wait();
		}
	}

	void Thread::exit()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (is_run_) {
			is_run_ = false;
		}
	}

	bool Thread::isRun()
	{
		return is_run_;
	}

	void Thread::onRun()
	{
		if (on_create_cb_) {
			on_create_cb_(this);
		}
		if (on_run_cb_) {
			on_run_cb_(this);
		}
		if (on_destroy_cb_) {
			on_destroy_cb_(this);
		}

		sem_.wakeup();
		is_run_ = false;
	}
}