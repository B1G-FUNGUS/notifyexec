# Description

A plugin inspried by [this project](https://github.com/tymm/command-execute).

This allows you to execute a command for incoming messages which are recieved
by unfocused conversations (note that focus might not be tracked properly by
all libpurple clients).

You can use environment variables by executing the command withing a shell (ie;
`bash -e '<command>'`). The reason for using a shell is to avoid any jankiness
by trying to figure out how the user wants to do things, but this can be
changed if there is enough demmand.

# Building

The command I use is `gcc notifyexec.c -Wall -fpic `pkg-config --cflags
glib-2.0` -shared -o notifyexec.so`, though of course you may have to change
this to work with your system. I do not believe there is any platform specific
code in here since it depends on glib, but be sure to let me know of any
issues.

# TODO

Right now this plugin is pretty bare bones, but I don't mind extending it if
other people need, so feel free to leave any desired features as an issue. I'm
starting it off bare bones because I do not want to maintain features that
people are not going to use.
