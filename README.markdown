# OSXAutomation

A small command-line utility for automating mouse and keyboard input.

Developed for the Castanaut (http://github.com/joseph/castanaut) Ruby gem,
which lets you script a "screenplay" of interactions with Mac OS X, and
potentially record it into a screencast.

Initially developed by Joseph Pearson of Inventive Labs
(http://inventivelabs.com.au) and released under the WTFPL.


## Requirements

* Xcode


## Building and running

You can build the project in Xcode or from the commandline with the xcodebuild
utility. Two scripts are provided in the scripts directory for development
convenience. For example:

Build the utility and test it, moving the mouse to 100x100 on your screen.

    $ scripts/buildrun "mousewarp 100 100"

Run the existing built executable, typing a short message:

    $ scripts/run "type foobar"

For general use, move the utility from `build/Release/osxautomation` to
somewhere in your path (eg `/usr/local/bin`) and run like, eg:

    $ osxautomation "mouselocation"

(This example will print the mouse coordinates.)

You can chain commands with consecutive arguments:

    $ osxautomation "mousemove 300 400" "mouselocation"


## Commands

A list of the current input commands:

* `mouselocation` - prints the pixel coordinates of the mouse cursor
* `mousewarp <x> <y>` - jumps the mouse cursor to these pixel coordinates
* `mousemove <x> <y>` - steadily moves the mouse cursor to these coordinates
* `mousedown <integer>` - depresses the mouse button identified by the integer
* `mouseup <integer>`
* `mouseclick <integer>`
* `mousedoubleclick <integer>`
* `mousetripleclick <integer>`
* `mousedrag <x> <y>` - depresses the left mouse button (1) and moves the mouse
    to the given coordinates, then releases the button
* `press <keycode>` - depresses the keyboard key identified by the keycode
* `release <keycode>` - releases the key identified by keycode
* `hit <keycode>` - depresses and releases key
* `type <string>` - types out a string steadily to whatever has focus
* `wait <integer>` - pauses execution for <integer> seconds


## A code quality caveat

It works. It may not be elegant or future-proof, or accept the widest possible
variety of inputs. I'm not a Mac OS X developer, but two years ago I had an
itch and this scratched it. YMMV.

Please contribute improvements (but be aware that the command-line
interface must remain stable if I am to integrate changes - Castanaut is
dependent on this utility).


## License

Copyright (C) 2010 Inventive Labs.

Released under the WTFPL: http://sam.zoy.org/wtfpl.

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.
