#ifndef __CONFIG_H_
#define __CONFIG_H_

#define REG_UDP_PORT            19999
#define REG_UDP_MCAST_GROUP     "230.230.230.230"
#define REG_TCP_PORT            20000

#define DEV_UDP_PORT            29999
#define DEV_UDP_MCAST_GROUP     "231.231.231.231"
#define DEV_TCP_PORT            30000

#define UDP_MCAST_TTL           32

#define REQ_REG_LOOKUP          "RegLookup"
#define ACK_REG_LOOKUP          "RegLookupAck"
#define REQ_DEV_LOOKUP          "DevLookup"
#define ACK_DEV_LOOKUP          "DevLookupAck"
#define REQ_PING                "Ping"
#define ACK_PING                "PingAck"

#define MS_PER_SEC              ((uint64_t) 1000)
#define US_PER_SEC              (1000 * MS_PER_SEC)
#define MS(s)                   ((s) * MS_PER_SEC)
#define US(s)                   ((s) * US_PER_SEC)
#define KEEPALIVE_INTERVAL      (MS(2))

#define STRLEN(s)               (sizeof(s) - 1)

#endif /* __CONFIG_H_ */
