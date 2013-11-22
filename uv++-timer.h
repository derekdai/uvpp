namespace Uv
{
    class Timer: public GenericHandle<uv_timer_t>
    {
    public:
        class TimeoutHandler
        {
        public:
            virtual void OnTimeout(Timer *source, int status) = 0;
        };

    public:
        Timer(): m_pHandler(NULL)
        {
        }

        int Start(/* [in] */ TimeoutHandler &handler,
                  /* [in] */ uint64_t interval)
        {
            assert(IsOpened());

            return Start(handler, interval, interval);
        }

        int Start(/* [in] */ TimeoutHandler &handler,
                  /* [in] */ uint64_t interval,
                  /* [in] */ uint64_t delay);

        int Stop();

    protected:
        virtual ~Timer()
        {
            cout << "~Timer()" << endl;
        }

        virtual int DoOpen(uv_handle_t *peer)
        {
            return uv_timer_init(Loop::Get(), (uv_timer_t *) peer);
        }

        static void OnTimeout(uv_timer_t *peer, int status);

    private:
        TimeoutHandler *m_pHandler;
    };
}
