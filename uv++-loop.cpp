#include <uv.h>
#include "uv++-loop.h"

__thread Uv::Loop *Uv::Loop::s_pLoop = NULL;
