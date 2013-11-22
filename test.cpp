#include "uv++.h"
#include <iostream>

using namespace Uv;
using namespace std;

class MyTimeoutHandler: public Timer::TimeoutHandler
{
public:
    MyTimeoutHandler(): count(3) {}

    void OnTimeout(Timer *source, int status)
    {
        cout << count -- << endl;
        if(0 >= count) {
            source->Stop();
        }
    }

private:
    int count;
};

class MyWeakRef: public Handle::WeakRef
{
public:
    void OnClose(Handle *handle)
    {
        cout << "Timer closed" << endl;
    }
};

class MyTimer
{
public:
    MyTimer(): peer(NULL)
    {
    }

    int Open()
    {
        peer = (uv_timer_t *) malloc(sizeof(uv_timer_t));
        return uv_timer_init(uv_default_loop(), peer);
    }

    void Close()
    {
        uv_close((uv_handle_t *) peer, NULL);
        uv_run(uv_default_loop(), UV_RUN_ONCE);
        free(peer);
        peer = NULL;
    }

    ~MyTimer()
    {
        if(peer) {
            Close();
        }
    }

private:
    uv_timer_t *peer;
};

int main()
{
    //MyTimer timer;
    //assert(! timer.Open());
    //timer.Close();
    //assert(! timer.Open());
    //timer.Close();

    //uv_timer_t *timer = (uv_timer_t *) malloc(sizeof(uv_timer_t));
    //uv_timer_init(uv_default_loop(), timer);
    //uv_close((uv_handle_t *) timer, NULL);
    //free(timer);
    //timer = (uv_timer_t *) malloc(sizeof(uv_timer_t));
    //uv_timer_init(uv_default_loop(), timer);
    //uv_close((uv_handle_t *) timer, NULL);
    //free(timer);

    Timer *timer = new Timer();
    assert(! timer->Open());
    timer->Close();
    assert(! timer->Open());
    timer->Close();

    //Timer *timer = new Timer();
    //MyTimeoutHandler timeoutHandler;
    //MyWeakRef weakRef;
    //assert(! timer->Open());
    //assert(! timer->Start(timeoutHandler, 1000));
    ////timer->SetWeakRef(&weakRef);
    //timer->Unref();

    //Timer *timer2 = new Timer();
    //delete timer2;

    //Tcp *server = new Tcp();
    //assert(! server->Open());
    //delete server;

    //server = new Tcp();
    //assert(! server->Open());
    //delete server;

    Loop::Run();

    return 0;
}
