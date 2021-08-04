#include <zbluenet/net/socket_address.h>

#include <zbluenet/net/platform.h>

#include <cstddef>
#include <cstring>

namespace zbluenet {
	namespace net {

		class SocketAddress::Impl {
		public:
			using Protocol = SocketAddress::Protocol;
			typedef union native_address_tag {
				struct sockaddr_in ipv4_addr_;
				struct sockaddr_in6 ipv6_addr_;
			} NativeAddress;

			Impl();
			Impl(const std::string &ip, uint16_t port);
			~Impl();

			const std::string &getIp() const;
			uint16_t getPort() const;
			Protocol::type getProtocol() const;
			void setAddress(const std::string &ip, uint16_t port);

			const void *getNativeAddress() const;
			size_t getNativeAddressSize() const;
			bool setNativeAddress(const void *native_addr);

		public:
			void translateNativeAddressToAddress() const;
			void translateAddressToNativeAddress() const;

		private:
			mutable bool has_addr_;
			mutable std::string ip_;
			mutable uint16_t port_;

			mutable bool has_native_addr_;
			mutable Protocol::type protocol_;
			mutable NativeAddress native_addr_;
			mutable size_t native_addr_size_;
		};

		SocketAddress::Impl::Impl():
			has_addr_(false),
			ip_(""),
			port_(0),
			has_native_addr_(false),
			protocol_(Protocol::UNKNOWN),
			native_addr_size_(0)
		{

		}

		SocketAddress::Impl::Impl(const std::string &ip, uint16_t port):
			has_addr_(true), ip_(ip), port_(port),
			has_native_addr_(false), protocol_(Protocol::UNKNOWN), native_addr_size_(0)
		{

		}

		SocketAddress::Impl::~Impl()
		{

		}

		const std::string &SocketAddress::Impl::getIp() const
		{
			if (!has_addr_ && has_native_addr_) {
				translateNativeAddressToAddress();
			}
			return ip_;
		}

		uint16_t SocketAddress::Impl::getPort() const
		{
			if (!has_addr_ && has_native_addr_) {
				translateNativeAddressToAddress();
			}
			return port_;
		}

		SocketAddress::Protocol::type SocketAddress::Impl::getProtocol() const
		{
			if (!has_native_addr_ && has_addr_) {
				translateAddressToNativeAddress();
			}
			return protocol_;
		}

		void SocketAddress::Impl::setAddress(const std::string &ip, uint16_t port)
		{
			ip_ = ip;
			port_ = port;

			has_addr_ = true;
			has_native_addr_ = false;
		}

		const void *SocketAddress::Impl::getNativeAddress() const
		{
			if (!has_native_addr_ && has_addr_) {
				translateAddressToNativeAddress();
			}
			if (Protocol::IP_V4 == protocol_) {
				return &native_addr_.ipv4_addr_;
			} else if (Protocol::IP_V6 == protocol_) {
				return &native_addr_.ipv6_addr_;
			}
			return nullptr;
		}

		size_t SocketAddress::Impl::getNativeAddressSize() const
		{
			if (!has_native_addr_ && has_addr_) {
				translateAddressToNativeAddress();
			}
			return native_addr_size_;
		}

		bool SocketAddress::Impl::setNativeAddress(const void *native_addr)
		{
			struct sockaddr *sock_addr = (struct sockaddr *)native_addr;
			if (AF_INET == sock_addr->sa_family) {
				protocol_ = Protocol::IP_V4;
				native_addr_size_ = sizeof(struct sockaddr_in);
			} else if (AF_INET6 == sock_addr->sa_family) {
				protocol_ = Protocol::IP_V6;
				native_addr_size_ = sizeof(struct sockaddr_in6);
			} else {
				return false;
			}

			::memcpy(&native_addr_, native_addr, native_addr_size_);

			has_addr_ = false;
			has_native_addr_ = true;

			return true;
		}

		void SocketAddress::Impl::translateNativeAddressToAddress() const
		{
			ip_.clear();
			port_ = 0;

			if (Protocol::IP_V4 == protocol_) {
				struct sockaddr_in *sock_addr4 = &native_addr_.ipv4_addr_;
				char ip[32];
				if (::inet_ntop(AF_INET, &sock_addr4->sin_addr, ip, sizeof(ip)) == nullptr) {
					return;
				}
				ip_ = ip;
				port_ = ntohs(sock_addr4->sin_port);
			} else if (Protocol::IP_V6 == protocol_) {
				struct sockaddr_in6 *sock_addr6 = &native_addr_.ipv6_addr_;
				char ip[128];
				if (::inet_ntop(AF_INET6, &sock_addr6->sin6_addr,
					ip, sizeof(ip)) == nullptr) {
					return;
				}
				ip_ = ip;
				port_ = ntohs(sock_addr6->sin6_port);
			} else {
				return;
			}

			has_addr_ = true;
		}

