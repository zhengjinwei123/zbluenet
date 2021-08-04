#ifndef ZBLUENET_BASE_HEAP_VALUE_H
#define ZBLUENET_BASE_HEAP_VALUE_H

namespace zbluenet {
	template <typename T>
	class BaseHeapValue {
	public:
		BaseHeapValue(const T &v):
			heap_pos_(-1), value_(v)
		{

		}
		virtual ~BaseHeapValue()
		{

		}

		virtual int getHeapPos() const { return heap_pos_; }
		virtual void setHeapPos(int heap_pos) { heap_pos_ = heap_pos; }
		virtual void setValue(const T &value) { value_ = value;  }
		virtual T& getValue() { return value_; }
		virtual const T& getValue() const { return value_;  }

		bool operator<(const BaseHeapValue &other) const { return value_ < other.value_;  }
	private:
		int heap_pos_;
		T value_;
	};
} // namespace zbluenet

#endif //ZBLUENET_BASE_HEAP_VALUE_H
