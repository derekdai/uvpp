#include <cstring>

namespace Uv
{
    class Buffer
    {
    private:
        enum Flags
        {
            None        = 0,
            Writable    = 1 << 1, 
            Owned       = 1 << 2,
            Locked      = 1 << 3,
        };

    public:
        Buffer(): m_refCount(1),
                  m_flags(None)
        {
            m_peer = (uv_buf_t) { .base = NULL, .len = 0 };
        }

        Buffer(size_t size): m_refCount(1),
                             m_flags(Writable | Owned)
        {
            m_peer = (uv_buf_t) { .base = Alloc(size), .len = size };
        }

        Buffer(/* [in] */ const char * str,
               /* [in] */ ssize_t len = -1): m_refCount(1),
                                             m_flags(None)
        {
            assert(str);

            m_peer = uv_buf_init((char *) str, -1 >= len ? strlen(str) : len);
        }

        Buffer * Ref();

        void Unref();

        bool IsOwned() const
        {
            return !! (m_flags & Owned);
        }

        bool IsWritable() const
        {
            return !! (m_flags & Writable);
        }

        bool IsLocked() const
        {
            return !! (m_flags & Locked);
        }

        void Lock()
        {
            assert(! IsLocked());

            m_flags |= Locked;
        }

        void Unlock()
        {
            assert(IsLocked());

            m_flags &= ~ Locked;
        }

        size_t GetSize() const
        {
            return m_peer.len;
        }

        const char * GetBase() const
        {
            return m_peer.base;
        }

        char * GetBase()
        {
            assert(IsWritable() && ! IsLocked());

            return m_peer.base;
        }

        operator const char * () const
        {
            return m_peer.base;
        }

        operator char * ()
        {
            assert(IsWritable() && ! IsLocked());

            return m_peer.base;
        }

        operator const uv_buf_t * () const
        {
            return &m_peer;
        }

        static char * Alloc(/* [in] */ size_t size);

        static void Free(char * buf);

    protected:
        /**
         * This construct takes the ownership of base, so it's Buffer's
         * duty to free base
         */
        Buffer(/* [in] */ const uv_buf_t *otherPeer,
               /* [in] */ ssize_t nread): m_refCount(1),
                                          m_flags(Writable | Owned)
        {
            assert(otherPeer);

            m_peer = uv_buf_init(nread < 0 ? NULL : otherPeer->base,
                                 nread < 0 ? 0 : nread);
        }

        friend class Stream;
        friend class Udp;

    private:
        ~Buffer()
        {
            if(IsOwned()) {
                Free(m_peer.base);
            }
        }

    private:
        uv_buf_t m_peer;

        int m_refCount;

        unsigned int m_flags;
    };
}
