#include "uv++.h"

namespace Uv
{
    int Address::Set(/* [in] */ Type type,
                     /* [in] */ const char * ip,
                     /* [in] */ int port)
    {
        assert(Type_Unknown != type);

        int result;
        if(Type_Ip4 == type) {
            result = uv_ip4_addr(ip, port, &m_peer.ip4);
        }
        else {
            result = uv_ip6_addr(ip, port, &m_peer.ip6);
        }

        if(! result) {
            m_type = type;
        }

        return result;
    }
}
