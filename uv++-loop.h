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
        static Loop & Get()
        {
            if(! s_pLoop) {
                s_pLoop = new Loop();
            }

            return *s_pLoop;
        }

        static int Run()
        {
            return Get().Run(Default);
        }

        int Run(RunMode mode)
        {
            return uv_run(m_pPeer, (uv_run_mode) mode);
        }

        void Stop()
        {
            uv_stop(m_pPeer);
        }

        operator uv_loop_t *()
        {
            return m_pPeer;
        }

        ~Loop()
        {
            uv_loop_delete(m_pPeer);
            s_pLoop = NULL;
        }

    private:
        Loop(): m_pPeer(uv_loop_new())
        {
            m_pPeer->data = this;
        }

        static Loop & FromPeer(uv_loop_t *peer)
        {
            return * (Loop *) peer->data;
        }

        friend class Handle;

    private:
        static __thread Loop * s_pLoop;

        uv_loop_t * m_pPeer;
    };
}

#endif /* __UVPP_LOOP_H_ */
