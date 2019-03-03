#include "core/mana.h"

// C naming conventions
//https://stackoverflow.com/questions/1722112/what-are-the-most-common-naming-conventions-in-c
//https://stackoverflow.com/questions/4316314/pass-struct-by-reference-in-c
//https://stackoverflow.com/questions/252780/why-should-we-typedef-a-struct-so-often-in-c

bool init()
{
    init_engine(&engine);

    return true;
}

bool new_window()
{
    create_glfw_window(&engine.window);
}

void update()
{
    update_engine(&engine.window);
}

bool should_close()
{
    return glfwWindowShouldClose(engine.window.glfwWindow);
}

void close_window()
{
    glfwTerminate();
}

void cleanup()
{
    delete_engine(&engine);
}
