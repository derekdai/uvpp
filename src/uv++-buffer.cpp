#include "uv++.h"

namespace Uv
{
    Buffer * Buffer::Ref()
    {
        assert(0 < m_refCount);

        m_refCount ++;

        return this;
    }

    void Buffer::Unref()
    {
        assert(0 < m_refCount);

        m_refCount --;
        if(0 < m_refCount) {
            return;
        }

        delete this;
    }

    char * Buffer::Alloc(/* [in] */ size_t size)
    {
        return (char *) malloc(size);
    }

    void Buffer::Free(char * buf)
    {
        free(buf);
    }
}
