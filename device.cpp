#include <iostream>
#include "generic-device.h"
#include "config.h"

using namespace std;
using namespace Uv;

class Device: public GenericDevice,
              private Udp::RecvHandler,
              private Stream::InConnectHandler,
              private Timer::TimeoutHandler
{
public:
    Device(): m_pinging(false)
    {
    }

    virtual ~Device()
    {
    }

private:
    virtual int DoInit()
    {
        int result = GenericDevice::DoInit();
        if(result) {
            goto end;
        }

        result = JoinMulticastGroup(Ip4Address("0.0.0.0", DEV_UDP_PORT),
                                    DEV_UDP_MCAST_GROUP,
                                    UDP_MCAST_TTL,
                                    *this);
        if(result) {
            goto end;
        }

        result = Listen(Ip4Address("0.0.0.0", DEV_TCP_PORT), *this);
        if(result) {
            goto leaveMulticastGroup;
        }

        result = LookupRegistry();
        if(result) {
            goto stopListen;
        }

        cout << "device started up!" << endl;

        goto end;

    stopListen:
        ListenStop();
    leaveMulticastGroup:
        LeaveMulticastGroup();
    end:
        return result;
    }

    void OnConnect(/* [in] */ Stream *source,
                   /* [in] */ Stream *conn,
                   /* [in] */ int status)
    {
        cout << "New connection accepted" << endl;

        MonitorHandle(conn);
    }

    void OnRegLookupAck(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        cout << "found registry @" << addr.ToString() << endl;

        m_regAddr = addr;

        StopTimer();
        int result = StartTimer(KEEPALIVE_INTERVAL - MS(1), *this);
        if(result) {
            cout << "failed to start keep alive timer";
        }
    }

    void OnDevLookupReq(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        OnRegLookupAck(source, buf, addr);

        buf = new Buffer(ACK_DEV_LOOKUP, STRLEN(ACK_DEV_LOOKUP));
        if(! buf) {
            cout << "out of resource" << endl;
        }

        if(source->Send(*buf, addr)) {
            source->Close();
        }
        buf->Unref();
    }

    void OnPingAck(/* [in] */ Udp *source,
                   /* [in] */ Buffer *buf,
                   /* [in] */ const Address &addr)
    {
        m_pinging = false;
    }

    void OnRecv(/* [in] */ Udp *source,
                /* [in] */ Buffer *buf,
                /* [in] */ const Address &addr,
                /* [in] */ unsigned int flags,
                /* [in] */ int status)
    {
        if(status) {
            return;
        }

        if(! strncmp(REQ_DEV_LOOKUP, *buf, STRLEN(REQ_DEV_LOOKUP))) {
            OnDevLookupReq(source, buf, addr);
        }
        else if(! strncmp(ACK_REG_LOOKUP, *buf, STRLEN(ACK_REG_LOOKUP))) {
            OnRegLookupAck(source, buf, addr);
        }
        else if(! strncmp(ACK_PING, *buf, STRLEN(ACK_PING))) {
            OnPingAck(source, buf, addr);
        }
    }

    void OnTimeout(Timer *source, int status)
    {
        if(m_pinging) {
            cout << "registry @" << m_regAddr.ToString() << " down" << endl;
            StopTimer();

            m_pinging = false;
        }
        else {
            if(PingRegistry(m_regAddr)) {
                cout << "failed to ping registry" << endl;
                return;
            }

            m_pinging = true;
        }

    }

private:
    Address m_regAddr;

    bool m_pinging;
};

int main()
{
    Device *device = new Device();
    int result = device->Init();
    if(result) {
        cout << "failed to initialize device: " << GetErrorStr(result) << endl;
        return result;
    }
    device->Run();
    delete device;

    cout << Handle::count << " handles alive" << endl;

    return 0;
}
