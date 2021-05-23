# Desktop Raine

![Raine!](preview.gif)

Raine from TwoKinds, right on your desktop! Made using C and SDL2. Currently in very early development. Artwork by boe from the Raine Fanclub server.

## Build instructions

### Linux/Unix

#### Dependencies
* SDL2
* A C compiler (such as gcc)
* Make

#### Targets
* `make` - Gives you a binary called "raine"
* `make run` - Builds and runs raine
* `make clean` - Removes the binary
* `make install` - Imbues your system with raine
* `make un-` Why would you need an uninstall target? Stop asking questions.

### Windows

You will need to install `Mingw-w64`, and run win-build.bat. You will end up with a file called `raine.exe`.

If you want to install it somewhere, you will need to copy the `img` folder and `SDL2.dll` to the same directory as the exe.

An installer wizard is possibly on the horizon.