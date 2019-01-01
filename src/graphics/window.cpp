#include "graphics/window.hpp"

Window::Window(bool vSync, int width, int height) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(
      GLFW_OPENGL_FORWARD_COMPAT,
      GL_TRUE);  // uncomment this statement to fix compilation on OS X
#endif

  GLFWwindow* window =
      glfwCreateWindow(width, height, "Hello Triangle", NULL, NULL);

  this->glWindow = window;
  this->vSync = vSync;
  this->limitFPS = false;
  this->width = width;
  this->height = height;
}

int Window::setupWindow() {
  if (!this->glWindow) return 2;

  glfwMakeContextCurrent(this->glWindow);

  // vsync 0 = false, 1 = true
  glfwSwapInterval(this->vSync);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
  const GLubyte* version = glGetString(GL_VERSION);    // version as a string
  std::cout << "Renderer: " << renderer << std::endl;
  std::cout << "OpenGL version supported " << version << std::endl;

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  // glEnable(GL_DEPTH_TEST); // enable depth-testing
  // glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as
  // "closer"

  // Set the clear color
  // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // glClearColor(0.1f, 0.1f, 0.1f, 1.f);

  // glEnable(GL_LIGHTING);
  // glEnable(GL_LIGHT0); /* Uses default lighting parameters */

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  // glEnable(GL_NORMALIZE);

  /* XXX docs say all polygons are emitted CCW, but tests show that some aren't.
   */
  // if (getenv("MODEL_IS_BROKEN"))
  // glFrontFace(GL_CW);

  // glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

  /*if (opts.showTriangles) {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }*/

  // Support for transparencies
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Antialiasing
  glfwWindowHint(GLFW_SAMPLES, 4);

  return 0;
}

Window::~Window() {
  // close GL context and any other GLFW resources
  glfwTerminate();
}