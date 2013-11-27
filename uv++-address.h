#include <cstdio>
#include <cstring>
#include <arpa/inet.h>

namespace Uv
{
    class Address
    {
    private:
        #define ADDR_MAXLEN       (55)

    public:
        enum Type
        {
            Type_Unknown = AF_UNSPEC,
            Type_Ip4 = AF_INET,
            Type_Ip6 = AF_INET6,
            //Type_Unix = AF_UNIX,
        };
    private:
        union UAddress
        {
            sockaddr base;
            sockaddr_in ip4;
            sockaddr_in6 ip6;
            //sockaddr_un unix;
        };

    public:
        Address()
        {
            m_pStr = new char[ADDR_MAXLEN];
            m_peer.base.sa_family = Type_Unknown;
        }

        Address(/* [in] */ Type type,
                /* [in] */ const char *ip,
                /* [in] */ int port)
        {
            m_pStr = new char[ADDR_MAXLEN];
            Set(type, ip, port);
        }

        Address(const sockaddr *addr)
        {
            m_pStr = new char[ADDR_MAXLEN];
            Set(addr);
        }

        Address(const Address &other)
        {
            m_pStr = new char[ADDR_MAXLEN];
            Set((const sockaddr *) &other.m_peer);
        }

        ~Address()
        {
            delete[] m_pStr;
        }

        int SetIp4(/* [in] */ const char * ip,
                   /* [in] */ int port)
        {
            return Set(Type_Ip4, ip, port);
        }

        int SetIp6(/* [in] */ const char * ip,
                   /* [in] */ int port)
        {
            return Set(Type_Ip6, ip, port);
        }

        int Set(/* [in] */ Type type,
                /* [in] */ const char * ip,
                /* [in] */ int port);

        int Set(/* [in] */ const sockaddr *peer);

        Type GetType() const
        {
            return (Type) m_peer.base.sa_family;
        }

        int GetPort() const
        {
            if(Type_Ip4 == GetType()) {
                return ntohs(m_peer.ip4.sin_port);
            }
            else if(Type_Ip6 == GetType()) {
                return ntohl(m_peer.ip6.sin6_port);
            }

            assert(0);
        }

        const char * ToString() const;

        operator sockaddr *()
        {
            return (sockaddr *) &m_peer;
        }

        operator const sockaddr *() const
        {
            return (sockaddr *) &m_peer;
        }

        Address & operator =(const Address &other)
        {
            Set((sockaddr *) &other.m_peer);

            return *this;
        }

        bool operator < (const Address &other) const;

    private:
        void SetType(sa_family_t type)
        {
            m_peer.base.sa_family = type;
        }

    private:
        UAddress m_peer;

        char *m_pStr;
    };

    class Ip4Address: public Address
    {
    public:
        Ip4Address(/* [in] */ const char *ip,
                   /* [in] */ int port): Address(Address::Type_Ip4,
                                                 ip,
                                                 port)
        {
        }

        int Set(/* [in] */ const char *ip,
                /* [in] */ int port)
        {
            return SetIp4(ip, port);
        }
    };

    class Ip6Address: public Address
    {
    public:
        Ip6Address(/* [in] */ const char *ip,
                   /* [in] */ int port): Address(Address::Type_Ip6,
                                                 ip,
                                                 port)
        {
        }

        int Set(/* [in] */ const char *ip,
                /* [in] */ int port)
        {
            return SetIp6(ip, port);
        }
    };
}
