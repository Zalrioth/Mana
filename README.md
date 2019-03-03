# Mana - Mana's Also Not (an) Ancronym

A simple cross-platform Vulkan graphics engine.

## Getting Started

These instructions will help you get a copy of the project up and running on your local machine for development.<br/>
Note: You may need to change CMake include directory paths.

### Prerequisites

* [GLM](https://github.com/g-truc/glm) - OpenGL Mathematics library
* [GLFW](https://github.com/glfw/glfw) - Vulkan helper library
* [Assimp](https://github.com/assimp/assimp) - Model loading library

### Setting Up a Project

Create a new class that extends application and override update/render

```
class Game : public Application {
 public:
  Game();
  ~Game();
  void update(float deltaTime);
  void render();
};
```

Create a new instance of your game

```
  Application* game = new Game();

  try {
    game->run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
```