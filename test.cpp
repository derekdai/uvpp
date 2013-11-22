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

class ServerEventHandler: public Stream::InConnectHandler
{
public:
    ServerEventHandler(): conn(NULL)
    {
    }

    ~ServerEventHandler()
    {
        delete conn;
    }

protected:
    virtual void OnConnect(Stream *server,
                           Stream *conn,
                           int status)
    {
        this->conn = conn->Ref();
    }

private:
    Stream *conn;
};

int main()
{
    Timer *timer = new Timer();
    MyTimeoutHandler timeoutHandler;
    MyWeakRef weakRef;
    assert(! timer->Open());
    assert(! timer->Start(timeoutHandler, 1000));
    timer->SetWeakRef(&weakRef);
    timer->Unref();

    Tcp *server = new Tcp();
    ServerEventHandler serverEventHandler;
    assert(! server->Open());
    assert(! server->Listen(serverEventHandler));
    delete server;

    server = new Tcp();
    assert(! server->Open());
    delete server;

    Loop::Run();

    return 0;
}
