# WebSocket server for controlling GPIOs

## Building on development machine

``` bash
cd src/websocket-gpios
mkdir -p build && cd build && cmake ..
make -j
```

See also "Testing" below.


## Building on the Raspberry Pi

Set the configuration-time variable `ENABLE_PIGPIO` to `1`, via:

``` bash
cd src/websocket-gpios
mkdir -p build && cd build && cmake -DENABLE_PIGPIO=1 ..
make -j
```

The resulting `websocket-gpio-server` must be run as `root` on the
RPi.


## Testing

As well as Catch2-based tests, end-to-end testing is done via Python.

Test set-up:

``` bash
python3 -m venv --prompt websocket-gpios-venv venv
. venv/bin/activate
pip install --upgrade pip
pip install black pytest pytest-asyncio
```

To build and run Catch2 tests:

``` bash
cd src/websocket-gpios
mkdir -p build && cd build && cmake ..
make -j && ./run-tests
```

(This will build the `websocket-gpio-server` executable also.)

To run Python tests:

* Go through the above set-up.
* Build and run the `websocket-gpio-server` executable on port 8055
  with loopback (the tests currently rely on some details of exactly
  which messages the loopback server sends).
* Run `pytest`:


``` bash
cd src/websocket-gpios/tests/python
pytest -v .
```

The async nature of the test means that test failures produce verbose
and not particularly helpful messages.


## Design overview

### IGpioArray: Interface for control of GPIOs

The interface `IGpioArray` has methods for controlling an array of
GPIOs.  Implementations are:

* `StubGpioArray`, which returns failure for all method calls.
  Looking ahead, this is the concrete class used when a client
  connects but the real GPIOs are already in use by another client.

* `LoopbackGpioArray`, which is for testing.  It mimics a set-up
  whereby pin 4 is connected to pin 5 (via a current-limiting
  resistor).

* `PiGpioArray`, which controls the real GPIOs on a Raspberry Pi.

### WebSocket server

Built out of one `WebSocketListener` — accepts connections and creates
a `ClientSession` for each one.

### WebSocketListener

Logic is spread over various methods because of the async design, but
in essence it does:

``` c++
acceptor.open_and_bind_and_listen();
while (true) {
  socket = acceptor.accept();
  client = new ClientSession(socket);
  client->run();
}
```

This omits error handling, some arguments, etc.; for details see
source.

### ClientSession

Logic is spread over various methods because of the async design, but
in essence it does:

``` c++
ws = websocket::stream(socket);
ws.accept();

json_interface_ = interface_broker->issue_json_interface(this);

while (true) {
    message = ws.read();
    reponse = json_interface_->do_commands(message);
    ws.send(response);
}

This omits ensuring operations happen on the right thread, JSON
parsing, error handling, shared-pointer details, etc.; for details see
source.

The `send()` is implemented in a thread-safe way to allow other
threads (in practice, the thread which monitors GPIOs set to input) to
also send messages.

### GpioJsonInterface

An object which accepts a string representing a list of commands,
passes them to a `IGpioArray` instance, and reports the results.

### GpioInterfaceBroker

Constructed at top-level from a (shared-pointer to) a `IGpioArray`
instance, which is the "real" set of GPIOs the broker mediates access
to.  Tracks whether it has granted access to this `IGpioArray`.

Receives requests for a `GpioJsonInterface` instance.  In response to
such a request, if nobody currently has access to the `IGpioArray`,
gives the requestor such access.  Otherwise, give a
`GpioJsonInterface` connected to a `StubGpioArray`.

Ownership is tracked via a weak pointer to an object exposing
`IMessageTransmitChannel`.  This is the requestor to which the broker
granted real (i.e., non-`StubGpioArray`) access.  In real use, this
object is a `ClientSession`, but in tests, a test helper object is
used.

### IMessageTransmitChannel

Interface with one method, `send(message)`.

### Lifetime management

A `ClientSession` is created by the `WebSocketListener` and held via a
shared-pointer.  That shared-pointer (via `shared_from_this()`) is
captured by the callbacks passed to the various async calls; the
`ClientSession` remains alive as long as an async call is pending.

When the websocket client closes the connection, the `ClientSession`
will not schedule a further async operation, and so nobody has a copy
of that shared-pointer, meaning the `ClientSession` will be destroyed.

The `ClientSession` owns the `GpioJsonInterface` via a shared-pointer.

Each `GpioJsonInterface` instance has a shared-pointer to a
`IGpioArray`.  Those `GpioJsonInterface` instances which have a
`StubGpioArray` have the only shared-pointer to that `StubGpioArray`
and so the `StubGpioArray` is destroyed when the `GpioJsonInterface`
is.  The (at most one) `GpioJsonInterface` which has a shared-pointer
to the "real" `IGpioArray` shares ownership with the
`GpioInterfaceBroker`, and so the real `IGpioArray` is never
destroyed.

### Thread monitoring input pins

The `IGpioArray` method `launch_input_monitor()` does nothing in the
`PiGpioArray` implementation because the `pigpio` library already
launches a thread.  In the `LoopbackGpioArray` implementation, a
thread is created which polls the current state of the pins and sends
messages on any change.
