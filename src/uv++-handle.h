#ifndef __UVPP_HANDLE_H_
#define __UVPP_HANDLE_H

#include <cassert>
#include <cstdlib>

namespace Uv
{
    class Handle
    {
    public:
        class WeakRef
        {
        private:
            virtual void OnClose(Handle *handle) = 0;

            friend class Handle;
        };

    protected:
        enum Flags
        {
            Flags_None,
            Flags_Closing     = 1 << 0,
        };

    public:
        void Close();

        bool IsOpened() const
        {
            return !IsClosing() && !IsClosed();
        }

        bool IsClosing() const
        {
            return !!(Flags_Closing & m_flags);
        }

        bool IsClosed() const
        {
            return !m_pPeer && !IsClosing();
        }

        void SetWeakRef(WeakRef *weakref)
        {
            m_pWeakRef = weakref;
        }

        WeakRef* GetWeakRef() const
        {
            return m_pWeakRef;
        }

        Handle * Ref()
        {
            assert(0 < m_refCount);

            m_refCount ++;

            return this;
        }

        void Unref();

        Loop & GetLoop()
        {
            // TODO Handle should not access field of loop directly
            return Loop::FromPeer(m_pPeer->loop);
        }

        uv_handle_type GetType() const
        {
            assert(IsOpened() || IsClosing());

            return m_pPeer->type;
        }

        const char * GetTypeName() const;

    protected:
        Handle(): m_pPeer(NULL),
                  m_pWeakRef(NULL),
                  m_refCount(1),
                  m_flags(Flags_None)
        {
            count ++;
        }

        virtual ~Handle()
        {
            count --;

            assert(! IsOpened());
        }

        int Open(/* [in] */ Loop &loop);

        virtual int DoOpen(Loop &loop, uv_handle_t *peer) = 0;

        virtual void DoClose() = 0;

        virtual size_t GetPeerSize() const = 0;

        uv_handle_t * GetPeer()
        {
            return m_pPeer;
        }

        static void OnClose(uv_handle_t *peer);

    private:
        void SetFlags(Flags flags)
        {
            m_flags |= flags;
        }

        void ClearFlags(Flags flags)
        {
            m_flags &= ~flags;
        }

    private:
        uv_handle_t *m_pPeer;

        WeakRef *m_pWeakRef;

        int m_refCount;

        unsigned int m_flags;

    public:
        static int count;
    };
}

#endif /* __UVPP_HANDLE_H_ */
