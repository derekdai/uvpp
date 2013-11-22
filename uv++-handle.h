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
        public:
            virtual void OnDestroy(Handle *handle) = 0;
        };

    public:
        int Open()
        {
            assert(! IsOpened());

            size_t peerSize = GetPeerSize();
            assert(sizeof(uv_handle_t) <= peerSize);
            m_pPeer = (uv_handle_t *) malloc(peerSize);
            m_pPeer->data = this;

            int result = DoOpen(m_pPeer);

            if(result) {
                free(m_pPeer);
                m_pPeer = NULL;
            }

            return result;
        }

        void Close()
        {
            assert(! IsOpened());

            uv_close(m_pPeer, OnClose);
            free(m_pPeer);
        }

        bool IsOpened()
        {
            return NULL != m_pPeer;
        }

        void SetWeakRef(WeakRef *weakref)
        {
            m_pWeakRef = weakref;
        }

        WeakRef* GetWeakRef()
        {
            return m_pWeakRef;
        }

        void Ref()
        {
            assert(0 < m_refCount);

            m_refCount ++;
        }

        void Unref()
        {
            assert(0 < m_refCount);

            m_refCount --;
            if(m_refCount) {
                return;
            }

            delete this;
        }

    protected:
        Handle(): m_pPeer(NULL), m_pWeakRef(NULL), m_refCount(1)
        {
        }

        virtual ~Handle()
        {
            if(! IsOpened()) {
                Close();
            }
        }

        virtual int DoOpen(uv_handle_t *peer) = 0;

        static void OnClose(uv_handle_t *peer)
        {
            Handle *self = (Handle *) peer->data;
            if(self->m_pWeakRef) {
                self->m_pWeakRef->OnDestroy(self);
            }
        }

    private:
        virtual size_t GetPeerSize() = 0;

    protected:
        uv_handle_t *m_pPeer;

    private:
        WeakRef *m_pWeakRef;

        int m_refCount;
    };

    template <class T>
    class GenericHandle: public Handle
    {
    protected:
        virtual size_t GetPeerSize()
        {
            return sizeof(T);
        }

        T * GetPeer()
        {
            return (T *) m_pPeer;
        }
    };
}

#endif /* __UVPP_HANDLE_H_ */
