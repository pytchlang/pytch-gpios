#ifndef WEBSOCKET_GPIOS_BIND_MACRO_H_
#define WEBSOCKET_GPIOS_BIND_MACRO_H_

#define BIND_FRONT_THIS(method) \
    beast::bind_front_handler(method, shared_from_this())

#define BIND_FRONT_THIS_1(method, arg) \
    beast::bind_front_handler(method, shared_from_this(), arg)

#endif // WEBSOCKET_GPIOS_BIND_MACRO_H_
