#include "core/arithmatic.h"

#include <GL/glew.h> // This must appear before freeglut.h
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

int window()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void connect()
{
    printf("Connected to C extension...\n");
}

//return random value in range of 0-50
int randNum()
{
    int nRand = rand() % 50;
    return nRand;
}

//add two number and return value
int addNum(int a, int b)
{
    int nAdd = a + b;
    return nAdd;
}