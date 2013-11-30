#ifndef __UVPP_LOOP_H_
#define __UVPP_LOOP_H_

namespace Uv
{
    class Loop
    {
    public:
        enum RunMode {
            Default = UV_RUN_DEFAULT,
            Once    = UV_RUN_ONCE,
            NoWait  = UV_RUN_NOWAIT
        };

    public:
        static Loop & Get();

        static int Run();

        static void Free();

        int Run(/* [in] */ RunMode mode);

        bool IsRunning() const
        {
            return !! m_runLevel;
        }

        void Stop()
        {
            uv_stop(m_pPeer);
        }

        uint64_t Now()
        {
            return uv_now(*this);
        }

        int GetFd() const
        {
            return uv_backend_fd(*this);
        }

        operator uv_loop_t *()
        {
            return m_pPeer;
        }

        operator const uv_loop_t *() const
        {
            return m_pPeer;
        }

    private:
        Loop(): m_pPeer(uv_loop_new()),
                m_runLevel(0)
        {
            m_pPeer->data = this;
        }

        ~Loop()
        {
            uv_loop_delete(m_pPeer);
        }

        static Loop & FromPeer(uv_loop_t *peer)
        {
            return * (Loop *) peer->data;
        }

        friend class Handle;

    private:
        static __thread Loop * s_pLoop;

        uv_loop_t * m_pPeer;

        unsigned int m_runLevel;
    };
}

#endif /* __UVPP_LOOP_H_ */
