#include <pigpio.h>

#include "PiGpioArray.h"

////////////////////////////////////////////////////////////////////////////////

PiGpioArray::PiGpioArray()
{
    // TODO: Any way to report this failing?
    gpioInitialise();
}

PiGpioArray::~PiGpioArray()
{
    // This will not return until the alert-emitting thread has
    // successfully cancelled.  The next time another instance calls
    // gpioInitialise(), the alert functions will all be cleared.
    //
    // TODO: Any way to report this failing?
    gpioTerminate();
}
