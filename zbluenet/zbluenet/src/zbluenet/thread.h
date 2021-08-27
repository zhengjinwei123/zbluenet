#ifndef ZBLUENET_THREAD_H
#define ZBLUENET_THREAD_H

#include <zbluenet/semaphore.h>
#include <thread>

namespace zbluenet {
	class Thread {
	public:
		using EventCallback = std::function<void(Thread*)>;

	public:
		Thread();
		~Thread();

		static void sleep(time_t deltaTime);
		static std::thread::id getId();

		void start(EventCallback on_create = nullptr, EventCallback on_run = nullptr, EventCallback on_destroy = nullptr);
		void close();
		void exit();

		bool isRun();

	protected:
		void onRun();

	private:
		EventCallback on_create_cb_;
		EventCallback on_run_cb_;
		EventCallback on_destroy_cb_;

		std::mutex mutex_;
		Semaphore sem_;
		bool is_run_;
	};

} // namespace zbluenet
#endif // ZBLUENET_THREAD_H
