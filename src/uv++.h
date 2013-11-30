#ifndef __UVPP_H_
#define __UVPP_H_

#include <iostream>
using namespace std;

#include <uv.h>
#include "uv++-loop.h"
#include "uv++-handle.h"
#include "uv++-timer.h"
#include "uv++-signal.h"
#include "uv++-address.h"
#include "uv++-buffer.h"
#include "uv++-socket.h"
#include "uv++-stream.h"
#include "uv++-tcp.h"
#include "uv++-udp.h"

namespace Uv
{
    inline const char * GetErrorStr(int error)
    {
        return uv_strerror(error);
    }
}

#endif /* __UVPP_H_ */
