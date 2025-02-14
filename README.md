# c-fdg
A simple implementation of a `force-directed graph` with C and SDL3

## Prerequisites
Make sure you have the [SDL3](https://wiki.libsdl.org/SDL3/FrontPage) Library installed on your system.

### Building SDL3
Information about building SDL3 with `cmake` can be found [here](https://github.com/libsdl-org/SDL/blob/main/docs/README-cmake.md).

Here's what I did in order to build and install it on my system:

```bash
# Clone the repo
git clone https://github.com/libsdl-org/SDL.git --depth=1 

# Navigate to cloned repo and prepare build folder
cd SDL
mkdir build
cd build

# Set CMake Configuration 
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build SDL in release mode
cmake --build . --config Release --parallel

# Install SDL on your system
sudo make install
```

## Acknowledgments
- [SDL3 Wiki](https://wiki.libsdl.org/SDL3/FrontPage)
- [Simple Force Directed Graph Layout Livecoding](https://www.youtube.com/watch?v=PTBuq0CXpWs&ab_channel=DavidDobervich)
- [C Dynamic Array Library](https://github.com/gustavopezzi/dynamicarray)
