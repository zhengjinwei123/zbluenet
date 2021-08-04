#ifndef ZBLUENET_CONCURRENT_QUEUE_H
#define ZBLUENET_CONCURRENT_QUEUE_H

#include <cstddef>
#include <deque>
#include <condition_variable>
#include <mutex>

#include <zbluenet/class_util.h>

namespace zbluenet {

	template <class T>
	class ConcurrentQueue : public Noncopyable {
	public:
		ConcurrentQueue(size_t max_size = 0) :
			max_size_(max_size), is_bounded_(max_size != 0)
		{
		}

		~ConcurrentQueue()
		{
			CleanQueue<T>::clean(queue_);
		}

		size_t size()
		{
			std::lock_guard<std::mutex> lock(data_mutex_);
			return queue_.size();
		}

		size_t maxSize() const
		{
			return max_size_;
		}

		bool isBounded() const
		{
			return is_bounded_;
		}

		bool empty()
		{
			std::lock_guard<std::mutex> lock(data_mutex_);
			return queue_.empty();
		}

		bool full()
		{
			std::lock_guard<std::mutex> lock(data_mutex_);
			return fullNoLock();
		}

		void push(const T &item)
		{
			std::unique_lock<std::mutex> lock(data_mutex_);
			while (fullNoLock()) {
				not_full_cond_.wait(lock);
			}
			queue_.push_back(item);
			not_empty_cond_.notify_one();
		}

		bool pushIfNotFull(const T &item)
		{
			std::lock_guard<std::mutex> lock(data_mutex_);
			if (fullNoLock()) {
				return false;
			}
			queue_.push_back(item);
			not_empty_cond_.notify_one();
			return true;
		}

		void pop(T &item)
		{
			std::unique_lock<std::mutex> lock(data_mutex_);
			while (queue_.empty()) {
				not_empty_cond_.wait(lock);
			}
			T front = queue_.front();
			queue_.pop_front();
			item = front;
			not_full_cond_.notify_one();
		}

		bool popIfNotEmpty(T &item)
		{
			std::lock_guard<std::mutex>  lock(data_mutex_);
			if (queue_.empty()) {
				return false;
			}
			T front = queue_.front();
			queue_.pop_front();
			item = front;
			not_full_cond_.notify_one();
			return true;
		}

		bool peek(T &item)
		{
			std::lock_guard<std::mutex> lock(data_mutex_);
			if (queue_.empty()) {
				return false;
			}
			item = queue_.front();
			return true;
		}

	private:
		template <class U>
		struct CleanQueue {
			static void clean(std::deque<U> &queue)
			{
			}
		};

		template <class U>
		struct CleanQueue<U *> {
			static void clean(std::deque<U *> &queue)
			{
				for (size_t i = 0; i < queue.size(); ++i) {
					delete queue[i];
				}
			}
		};

		bool fullNoLock() const
		{
			if (!is_bounded_) {
				return false;
			}
			else {
				return queue_.size() >= max_size_;
			}
		}

	private:
		std::deque<T> queue_;
		size_t max_size_;
		bool is_bounded_;
		std::mutex data_mutex_;
		std::condition_variable not_full_cond_;
		std::condition_variable not_empty_cond_;
	};

} // namespace zbluenet

#endif
