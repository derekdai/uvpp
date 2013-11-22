namespace Uv
{
    class Address
    {
    public:
        enum Type
        {
            Type_Unknown,
            Type_Ip4,
            Type_Ip6,
        };
    private:
        union UAddress
        {
            sockaddr_in ip4;
            sockaddr_in6 ip6;
        };

    public:
        Address(): m_type(Type_Unknown)
        {
        }

        Address(/* [in] */ Type type,
                /* [in] */ const char *ip,
                /* [in] */ int port)
        {
            Set(type, ip, port);
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

        Type GetType() const
        {
            return m_type;
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
        Type m_type;

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
