#ifndef ZBLUENET_CLASS_UTIL_H
#define ZBLUENET_CLASS_UTIL_H

#define ZBLUENET_SINGLETON(T) \
public: \
	static T *getInstance() { \
		static T obj; \
		return &obj; \
	} \
private: \
	T(); \
	~T(); \
	T(const T &); \
	T& operator=(const T &); \

#define ZBLEUNET_PRECREATED_SINGLETON(_T)      \
public:                                        \
    static _T *getInstance()                   \
    {                                          \
        static _T obj;                         \
        return &obj;                           \
    }                                          \
                                               \
private:                                       \
    struct ObjectCreator {                     \
        ObjectCreator() { _T::getInstance(); } \
    };                                         \
    static ObjectCreator oc;                   \
                                               \
    _T();                                      \
    ~_T();                                     \
    _T(const _T &);                            \
    _T &operator=(const _T &);                 \


#define ZBLUENET_PRECREATED_SINGLETON_IMPL(_T) \
    _T::ObjectCreator _T::oc;        \


#define ZBLUENET_NONCOPYABLE(_T) \
    _T(const _T &);              \
    _T &operator=(const _T &);   \

namespace zbluenet {
	
	class Noncopyable {
	protected:
		Noncopyable() = default;
		~Noncopyable() = default;
	private:  // emphasize the following members are private
		Noncopyable(const Noncopyable&) = delete;
		const Noncopyable& operator=(const Noncopyable&) = delete;
	};

} // namespace zbluenet

#endif // ZBLUENET_CLASS_UTIL_H
