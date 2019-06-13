#include "core/engine.hpp"

//http://web.eecs.umich.edu/~sugih/courses/eecs487/glfw-howto/
//https://stackoverflow.com/questions/22231740/opengl32-lib-not-linking-properly
//http://antongerdelan.net/opengl/hellotriangle.html
//https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11302
//https://learnopengl.com/Model-Loading/Model
//https://www.glfw.org/docs/latest/vulkan_guide.html

// camera
double xPosition = 0;
double yPosition = 0;
double yScroll = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Engine::Engine(std::string title)
{
    this->engineSettings = new EngineSettings(title);
    this->engineSettings->window = new Window(true, 1280, 720);
    this->engineSettings->gGBuffer = new GBuffer(this->engineSettings->window->width, this->engineSettings->window->height);
    this->engineSettings->gPostProcess = new PostProcess(this->engineSettings->window->width, this->engineSettings->window->height);

    glfwMakeContextCurrent(this->engineSettings->window->glWindow);
    glfwSetFramebufferSizeCallback(this->engineSettings->window->glWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(this->engineSettings->window->glWindow, mouse_callback);
    glfwSetScrollCallback(this->engineSettings->window->glWindow, scroll_callback);
    glfwSetInputMode(this->engineSettings->window->glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // NOTE: Vulkan will hijack window unless properly setup for compute only
    //if (glfwVulkanSupported()) {
    //    // Vulkan is available, at least for compute
    //    std::cout << "Vulkan support found" << std::endl;
    //}

    srand(time(NULL));
}

Engine::~Engine()
{
    delete this->engineSettings;
}

void Engine::setScene(Scene* scene)
{
    this->scene = scene;
}

EngineSettings* Engine::getEngineSettings()
{
    return this->engineSettings;
}

void Engine::run()
{
    double limitUpdateFPS = 1.0 / 60.0;
    // maybe cap fps at 144 for limit? 30, 60, 120, 144
    const double targetFps = 144.0;
    const double limitDrawFPS = 1.0 / targetFps;
    int input[128];

    double lastTime = getTime(), timer = lastTime;
    double deltaTime = 0, nowTime = 0;
    int frames = 0, updates = 0;
    double lastRenderTime = 0;

    int fps = (int)targetFps;

    for (int loopNum = 0; loopNum < 10; loopNum++)
        this->engineSettings->fpsPast[loopNum] = fps;

    while (this->engineSettings->running && !glfwWindowShouldClose(this->engineSettings->window->glWindow)) {
        nowTime = getTime();
        deltaTime += (nowTime - lastTime) / limitUpdateFPS;
        lastTime = nowTime;

        while (deltaTime >= 1.0) {
            this->scene->update(this->engineSettings, deltaTime / 20);
            updates++;
            deltaTime--;
        }

        this->engineSettings->projectionMatrix = glm::perspective(glm::radians(this->engineSettings->camera->Zoom),
            (float)this->engineSettings->window->width / (float)this->engineSettings->window->height, this->engineSettings->camera->ZNear, this->engineSettings->camera->ZFar);
        this->engineSettings->viewMatrix = this->engineSettings->camera->GetViewMatrix();
        this->scene->render(this->engineSettings);

        glfwSwapBuffers(this->engineSettings->window->glWindow);
        glfwPollEvents();

        this->engineSettings->inputManager->processInput(this->engineSettings->window);

        engineSettings->xPosition = xPosition;
        engineSettings->yPosition = yPosition;
        engineSettings->yScroll = yScroll;
        yScroll = 0;

        frames++;

        if (getTime() - timer > 1.0) {
            timer++;
            //std::cout << "Draw FPS: " << frames << std::endl;
            //std::cout << "Target FPS: " << targetFps << std::endl;
            //std::cout << "Average FPS: " << this->engineSettings->averageFps << std::endl;
            //std::cout << "Update FPS: " << updates << std::endl;

            fps = frames;

            std::string newTitle = "Grindstone FPS: " + std::to_string(this->engineSettings->averageFps);

            glfwSetWindowTitle(this->engineSettings->window->glWindow, newTitle.c_str());

            float averageCalc = 0;
            for (int loopNum = 10 - 1; loopNum >= 0; loopNum--) {
                if (loopNum != 0)
                    this->engineSettings->fpsPast[loopNum] = this->engineSettings->fpsPast[loopNum - 1];

                averageCalc += this->engineSettings->fpsPast[loopNum];
            }
            this->engineSettings->fpsPast[0] = fps;
            this->engineSettings->averageFps = averageCalc / 10;

            updates = 0, frames = 0;
        }

        if (this->engineSettings->window->vSync == false && this->engineSettings->window->limitFPS == true) {
            double endTime = lastTime + limitDrawFPS;
            sleep(limitDrawFPS * 1000);
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    xPosition = xpos;
    yPosition = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    yScroll = yoffset;
}