		void SocketAddress::Impl::translateAddressToNativeAddress() const
		{
			::memset(&native_addr_, 0, sizeof(native_addr_));
			protocol_ = Protocol::UNKNOWN;
			native_addr_size_ = 0;

			if (ip_.find(".") != ip_.npos) {
				struct sockaddr_in *sock_addr4 = &native_addr_.ipv4_addr_;
				if (::inet_pton(AF_INET, ip_.c_str(), &sock_addr4->sin_addr) != 1) {
					return;
				}
				sock_addr4->sin_family = AF_INET;
				sock_addr4->sin_port = htons(port_);
				protocol_ = Protocol::IP_V4;
				native_addr_size_ = sizeof(struct sockaddr_in);
			} else if (ip_.find(":") != ip_.npos) {
				struct sockaddr_in6 *sock_addr6 = &native_addr_.ipv6_addr_;
				if (::inet_pton(AF_INET6, ip_.c_str(), &sock_addr6->sin6_addr) != 1) {
					return;
				}
				sock_addr6->sin6_family = AF_INET6;
				sock_addr6->sin6_port = htons(port_);
				protocol_ = Protocol::IP_V6;
				native_addr_size_ = sizeof(struct sockaddr_in6);
			} else {
				return;
			}

			has_native_addr_ = true;
		}

		//////////////////////////////////////////////////////////////////////////
		SocketAddress::SocketAddress() :
			pimpl_(new Impl())
		{
		}

		SocketAddress::SocketAddress(const std::string &ip, uint16_t port) :
			pimpl_(new Impl(ip, port))
		{
		}

		SocketAddress::SocketAddress(const SocketAddress &copy) :
			pimpl_(new Impl(*copy.pimpl_))
		{
		}

		SocketAddress::~SocketAddress()
		{
		}

		SocketAddress &SocketAddress::operator=(const SocketAddress &rhs)
		{
			if (this == &rhs) {
				return *this;
			}

			*pimpl_ = *rhs.pimpl_;

			return *this;
		}

		const std::string &SocketAddress::getIp() const
		{
			return pimpl_->getIp();
		}

		uint16_t SocketAddress::getPort() const
		{
			return pimpl_->getPort();
		}

		SocketAddress::Protocol::type SocketAddress::getProtocol() const
		{
			return pimpl_->getProtocol();
		}

		void SocketAddress::setAddress(const std::string &ip, uint16_t port)
		{
			return pimpl_->setAddress(ip, port);
		}

		const void *SocketAddress::getNativeAddress() const
		{
			return pimpl_->getNativeAddress();
		}

		size_t SocketAddress::getNativeAddressSize() const
		{
			return pimpl_->getNativeAddressSize();
		}

		bool SocketAddress::setNativeAddress(const void *native_addr)
		{
			return pimpl_->setNativeAddress(native_addr);
		}

		bool SocketAddress::getAddressByDomain(const std::string &domain,
			std::vector<SocketAddress> *addr_list)
		{
			struct addrinfo *res = NULL;
			struct addrinfo hints;
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;

			int ret = ::getaddrinfo(domain.c_str(), NULL, &hints, &res);
			if (ret != 0) {
				return false;
			}

			for (const struct addrinfo *p = res; p != NULL; p = p->ai_next) {
				SocketAddress addr;
				addr.setNativeAddress(p->ai_addr);
				addr_list->push_back(addr);
			}

			::freeaddrinfo(res);

			return true;
		}

		bool SocketAddress::ipV4TextToBin(const std::string &text, uint32_t *binary)
		{
			struct in_addr addr;
			::memset(&addr, 0, sizeof(addr));
			if (::inet_pton(AF_INET, text.c_str(), &addr) != 1) {
				return false;
			}

			*binary = ntohl(addr.s_addr);

			return true;
		}

		void SocketAddress::ipV4BinToText(uint32_t binary, std::string *text)
		{
			struct in_addr addr;
			::memset(&addr, 0, sizeof(addr));
			addr.s_addr = htonl(binary);

			char ip[32];
			if (::inet_ntop(AF_INET, &addr, ip, sizeof(ip)) == NULL) {
				return;
			}

			*text = ip;
		}

		std::string SocketAddress::ipV4BinToText(uint32_t binary)
		{
			std::string text;
			ipV4BinToText(binary, &text);
			return text;
		}

		bool SocketAddress::ipV6TextToBin(const std::string &text, uint8_t binary[16])
		{
			struct in6_addr addr;
			::memset(&addr, 0, sizeof(addr));
			if (::inet_pton(AF_INET6, text.c_str(), &addr) != 1) {
				return false;
			}

			for (int i = 0; i < 16; ++i) {
				binary[i] = addr.s6_addr[i];
			}

			return true;
		}

		void SocketAddress::ipV6BinToText(const uint8_t binary[16], std::string *text)
		{
			struct in6_addr addr;
			::memset(&addr, 0, sizeof(addr));
			for (int i = 0; i < 16; ++i) {
				addr.s6_addr[i] = binary[i];
			}

			char ip[128];
			if (::inet_ntop(AF_INET, &addr, ip, sizeof(ip)) == NULL) {
				return;
			}

			*text = ip;
		}

		std::string SocketAddress::ipV6BinToText(const uint8_t binary[16])
		{
			std::string text;
			ipV6BinToText(binary, &text);
			return text;
		}

	} // namespace net
} // namespace zbluenet