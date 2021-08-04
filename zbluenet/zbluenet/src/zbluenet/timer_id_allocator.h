#ifndef ZBLUENET_TIMER_ID_ALLOCATOR_H
#define ZBLUENET_TIMER_ID_ALLOCATOR_H

#include <zbluenet/type_define.h>

namespace zbluenet {
	class TimerIdAllocator {
	public:
		TimerIdAllocator() :
			timer_id_(0)
		{

		}
		~TimerIdAllocator()
		{

		}

		zbluenet_type::TimerId nextId()
		{
			if (++timer_id_ < 0) {
				timer_id_ = 0;
			}
			return timer_id_;
		}

	private:
		zbluenet_type::TimerId timer_id_;
	};
} // namespace zbluenet

#endif // ZBLUENET_TIMER_ID_ALLOCATOR_H
