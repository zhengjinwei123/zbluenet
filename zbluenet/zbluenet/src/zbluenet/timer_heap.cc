#include <zbluenet/timer_heap.h>
#include <zbluenet/timer.h>
#include <zbluenet/timer_id_allocator.h>
#include <zbluenet/timestamp.h>

#include <cstddef>
#include <algorithm>
#include <vector>
#include <unordered_map>

namespace zbluenet {
	class TimerHeap::Impl {
	public:
		using TimerCallback = TimerHeap::TimerCallback;
		using TimerMap = std::unordered_map<zbluenet_type::TimerId, Timer *>;
		using TimerVector = std::vector<Timer *>;

		Impl();
		~Impl();

		int64_t getNextTimeoutMillisecond(const Timestamp &now) const;
		zbluenet_type::TimerId addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = -1);
		void removeTimer(zbluenet_type::TimerId timer_id);
		void checkTimeout(const Timestamp &now);

	public:
		void minHeapReserve();
		void minHeapInsert(Timer *timer);
		void minHeapErase(Timer *timer);
		Timer *minHeapTop() const;

	private:
		TimerIdAllocator timer_id_allocator_;
		TimerMap timer_map_;
		TimerVector timer_min_heap_;
	};

	TimerHeap::Impl::Impl()
	{
		timer_min_heap_.emplace_back(nullptr);
	}

	TimerHeap::Impl::~Impl()
	{
		for (auto iter = timer_map_.begin(); iter != timer_map_.end(); ++iter)
		{
			delete iter->second;
		}
	}

	int64_t  TimerHeap::Impl::getNextTimeoutMillisecond(const Timestamp &now) const
	{
		Timer *timer = minHeapTop();
		if (nullptr == timer) {
			return -1;
		}
		if (timer->getValue() < now) {
			return -1;
		}

		return now.distanceMillisecond(timer->getValue());
	}

	zbluenet_type::TimerId TimerHeap::Impl::addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times)
	{
		zbluenet_type::TimerId timer_id = timer_id_allocator_.nextId();
		timeout_ms = std::max((int64_t)0, timeout_ms);
		std::unique_ptr<Timer> timer(new Timer(timer_id,
			now + timeout_ms,
			timeout_ms,
			timer_cb,
			call_times
		));

		if (timer_map_.find(timer_id) != timer_map_.end()) {
			return -1;
		}

		timer_map_.insert(std::make_pair(timer_id, timer.get()));

		Timer *timer_raw = timer.release();
		minHeapReserve();
		minHeapInsert(timer_raw);
		return timer_id;
	}

	void TimerHeap::Impl::removeTimer(zbluenet_type::TimerId timer_id)
	{
		auto iter = timer_map_.find(timer_id);
		if (iter == timer_map_.end()) {
			return;
		}

		Timer *timer = iter->second;
		minHeapErase(timer);
		timer_map_.erase(iter);
		delete timer;
	}

	void TimerHeap::Impl::checkTimeout(const Timestamp &now)
	{
		for (;;) {
			Timer *timer = minHeapTop();
			if (nullptr == timer) {
				return;
			}

			if (now.millisecondLess(timer->getValue())) {
				return;
			}

			zbluenet_type::TimerId timer_id = timer->getId();
			TimerCallback timer_cb = timer->getCallbackFunc();

			minHeapErase(timer);

			if (timer->getCallTimes() == 1) {
				timer_map_.erase(timer_id);
				delete timer;
			} else {
				if (timer->getCallTimes() > 0) {
					timer->decCallTimes();
				}

				timer->getValue() += timer->getTimeoutMillisec();
				minHeapInsert(timer);
			}

			timer_cb(timer_id);
		}
	}

	void TimerHeap::Impl::minHeapReserve()
	{
		timer_map_.reserve(timer_min_heap_.size() + 1);
	}

	void TimerHeap::Impl::minHeapInsert(Timer *timer)
	{
		int cur_index = timer_min_heap_.size();
		timer_min_heap_.emplace_back(timer);
		timer->setHeapPos(cur_index);

		// 向上调整堆
		for (;;) {
			int parent_index = cur_index / 2;

			if (0 == parent_index) {
				break;
			}

			if (timer_min_heap_[parent_index]->getValue() < timer_min_heap_[cur_index]->getValue()) {
				break;
			}

			timer_min_heap_[parent_index]->setHeapPos(cur_index);
			timer_min_heap_[cur_index]->setHeapPos(parent_index);
			std::swap(timer_min_heap_[parent_index], timer_min_heap_[cur_index]);

			cur_index = parent_index;
		}
	}

	void TimerHeap::Impl::minHeapErase(Timer *timer)
	{
		int cur_index = timer->getHeapPos();
		if (cur_index < 0) {
			return;
		}

		timer->setHeapPos(-1);
		timer_min_heap_[cur_index] = timer_min_heap_.back();
		timer_min_heap_[cur_index]->setHeapPos(cur_index);
		timer_min_heap_.pop_back();

		// 向下调整
		for (;;) {
			int child_index = cur_index * 2;

			if (child_index >= (int)timer_min_heap_.size()) {
				break;
			}

			if (child_index + 1 < (int)timer_min_heap_.size() &&
				timer_min_heap_[child_index + 1]->getValue() < timer_min_heap_[child_index]->getValue()) {
				++child_index;
			}

			if (timer_min_heap_[cur_index]->getValue() < timer_min_heap_[child_index]->getValue()) {
				break;
			}

			timer_min_heap_[cur_index]->setHeapPos(child_index);
			timer_min_heap_[child_index]->setHeapPos(cur_index);

			std::swap(timer_min_heap_[cur_index], timer_min_heap_[child_index]);

			cur_index = child_index;
		}
	}

	Timer *TimerHeap::Impl::minHeapTop() const
	{
		if (timer_min_heap_.size() <= 1) {
			return nullptr;
		}
		return timer_min_heap_[1];
	}

	//////////////////////////////////////////////////////////////////////////
	TimerHeap::TimerHeap() : pimpl_(new Impl())
	{

	}

	TimerHeap::~TimerHeap()
	{

	}


	int64_t TimerHeap::getNextTimeoutMillisecond(const Timestamp &now) const
	{
		return pimpl_->getNextTimeoutMillisecond(now);
	}

	zbluenet_type::TimerId TimerHeap::addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times)
	{
		return pimpl_->addTimer(now, timeout_ms, timer_cb, call_times);
	}

	void TimerHeap::removeTimer(zbluenet_type::TimerId timer_id)
	{
		pimpl_->removeTimer(timer_id);
	}

	void TimerHeap::checkTimeout(const Timestamp &now)
	{
		pimpl_->checkTimeout(now);
	}

} // namespace zbluenet
