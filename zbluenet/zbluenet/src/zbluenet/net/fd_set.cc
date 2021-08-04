#include <zbluenet/net/fd_set.h>

namespace zbluenet {
	namespace net {

		FDSet::FDSet()
		{
		
		}

		FDSet::~FDSet()
		{
			destroy();
		}

		void FDSet::create(int max_fd_count)
		{
			int nSocketNum = max_fd_count;
#ifdef _WIN32
			if (nSocketNum < 64) {
				nSocketNum = 64;
			} else if (nSocketNum > FD_SETSIZE) {
				nSocketNum = FD_SETSIZE;
			}

			fd_size_ = sizeof(u_int) + (sizeof(SOCKET) * nSocketNum);
#else
			if (nSocketNum < 30000) {
				nSocketNum = 30000;
			}
			fd_size_ = nSocketNum / (8 * sizeof(char)) + 1;
#endif // _WIN32
			pfd_set_ = (fd_set *)new char[fd_size_];
			memset(pfd_set_, 0, fd_size_);
			max_sock_fd_ = nSocketNum;
		}

		void FDSet::destroy()
		{
			if (pfd_set_) {
				delete[] pfd_set_;
				pfd_set_ = nullptr;
			}
		}

		void FDSet::add(SOCKET fd)
		{
#ifdef _WIN32
			FD_SET(fd, pfd_set_);
#else
			if (fd < max_sock_fd_) {
				FD_SET(fd, pfd_set_);
			}
#endif
		}

		void FDSet::del(SOCKET fd)
		{
			FD_CLR(fd, pfd_set_);
		}

		void FDSet::zero()
		{
#ifdef _WIN32
			FD_ZERO(pfd_set_);
#else
			memset(pfd_set_, 0, fd_size_);
#endif
		}

		bool FDSet::has(SOCKET fd)
		{
			return FD_ISSET(fd, pfd_set_);
		}

		fd_set * FDSet::fdset()
		{
			return pfd_set_;
		}

		void FDSet::copy(FDSet& other)
		{
			memcpy(pfd_set_, other.fdset(), other.fd_size_);
		}

	} // namespace net
} // namespace zbluenet