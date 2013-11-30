#include <uv++.h>
#include <iostream>

#include "daemon.h"
#include "generic-device.h"
#include "config.h"

using namespace std;
using namespace Uv;

class DeviceInfo
{
public:
    DeviceInfo(): m_timestamp(0)
    {
    }

    DeviceInfo(/* [in] */ const Address &addr,
               /* [in] */ uint64_t timestamp = 0): m_timestamp(0)
    {
    }

    const Address & GetAddress() const
    {
        return m_addr;
    }

    bool IsExpired(uint64_t now) const
    {
        return KEEPALIVE_INTERVAL < (now - m_timestamp);
    }

    void Ping(uint64_t now)
    {
        m_timestamp = now;
    }

    bool operator() (const DeviceInfo &l, const DeviceInfo &r) const
    {
        return l.m_addr < r.m_addr;
    }

    bool operator() (const DeviceInfo &l, const Address &r) const
    {
        return l.m_addr < r;
    }

private:
    Address m_addr;

    uint64_t m_timestamp;
};

class Registry: public GenericDevice,
                private Udp::RecvHandler,
                private Udp::SendHandler,
                private Stream::InConnectHandler,
                private Timer::TimeoutHandler
{
public:
    Registry()
    {
    }

    virtual ~Registry()
    {
    }

private:
    virtual int DoInit()
    {
        int result = GenericDevice::DoInit();
        if(result) {
            goto end;
        }

        result = JoinMulticastGroup(Ip4Address("0.0.0.0", REG_UDP_PORT),
                                    REG_UDP_MCAST_GROUP,
                                    UDP_MCAST_TTL,
                                    *this);
        if(result) {
            goto end;
        }

        result = Listen(Ip4Address("0.0.0.0", REG_TCP_PORT), *this);
        if(result) {
            goto leaveMulticastGroup;
        }

        //result = LookupRegistry(this);
        //if(result) {
            //goto stopListen;
        //}

        result = LookupDevices();
        if(result) {
            goto stopListen;
        }

        cout << "registry started up!" << endl;

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

    void OnSend(/* [in] */ Udp *source, int status)
    {
    }

    void OnRegLookupAck(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        cout << "found another registry @" << addr.ToString() << ", bye!" << endl;

        // TODO this causes issue
        Stop();
    }

    void OnDevLookupAck(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        set<DeviceInfo>::iterator iter = m_devices.find(addr);
        if(iter != m_devices.end()) {
            return;
        }

        if(! m_devices.size()) {
            StartTimer(KEEPALIVE_INTERVAL, *this);
        }

        cout << "found device @" << addr.ToString() << endl;
        m_devices.insert(DeviceInfo(addr, Loop::Get().Now()));
    }

    void OnPing(/* [in] */ Udp *source,
                   /* [in] */ Buffer *buf,
                   /* [in] */ const Address &addr)
    {
        if(AckPingRegistry(addr)) {
            cout << "failed to ack ping from device @" << addr.ToString() << endl;
        }

        set<DeviceInfo>::iterator iter = m_devices.find(addr);
        const_cast<DeviceInfo &>(*iter).Ping(source->GetLoop().Now());
    }

    void OnRegLookupReq(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        OnDevLookupAck(source, buf, addr);

        buf = new Buffer(ACK_REG_LOOKUP, STRLEN(ACK_REG_LOOKUP));
        if(! buf) {
            cout << "out of resource" << endl;
        }

        if(source->Send(*buf, addr)) {
            source->Close();
        }
        buf->Unref();
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

        if(! strncmp(REQ_REG_LOOKUP, *buf, STRLEN(REQ_REG_LOOKUP))) {
            OnRegLookupReq(source, buf, addr);
        }
        else if(! strncmp(ACK_REG_LOOKUP, *buf, STRLEN(ACK_REG_LOOKUP))) {
            OnRegLookupAck(source, buf, addr);
        }
        else if(! strncmp(ACK_DEV_LOOKUP, *buf, STRLEN(ACK_DEV_LOOKUP))) {
            OnDevLookupAck(source, buf, addr);
        }
        else if(! strncmp(REQ_PING, *buf, STRLEN(REQ_PING))) {
            OnPing(source, buf, addr);
        }
    }

    void OnTimeout(/* [in] */ Timer *source, int status)
    {
        if(status) {
            return;
        }

        uint64_t now = source->GetLoop().Now();
        set<DeviceInfo>::iterator curr = m_devices.begin();
        set<DeviceInfo>::iterator end = m_devices.end();
        for(; curr != end; ++ curr) {
            if(! (*curr).IsExpired(now)) {
                continue;
            }

            cout << "device down" << endl; 
            m_devices.erase(curr);
        }

        if(! m_devices.size()) {
            StopTimer();
        }
    }

private:
    set<DeviceInfo, DeviceInfo> m_devices;
};

int main()
{
    Registry *registry = new Registry();
    int result = registry->Init();
    if(result) {
        cout << "failed to initialize registry: " << GetErrorStr(result) << endl;
        return result;
    }
    registry->Run();
    delete registry;

    cout << Handle::count << " handles alive" << endl;

    return 0;
}
