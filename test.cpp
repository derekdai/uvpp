#include "uv++.h"
#include <set>
#include <iostream>

using namespace Uv;
using namespace std;

class MyTimeoutHandler: public Timer::TimeoutHandler,
                        public Handle::WeakRef
{
public:
    MyTimeoutHandler(): count(2) {}

    void AddHandle(Handle *handle)
    {
        handles.insert(handle);
        handle->SetWeakRef(this);
    }

    void CloseHandles()
    {
        set<Handle *>::iterator iter = handles.begin();
        set<Handle *>::iterator end = handles.end();
        for(; iter != end; ++iter) {
            cout << "MyTimeoutHandler::CloseHandles: " << (*iter)->GetTypeName()
                 << "(" << *iter << ")" << endl;
            (*iter)->SetWeakRef(NULL);
            (*iter)->Close();
            handles.erase(iter);
            cout << "MyTimeoutHandler::handles.size = " << handles.size() << endl;
        }
    }

private:
    void OnTimeout(Timer *source, int status)
    {
        if(status) {
            cout << GetErrorStr(status) << endl;
            return;
        }

        cout << "MyTimeoutHandler: " << count << endl;
        count --;
        if(0 >= count) {
            source->Stop();
            CloseHandles();
        }
    }

private:
    void OnClose(Handle *handle)
    {
        set<Handle *>::iterator iter = handles.find(handle);
        if(handles.end() == iter) {
            return;
        }

        cout << "Unwatching " << *iter << endl;
        handles.erase(iter);
    }

private:
    int count;

    set<Handle *> handles;
};

MyTimeoutHandler timeoutHandler;

class Server: public Stream::InConnectHandler,
                     Stream::RecvHandler,
                     Stream::SendHandler
{
private:
    virtual void OnConnect(Stream *server,
                           Stream *conn,
                           int status)
    {
        cout << "Server::OnConnect (" << conn << ")" << endl;

        timeoutHandler.AddHandle(conn);
        conn->RecvStart(*this);
    }

    virtual void OnRecv(Stream *conn, Buffer *buf, int status)
    {
        cout << "Server Recv: " << string(buf->GetBase(), buf->GetSize()) << endl;

        cout << "Server Send: World" << endl;
        buf = new Buffer("World");
        conn->Send(buf, this);
        buf->Unref();

        conn->RecvStop();
    }

    virtual void OnSend(Stream *conn, int status)
    {
        conn->Close();

        cout << "Disconnected (" << conn << ")" << endl;
    }
};

class Client: public Tcp::OutConnectHandler,
                     Stream::SendHandler,
                     Stream::RecvHandler
{
public:
    Client(): count(0)
    {
    }

private:
    void OnConnected(Tcp *conn, int status)
    {
        if(status) {
            cout << "Clinet::OnConnected: " << GetErrorStr(status) << endl;
            return;
        }

        cout << "Client Send: Hello" << endl;

        Buffer *buf = new Buffer("Hello");
        conn->Send(buf, this);
        buf->Unref();

        conn->RecvStart(*this);
    }

    void OnSend(Stream *conn, int status)
    {
        Inc(conn);
    }

    void OnRecv(Stream *conn, Buffer *buf, int status)
    {
        cout << "Client Recv: " << string(buf->GetBase(), buf->GetSize()) << endl;
        Inc(conn);
    }

    void Inc(Stream *conn)
    {
        count ++;
        if(2 == count) {
            conn->Close();
        }
    }

private:
    int count;
};

class SignalHandler: public Signal::SignalHandler
{
private:
    void OnSignal(Signal *source, int signum)
    {
        cout << "Bye" << endl;

        source->Close();
        source->GetLoop().Stop();
    }
};

class UdpPingServer: public Udp::RecvHandler
{
private:
    void OnRecv(Udp *source,
                Buffer *buf,
                const Address &addr,
                unsigned int flags,
                int status)
    {
        cout << "UdpServer Recv: " << string(buf->GetBase(), buf->GetSize()) << endl;

        source->Close();
    }
};

class UdpPingClient: public Udp::RecvHandler
{
public:
    void OnRecv(Udp *source,
                Buffer *buf,
                const Address &addr,
                unsigned int flags,
                int status)
    {
        cout << "UdpClient Recv: " << string(buf->GetBase(), buf->GetSize()) << endl;

        source->Close();
    }
};

int main()

{
    SignalHandler signalHandler;
    Signal *signal = Signal::New();
    timeoutHandler.AddHandle(signal);
    assert(! signal->Start(SIGINT, signalHandler));
    signal->Unref();
    signal = Signal::New();
    timeoutHandler.AddHandle(signal);
    assert(! signal->Start(SIGTERM, signalHandler));
    signal->Unref();

    Server serverEventHandler;
    Tcp *server = Tcp::New();
    timeoutHandler.AddHandle(server);
    assert(! server->Bind(Ip4Address("0.0.0.0", 1234)));
    assert(! server->Listen(serverEventHandler));
    server->Unref();

    Client clientEventHandler;
    Tcp *client = Tcp::New();
    timeoutHandler.AddHandle(client);
    assert(! client->Connect(Ip4Address("127.0.0.1", 1234), &clientEventHandler));
    client->Unref();

    UdpPingServer pingServer;
    Udp *udpServer = Udp::New();
    assert(udpServer);
    timeoutHandler.AddHandle(udpServer);
    assert(! udpServer->Bind(Ip4Address("0.0.0.0", 1357)));
    assert(! udpServer->RecvStart(pingServer));

    UdpPingClient pingClient;
    Udp *udpClient = Udp::New();
    assert(udpClient);
    timeoutHandler.AddHandle(udpClient);
    assert(! udpClient->Bind(Ip4Address("0.0.0.0", 2468)));
    assert(! udpClient->RecvStart(pingClient));

    Buffer *buf = new Buffer("Hello");
    udpServer->Send(*buf, Ip4Address("127.0.0.1", 2468));
    buf->Unref();

    buf = new Buffer("World");
    udpClient->Send(*buf, Ip4Address("127.0.0.1", 1357));
    buf->Unref();

    udpServer->Unref();
    udpClient->Unref();

    Timer *timer = Timer::New();
    timeoutHandler.AddHandle(timer);
    assert(timer);
    assert(! timer->Start(timeoutHandler, 1000));
    timer->Unref();

    Loop::Run();

    Loop::Free();
    timeoutHandler.CloseHandles();

    cout << Handle::count << " handle alive" << endl;

    return 0;
}
