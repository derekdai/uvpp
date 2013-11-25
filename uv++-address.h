namespace Uv
{
    class Address
    {
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
            m_peer.base.sa_family = Type_Unknown;
        }

        Address(/* [in] */ Type type,
                /* [in] */ const char *ip,
                /* [in] */ int port)
        {
            Set(type, ip, port);
        }

        Address(const sockaddr *addr)
        {
            Set(addr);
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

        operator sockaddr *()
        {
            return (sockaddr *) &m_peer;
        }

        operator const sockaddr *() const
        {
            return (sockaddr *) &m_peer;
        }

    private:
        void SetType(sa_family_t type)
        {
            m_peer.base.sa_family = type;
        }

    private:
        UAddress m_peer;
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
