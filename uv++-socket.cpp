#include "uv++.h"

namespace Uv
{
    void Socket::OnAlloc(/* [in] */ uv_handle_t *handle,
                         /* [in] */ size_t suggested_size,
                         /* [in] */ uv_buf_t *buf)
    {
        *buf = uv_buf_init(Buffer::Alloc(suggested_size), suggested_size);
    }
}
