#include "uv++.h"

namespace Uv
{
    int Address::Set(/* [in] */ Type type,
                     /* [in] */ const char * ip,
                     /* [in] */ int port)
    {
        int result;
        if(Type_Ip4 == type) {
            result = uv_ip4_addr(ip, port, (sockaddr_in *) &m_peer);
        }
        else {
            result = uv_ip6_addr(ip, port, (sockaddr_in6 *) &m_peer);
        }

        return result;
    }

    int Address::Set(/* [in] */ const sockaddr *otherPeer)
    {
        switch(otherPeer->sa_family) {
        case AF_INET:
            m_peer.ip4 = * (const sockaddr_in *) otherPeer;
            break;
        case AF_INET6:
            m_peer.ip6 = * (const sockaddr_in6 *) otherPeer;
            break;
        default:
            return UV_EAFNOSUPPORT;
        }

        return 0;
    }

    const char * Address::ToString() const
    {
        if(! m_pStr) {
            return NULL;
        }

        m_pStr[0] = '\0';

        if(Type_Ip4 == GetType()) {
            uv_ip4_name((struct sockaddr_in *) &m_peer.ip4,
                        m_pStr,
                        ADDR_MAXLEN);
        }
        else if(Type_Ip6 == GetType()) {
            uv_ip6_name((struct sockaddr_in6 *) &m_peer.ip6,
                        m_pStr,
                        ADDR_MAXLEN);
        }
        int addrLen = strlen(m_pStr);
        snprintf(m_pStr + addrLen,
                 ADDR_MAXLEN - strlen(m_pStr),
                 ":%d",
                 GetPort());

        return m_pStr;
    }

    bool Address::operator < (const Address &other) const
    {
        if(this == &other) {
            return false;
        }

        if(GetType() != other.GetType()) {
            if(Type_Ip4 == GetType()) {
                return true;
            }
            return false;
        }

        if(Type_Ip4 == GetType()) {
            return -1 == memcmp(&m_peer.ip4,
                                &other.m_peer.ip4,
                                sizeof(sockaddr_in));
        }

        return -1 == memcmp(&m_peer.ip6,
                            &other.m_peer.ip6,
                            sizeof(sockaddr_in6));
    }
}
