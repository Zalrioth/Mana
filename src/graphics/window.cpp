#include "graphics/window.hpp"

Window::Window(bool vSync, int width, int height)
{
    if (!glfwInit())
        throw std::runtime_error("Unable to start GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow* window = glfwCreateWindow(width, height, "Hello Triangle", NULL, NULL);

    if (!window)
        throw std::runtime_error("Unable to create OpenGL window");

    glfwMakeContextCurrent(window);
    glfwSwapInterval(vSync); // vsync 0 = false, 1 = true
    glewExperimental = GL_TRUE; // start GLEW extension handler
    glewInit();

    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported " << version << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND); // Support for transparencies
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glfwWindowHint(GLFW_SAMPLES, 4); // Antialiasing

    this->glWindow = window;
    this->vSync = vSync;
    this->limitFPS = false;
    this->width = width;
    this->height = height;
}

Window::~Window()
{
    glfwTerminate(); // close GL context and any other GLFW resources
}