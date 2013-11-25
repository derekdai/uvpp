#include "uv++.h"

namespace Uv
{
    int Address::Set(/* [in] */ Type type,
                     /* [in] */ const char * ip,
                     /* [in] */ int port)
    {
        int result;
        if(Type_Ip4 == type) {
            result = uv_ip4_addr(ip, port, &m_peer.ip4);
        }
        else {
            result = uv_ip6_addr(ip, port, &m_peer.ip6);
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
}
