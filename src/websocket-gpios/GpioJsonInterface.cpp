#include "GpioJsonInterface.h"

////////////////////////////////////////////////////////////////////////////////

GpioJsonInterface::GpioJsonInterface(std::shared_ptr<IGpioArray> gpios)
    : gpios_(gpios)
{
}
