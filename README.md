# Mana - Mana's Also Not (an) Ancronym

## Getting Started

These instructions will help you get a copy of the project up and running on your local machine for development.<br/>
Note: You may need to change CMake include directory paths.

### Prerequisites

* [Assimp](https://github.com/assimp/assimp) - Model loading library
* [Chaos](https://github.com/Zalrioth/Chaos) - GPU physics library
* [GLFW](https://github.com/glfw/glfw/) - Vulkan utility library
* [LLVM](http://releases.llvm.org/download.html) - Compiler that supports OpenMP 4
* [Lua](https://github.com/lua/lua) - Embedded scripting language
* [Vulkan](https://www.lunarg.com/vulkan-sdk/) - Vulkan SDK

### Setting Up a Project

Mana supports any langauge that can call foreign functions. Here's an example in Python:

```
from ctypes import *

mana = cdll.LoadLibrary("Mana.dll")


def game():
    print("Loading Game")

    mana.init()
    mana.new_window(1280, 720)

    while mana.should_close() is not True:
        mana.update()

    mana.close_window()
    mana.cleanup()

```
