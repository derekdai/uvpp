#include "uv++.h"

namespace Uv
{
    void Stream::OnConnect(/* [in] */ uv_stream_t *peer,
                           /* [in] */ int status)
    {
        Stream *self = (Stream *) peer->data;
        Stream *conn = NULL;
        int result = self->Accept(&conn);
        self->m_pInConnectHandler->OnConnect(self, conn, result);

        conn->Unref();
    }
}
