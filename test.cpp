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

class Server: public Stream::InConnectHandler
{
public:
    Server(): conn(NULL)
    {
    }

    ~Server()
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

class Client: public Tcp::OutConnectHandler
{
public:
    void OnConnected(Tcp *source, int status)
    {
        cout << "Client::Connected" << endl;
    }
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
    Server serverEventHandler;
    assert(! server->Open());
    assert(! server->Bind(Address::Type_Ip4, "0.0.0.0", 1234));
    assert(! server->Listen(serverEventHandler));
    server->Unref();

    Tcp client;
    Client clientEventHandler;
    assert(! client.Open());
    assert(! client.Connect(Address::Type_Ip4, "127.0.0.1", 1234, &clientEventHandler));

    Loop::Run();

    return 0;
}
