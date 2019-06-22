# Mana - Mana's Also Not (an) Ancronym

## Getting Started

Build then use with your favorite language.<br/>

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