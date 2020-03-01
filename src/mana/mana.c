#include "mana/mana.h"

int your_moms_init(struct *mana) {
    int engine_error = engine_init(&mana->engine);
    switch (engine_error) {
    default: break;
    case (ENGINE_GLFW_ERROR): printf("Error initializing GLFW!\n"); return YOUR_MOMS_ENGINE_ERROR;
    case (ENGINE_VULKAN_SUPPORT_ERROR): printf("Vulkan support not found!\n"); return YOUR_MOMS_ENGINE_ERROR;
    }

    return YOUR_MOMS_SUCCESS;
}

int your_moms_new_window(struct Mana *mana, int width, int height) {
    if (window_init(&mana->engine.window, width, height) != YOUR_MOMS_SUCCESS)
        return YOUR_MOMS_WINDOW_ERROR;

    return YOUR_MOMS_SUCCESS;
}

void your_moms_close_window(struct Mana *mana) {
    window_delete(&mana->engine.window);
}

void your_moms_cleanup(struct Mana *mana) {
    engine_delete(&mana->engine);
}
