# README #

This project provides a remote keyboard implementation, similar to [Unified Remote](https://www.unifiedremote.com/). The main difference is that it is between a Linux client and a Windows Host. All communication happens over TCPIP.

The Linux client's keyboard will be used to control the Windows computer almost seamlessly. It is even possible to play video games using this program.

The Server uses the Windows virtual keyboard interface to emulate key presses and releases. 

There are two main modes of functioning, one completely in the terminal, and one that relies on using the X server via the xcb libraries. This functionality is toggled via the `-x` and `--xtended-features` flag. If using in the terminal, several keys and key chords (such as those that require Ctrl/Alt/etc.) may not work since they are associated with interrupts at the tty layer. 

Here are the main remappings: 

* `Ctrl-u` (undo) on the client side is mapped to `Ctrl-z` on the windows side.
* `Ctrl-y` (yank) on the client side is mapped to `Ctrl-c` on the windows side.


## Extended Features ##

The above problem is overcome when using the X server and the `-x` flag. The program will spawn a blank X window which will capture all key presses/releases and key chords. When the server also has this flag enabled, it allows the usage of the keys in an almost native manner. All key chords and most special keys work (Ctrl, Alt, Del, Home, PgUp, PgDwn, etc.), and the server is also capable of emulating key presses and releases. Thus, this makes it viable for playing videogames and other keyboard heavy programs.

## Flags to pass: ##

* `-i <ip address>`: IP address of the server
* `--ip-address <ip address`: Same as above.
* `-p <number>`: specify which TCPIP port to use.
* `--port <number>`: same as above
* `-x`: required flag if the host specifies it, otherwise this program will send garbage. This allows almost all the keyboard keys to be used natively (for example, Ctrl, Alt, Shift, Home, End, etc, and in chords).
* `--xtended-features`: same as above