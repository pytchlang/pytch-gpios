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
pip install black pytest pytest-asyncio websockets
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


## Running

Currently this is "developer preview" status, so requires some set-up.

The main development Pytch site runs on the developer's machine.  The
GPIO interface runs on a Raspberry Pi.  The Pytch webapp uses some
features which are only available in a secure context, so the
websocket for GPIO control has to be `localhost` too.  This is
achieved with ssh port-forwarding.

* In `pytch-vm` repo, check out the `gpios-1` branch and build that
  with `npm run build`.
* In `pytch-webapp`, check out the `rpi-gpios-1` branch.  This looks
  for a websocket server on `localhost:8055`.
* Connect from the development machine to the Raspberry Pi via ssh,
  setting up a port-forward.  E.g., `ssh -L 8055:localhost:8055
  pi@rpi-pytch`.
* On the RPi, build the `websocket-gpio-server` with PiGpio enabled
  (see §"Building on the Raspberry Pi") and run it with `sudo
  ./websocket-gpio-server 8055 pigpio`.  The `sudo` is needed to
  access the GPIOs.

In a separate terminal, run the usual development server script, i.e.,
within `pytch-build/makesite/local-server` run `./dev-server.sh`.

Connect an LED and a 220R resistor in series between GPIO18 and GND of
the RPi, and a pushbutton between GPIO23 and GND.  Then a Pytch
program like

``` python
import pytch

class DoubleSnake(pytch.Sprite):
    Costumes = ["python-logo.png"]

    @pytch.when_this_sprite_clicked
    def flash_light(self):
        pytch.set_gpio_output(18, 1)
        pytch.wait_seconds(1.0)
        pytch.set_gpio_output(18, 0)

    @pytch.when_green_flag_clicked
    def adjust_size(self):
        pytch.set_gpio_as_input(23, "pull-up")
        while True:
            button_pressed = (pytch.get_gpio_value(23) == 0)
            self.set_size(2.0 if button_pressed else 1.0)
```

should then let you blink the LED by clicking on the sprite, and
control the size of the sprite with the pushbutton.


## Running self-contained on RPi400

Ensure Pytch repos are at appropriate branches:

* `pytch-webapp` on `rpi-gpios-1`
* `pytch-vm` on `gpios-2`

Update splat-mole with new commits and placeholder section in tutorial
content file.  Make a dedicated `splat-moles-only` branch (off
`release-recipes`) with just the splat-the-moles tutorial in it.  Use
that.

Create tmp WiP branch of pytch-releases with those submodule commits.
Build the deployment zipfile as usual.

Write fresh image of 64-bit "with desktop" version of Raspberry Pi OS
to an SD card.  Boot RPi with monitor and wired Ethernet attached.  It
resizes filesystem, generates SSH keys, reboots.

Set location to Ireland, Dublin, "use English".

Username "pytch", password "gpio202212".  Set screen size.  Skip WiFi
connection.  Say "yes" to downloading updates (takes a minute or so to
download and install updates).  Click "restart".

Set hostname to something unique (starting with `rpi-pytch-1`) using
the GUI config tool: World icon, preferences, Raspberry Pi
configuration.  Enable SSH under "interfaces".  Then use "set
hostname" button, reboot.  (Can now `ping rpi-pytch-1.local` from
another machine on same LAN.)

First launch of Chromium after changing hostname gives warning about
profile in use on another computer.  OK to say "remove lock and
relaunch".

Install nginx with `sudo apt install nginx`.  This didn't need
password; should it?
(https://forums.raspberrypi.com/viewtopic.php?t=169212)

Launch Chromium from menu.  Going to "localhost" shows nginx welcome
page.

Can ssh to RPi but, if you have lots of SSH keys stored, might need
`ssh -o PreferredAuthentications=password` to avoid "too many
authentication failures" error.

Copy build zip to RPi.  Unzip (with `sudo`) into `/var/www/html`.

Add file `pytch` to `/etc/nginx/sites-available` (currently this needs a
file to be edited to specify the exact beta/gAAAABBB path):

```
server {
    listen 127.0.0.1:80;
    root /var/www/html;

    location = / {
        absolute_redirect off;
        return 307 /beta/g123412341234/app/;
    }

    location ~ ^/(beta/g[0-9a-f]+)/app/ {
        try_files $uri /$1/app/index.html =404;
    }

    location / {
        try_files $uri =404;
    }
}
```

Enable with symlink from `sites-enabled`, removing `default` symlink
from `sites-enabled`.  Restart nginx with `sudo systemctl restart
nginx`.

In Chromium, go to `localhost`.

Splat-moles tutorial loads OK.  Takes a few seconds; slower than on a
desktop/laptop but still perfectly usable.  (Have yet to try actual
game.)

Launch full gpio-enabled mole game; got GPIO reset timed out (as
expected, because local WS server not running).

Make archive of this repo `git archive -o /tmp/websocket-gpios.zip
HEAD` and scp to RPi.  Unzip into fresh directory on RPi.

Install cmake and boost on RPi `sudo apt install cmake
libboost-all-dev`.  (Takes a while to install boost.  Get v1.74.)

Follow above "building on RPi" instructions.  Compiling takes a little
while (c.4min).  **TODO: Can we prepare pre-built binaries?  Are there
runtime dependencies (boost)?  ldd shows libboost_log.**

Run server with

``` shell
~/websocket-server/src/websocket-gpios/build $ sudo ./websocket-gpio-server 8055 pigpio
```

and re-green-flag project; worked.

**TODO: Keyboard on RPi is set to US; fix.**


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
```

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
