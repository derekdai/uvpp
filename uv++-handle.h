#ifndef __UVPP_HANDLE_H_
#define __UVPP_HANDLE_H

#include <cassert>
#include <cstdlib>

#include <iostream>
using namespace std;

namespace Uv
{
    class Handle
    {
    public:
        class WeakRef
        {
        public:
            virtual void OnClose(Handle *handle) = 0;
        };

    public:
        int Open(/* [in] */ Loop &loop = Loop::Get());

        void Close();

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

        Handle * Ref()
        {
            assert(0 < m_refCount);

            m_refCount ++;

            return this;
        }

        void Unref();

        virtual ~Handle()
        {
            cout << "~Handle(): " << IsOpened() << endl;
            if(IsOpened()) {
                Close();
            }
        }

        Loop & GetLoop()
        {
            // TODO Handle should not access field of loop directly
            return Loop::FromPeer(m_pPeer->loop);
        }

    protected:
        Handle(): m_pPeer(NULL),
                  m_pWeakRef(NULL),
                  m_refCount(1)
        {
        }

        virtual int DoOpen(Loop &loop, uv_handle_t *peer) = 0;

        virtual void DoClose()
        {
        }

        static void OnClose(uv_handle_t *peer);

        uv_handle_t * GetPeer()
        {
            return m_pPeer;
        }

        virtual size_t GetPeerSize() const = 0;

    private:
        uv_handle_t *m_pPeer;

        WeakRef *m_pWeakRef;

        int m_refCount;
    };
}

#endif /* __UVPP_HANDLE_H_ */
