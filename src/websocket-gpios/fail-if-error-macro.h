#ifndef WEBSOCKET_GPIOS_FAIL_IF_ERROR_MACRO_H_
#define WEBSOCKET_GPIOS_FAIL_IF_ERROR_MACRO_H_

#define FAIL_AND_RETURN_IF_EC(ec, origin) \
    do                                    \
    {                                     \
        if (ec)                           \
        {                                 \
            fail(ec, origin);             \
            return;                       \
        }                                 \
    } while (0)

#endif // WEBSOCKET_GPIOS_FAIL_IF_ERROR_MACRO_H_
