#include "uv++.h"
#include <iostream>

using namespace Uv;
using namespace std;

class MyTimeoutHandler: public Timer::TimeoutHandler
{
public:
    void OnTimeout(Timer *source, int status)
    {
        cout << '.' << endl;
    }
};

int main()
{
    Timer timer;
    MyTimeoutHandler timeoutHandler;
    assert(! timer.Open());
    assert(! timer.Start(timeoutHandler, 1000));

    Loop::Run();

    return 0;
}
