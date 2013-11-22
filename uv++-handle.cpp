#include "uv++.h"

namespace Uv
{
        void Handle::OnClose(uv_handle_t *peer)
        {
            Handle *self = (Handle *) peer->data;
            if(self->m_pWeakRef) {
                cout << "Handle::OnClose()" << endl;
                self->m_pWeakRef->OnClose(self);
            }
        }
}
