// Launches a window with dynamic entities that can be viewed with the camera

#include <mana/core/memoryallocator.h>
//
#include <mana/core/inputmanager.h>
#include <mana/graphics/entities/sprite.h>
#include <mana/graphics/shaders/modelstaticshader.h>
#include <mana/graphics/shaders/spriteshader.h>
#include <mana/graphics/utilities/camera.h>
#include <mana/graphics/utilities/modelcache.h>
#include <mana/graphics/utilities/texturecache.h>
#include <mana/mana.h>

struct Game {
  struct Mana mana;
  struct Window window;

  struct Camera camera;
  struct TextureCache texture_cache;
  struct SpriteShader sprite_shader;
  struct ModelCache model_cache;
  struct ModelStaticShader model_static_shader;
  vec3 light_pos;

  struct ArrayList models;
  struct ArrayList sprites;
};

enum GAME_STATUS { GAME_SUCCESS = 0,
                   GAME_MANA_ERROR,
                   GAME_LAST_ERROR };

int game_init(struct Game* game);
void game_delete(struct Game* game);
void game_start(struct Game* game);
void game_update(struct Game* game, struct Mana* mana, double delta_time);
void game_update_camera(struct Game* game, struct Engine* engine);
void game_update_input(struct Game* game, struct Engine* engine);
void game_update_uniform_buffers(struct Game* game, struct Engine* engine);

int main(int argc, char* argv[]) {
  setbuf(stdout, NULL);

  struct Game game = {0};
  int game_error = game_init(&game);
  switch (game_error) {
    case (GAME_SUCCESS):
      break;
    case (GAME_MANA_ERROR):
      fprintf(stderr, "Failed to setup game!\n");
      return 1;
    default:
      fprintf(stderr, "Unknown game error! Error code: %d\n", game_error);
      return 2;
  }
  game_start(&game);
  game_delete(&game);

  return 0;
}

int game_init(struct Game* game) {
  int mana_init_error = mana_init(&game->mana, (struct EngineSettings){GLFW_LIBRARY, VULKAN_API});
  switch (mana_init_error) {
    case (MANA_SUCCESS):
      break;
    case (MANA_ENGINE_ERROR):
      fprintf(stderr, "Failed to setup glfw for engine!\n");
      return GAME_MANA_ERROR;
    default:
      fprintf(stderr, "Unknown mana error! Error code: %d\n", mana_init_error);
      return GAME_MANA_ERROR;
  }

  window_init(&game->window, &game->mana.engine, 1280, 720);

  struct GPUAPI* gpu_api = &game->mana.engine.gpu_api;

  camera_init(&game->camera);
  vec3 added_pos = vec3_scale(game->camera.front, 10);
  game->camera.position = vec3_add(game->camera.position, added_pos);
  game->camera.yaw += 180;

  struct TextureSettings texture1 = {"./assets/textures/alpha.png", FILTER_LINEAR};
  struct TextureSettings texture2 = {"./assets/models/cube/colorgrid.png", FILTER_NEAREST};
  texture_cache_init(&game->texture_cache);
  texture_cache_add(&game->texture_cache, gpu_api, 2, texture1, texture2);

  sprite_shader_init(&game->sprite_shader, gpu_api);
  array_list_init(&game->sprites);
  for (int loop_num = 0; loop_num < 10; loop_num++) {
    struct Sprite* sprite = malloc(sizeof(struct Sprite));
    sprite_init(sprite, gpu_api, &game->sprite_shader.shader, texture_cache_get(&game->texture_cache, "./assets/textures/alpha.png"));
    sprite->position = (vec3){.x = loop_num, .y = loop_num, .z = loop_num};
    sprite->rotation = (quat){.data[0] = loop_num / 3.0f, .data[1] = loop_num / 3.0f, .data[2] = loop_num / 3.0f, .data[3] = 1.0f};
    array_list_add(&game->sprites, sprite);
  }

  model_static_shader_init(&game->model_static_shader, gpu_api);

  struct ModelSettings model1 = {"./assets/models/cube/cube.dae", 3, &game->model_static_shader.shader, texture_cache_get(&game->texture_cache, "./assets/models/cube/colorgrid.png")};
  model_cache_init(&game->model_cache);
  model_cache_add(&game->model_cache, gpu_api, 1, model1);

  array_list_init(&game->models);
  array_list_add(&game->models, model_cache_get(&game->model_cache, gpu_api, "./assets/models/cube/cube.dae"));

  return GAME_SUCCESS;
}

void game_delete(struct Game* game) {
  struct GPUAPI* gpu_api = &game->mana.engine.gpu_api;
  // Wait for command buffers to finish before deleting desciptor sets
  vkWaitForFences(gpu_api->vulkan_state->device, 2, gpu_api->vulkan_state->swap_chain->in_flight_fences, VK_TRUE, UINT64_MAX);

  for (int model_num = 0; model_num < array_list_size(&game->models); model_num++) {
    struct Model* model = array_list_get(&game->models, model_num);
    model_clone_delete(model, gpu_api);
    free(model);
  }
  array_list_delete(&game->models);

  model_static_shader_delete(&game->model_static_shader, gpu_api);

  model_cache_delete(&game->model_cache, gpu_api);
  texture_cache_delete(&game->texture_cache, gpu_api);

  for (int sprite_num = 0; sprite_num < array_list_size(&game->sprites); sprite_num++) {
    struct Sprite* sprite = array_list_get(&game->sprites, sprite_num);
    sprite_delete(sprite, gpu_api);
    free(sprite);
  }

  array_list_delete(&game->sprites);
  sprite_shader_delete(&game->sprite_shader, gpu_api);

  window_delete(&game->window);
  mana_cleanup(&game->mana);
}

void game_start(struct Game* game) {
  struct Engine* engine = &game->mana.engine;

  while (window_should_close(&game->window) == false) {
    engine->fps_counter.now_time = engine_get_time();
    engine->fps_counter.delta_time = engine->fps_counter.now_time - engine->fps_counter.last_time;
    engine->fps_counter.last_time = engine->fps_counter.now_time;

    window_prepare_frame(&game->window);
    game_update(game, &game->mana, engine->fps_counter.delta_time);
    window_end_frame(&game->window);

    engine->fps_counter.frames++;

    if (engine_get_time() - engine->fps_counter.timer > 1.0) {
      engine->fps_counter.timer++;

      engine->fps_counter.second_target_fps = engine->fps_counter.target_fps;
      engine->fps_counter.second_average_fps = engine->fps_counter.average_fps;
      engine->fps_counter.second_frames = engine->fps_counter.frames;

      char title_buffer[1024] = {0};
      sprintf(title_buffer, "Game %d", (int)engine->fps_counter.second_frames);
      window_set_title(&game->window, title_buffer);

      engine->fps_counter.fps = engine->fps_counter.frames;

      float average_calc = 0;
      for (int loop_num = FPS_COUNT - 1; loop_num >= 0; loop_num--) {
        if (loop_num != 0)
          engine->fps_counter.fps_past[loop_num] = engine->fps_counter.fps_past[loop_num - 1];

        average_calc += engine->fps_counter.fps_past[loop_num];
      }
      engine->fps_counter.fps_past[0] = engine->fps_counter.fps;
      engine->fps_counter.average_fps = average_calc / FPS_COUNT;
      engine->fps_counter.frames = 0;
    }
  }
}

void game_update(struct Game* game, struct Mana* mana, double delta_time) {
  struct GPUAPI* gpu_api = &mana->engine.gpu_api;
  // When the window is rezied everything must be recreated in vulkan
  if (mana->engine.gpu_api.vulkan_state->reset_shaders) {
    mana->engine.gpu_api.vulkan_state->reset_shaders = false;
    vkDeviceWaitIdle(mana->engine.gpu_api.vulkan_state->device);

    model_static_shader_delete(&game->model_static_shader, gpu_api);
    model_static_shader_init(&game->model_static_shader, gpu_api);

    sprite_shader_delete(&game->sprite_shader, gpu_api);
    sprite_shader_init(&game->sprite_shader, gpu_api);

    for (int model_num = 0; model_num < array_list_size(&game->models); model_num++) {
      struct Model* model = array_list_get(&game->models, model_num);
      model_recreate(model, gpu_api);
    }
    for (int sprite_num = 0; sprite_num < array_list_size(&game->sprites); sprite_num++) {
      struct Sprite* sprite = array_list_get(&game->sprites, sprite_num);
      sprite_recreate(sprite, gpu_api);
    }
  }

  game->light_pos = game->camera.position;

  game_update_input(game, &mana->engine);
  game_update_camera(game, &mana->engine);
  camera_update_vectors(&game->camera);
  gpu_api->vulkan_state->gbuffer->projection_matrix = camera_get_projection_matrix(&game->camera, &game->window);
  gpu_api->vulkan_state->gbuffer->view_matrix = camera_get_view_matrix(&game->camera);
  game_update_uniform_buffers(game, &mana->engine);

  gbuffer_start(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);

  for (int model_num = 0; model_num < array_list_size(&game->models); model_num++) {
    struct Model* model = array_list_get(&game->models, model_num);
    float rot_val = (model_num + 1) * delta_time;
    model->rotation = quaternion_mul(model->rotation, (quat){.data[0] = rot_val / 3.0f, .data[1] = rot_val / 3.0f, .data[2] = rot_val / 3.0f, .data[3] = 1.0f});
    model_render(model, gpu_api, delta_time);
  }
  for (int sprite_num = array_list_size(&game->sprites) - 1; sprite_num >= 0; sprite_num--) {
    struct Sprite* sprite = array_list_get(&game->sprites, sprite_num);
    float rot_val = (sprite_num + 1) * delta_time;
    sprite->rotation = quaternion_mul(sprite->rotation, (quat){.data[0] = rot_val / 3.0f, .data[1] = rot_val / 3.0f, .data[2] = rot_val / 3.0f, .data[3] = 1.0f});
    sprite_render(sprite, gpu_api, delta_time);
  }

  gbuffer_stop(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);
  blit_post_process_render(gpu_api->vulkan_state->post_process->blit_post_process, gpu_api);
  blit_swap_chain_render(gpu_api->vulkan_state->swap_chain->blit_swap_chain, gpu_api);
}

void game_update_camera(struct Game* game, struct Engine* engine) {
  bool in_focus = glfwGetWindowAttrib(engine->graphics_library.glfw_library.glfw_window, GLFW_FOCUSED);
  if (in_focus == false)
    return;

  struct InputManager* input_manager = game->window.input_manager;

  bool mouse_locked = (glfwGetMouseButton(engine->graphics_library.glfw_library.glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) ? true : false;
  mouse_locked ? glfwSetInputMode(engine->graphics_library.glfw_library.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED) : glfwSetInputMode(engine->graphics_library.glfw_library.glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  GLdouble x_pos, y_pos;
  glfwGetCursorPos(engine->graphics_library.glfw_library.glfw_window, &x_pos, &y_pos);

  double x_diff = input_manager->last_mouse_x_pos - x_pos;
  double y_diff = input_manager->last_mouse_y_pos - y_pos;

  input_manager->last_mouse_x_pos = x_pos;
  input_manager->last_mouse_y_pos = y_pos;

  if ((mouse_locked == false && game->camera.mouse_locked == true) || (mouse_locked == false && (x_pos <= 0 || y_pos <= 0 || x_pos >= game->window.width || y_pos >= game->window.height)) || (fabs(x_diff) > (game->window.width / 2.0f) || fabs(y_diff) > (game->window.height / 2.0f)))
    x_diff = y_diff = 0;

  x_diff *= game->camera.sensitivity;
  y_diff *= game->camera.sensitivity;

  game->camera.yaw -= x_diff;
  game->camera.pitch += y_diff;

  if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
    GLFWgamepadstate state = {0};
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
      float right_x_axis = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * game->camera.sensitivity * 15;
      float right_y_axis = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * game->camera.sensitivity * 15;

      if (fabs(right_x_axis) < 0.2f) right_x_axis = 0.0f;

      if (fabs(right_y_axis) < 0.2f) right_y_axis = 0.0f;

      input_manager->last_mouse_x_pos += right_x_axis;
      input_manager->last_mouse_y_pos -= right_y_axis;

      game->camera.yaw += right_x_axis;
      game->camera.pitch -= right_y_axis;
    }
  }

  if (game->camera.pitch > 89.0f) game->camera.pitch = 89.0f;
  if (game->camera.pitch < -89.0f) game->camera.pitch = -89.0f;

  game->camera.mouse_locked = mouse_locked;
}

void game_update_input(struct Game* game, struct Engine* engine) {
  bool in_focus = glfwGetWindowAttrib(engine->graphics_library.glfw_library.glfw_window, GLFW_FOCUSED);
  if (in_focus == false)
    return;

  struct InputManager* input_manager = game->window.input_manager;

  if (input_manager->keys[GLFW_KEY_ESCAPE].state == PRESSED)
    glfwSetWindowShouldClose(engine->graphics_library.glfw_library.glfw_window, true);

  float delta_time = 0.1f;
  float movement_speed = 1.0f;

  if (input_manager->keys[GLFW_KEY_LEFT_SHIFT].state == PRESSED)
    movement_speed *= 5;

  float velocity = movement_speed * delta_time;
  if (input_manager->keys[GLFW_KEY_W].state == PRESSED) {
    vec3 added_pos = vec3_scale(game->camera.front, velocity);
    game->camera.position = vec3_add(game->camera.position, added_pos);
  }
  if (input_manager->keys[GLFW_KEY_S].state == PRESSED) {
    vec3 added_pos = vec3_scale(game->camera.front, velocity);
    game->camera.position = vec3_sub(game->camera.position, added_pos);
  }
  if (input_manager->keys[GLFW_KEY_A].state == PRESSED) {
    vec3 added_pos = vec3_scale(game->camera.right, velocity);
    game->camera.position = vec3_sub(game->camera.position, added_pos);
  }
  if (input_manager->keys[GLFW_KEY_D].state == PRESSED) {
    vec3 added_pos = vec3_scale(game->camera.right, velocity);
    game->camera.position = vec3_add(game->camera.position, added_pos);
  }
  if (input_manager->keys[GLFW_KEY_E].state == PRESSED) {
    vec3 added_pos = vec3_scale(game->camera.up, velocity);
    game->camera.position = vec3_add(game->camera.position, added_pos);
  }
  if (input_manager->keys[GLFW_KEY_Q].state == PRESSED) {
    vec3 added_pos = vec3_scale(game->camera.up, velocity);
    game->camera.position = vec3_sub(game->camera.position, added_pos);
  }
  if (input_manager->keys[GLFW_KEY_Z].state == PRESSED)
    game->camera.roll -= 1.0f;

  if (input_manager->keys[GLFW_KEY_X].state == PRESSED)
    game->camera.roll += 1.0f;

  if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
    GLFWgamepadstate state = {0};
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
      float left_x_axis = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
      float left_y_axis = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

      if (fabs(left_x_axis) < 0.2f) left_x_axis = 0.0f;

      if (fabs(left_y_axis) < 0.2f) left_y_axis = 0.0f;

      float right_trigger = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1.0f) / 2.0f;

      velocity *= (right_trigger * 10.0f) + 1.0f;

      vec3 added_pos_y = vec3_scale(game->camera.front, velocity * left_y_axis);
      game->camera.position = vec3_sub(game->camera.position, added_pos_y);
      vec3 added_pos_x = vec3_scale(game->camera.right, velocity * left_x_axis);
      game->camera.position = vec3_add(game->camera.position, added_pos_x);
    }
  }
}

void game_update_uniform_buffers(struct Game* game, struct Engine* engine) {
  for (int entity_num = 0; entity_num < array_list_size(&game->sprites); entity_num++) {
    struct Sprite* sprite = (struct Sprite*)array_list_get(&game->sprites, entity_num);
    sprite_update_uniforms(sprite, &engine->gpu_api);
  }

  for (int model_num = 0; model_num < array_list_size(&game->models); model_num++) {
    struct Model* model = array_list_get(&game->models, model_num);
    model_update_uniforms(model, &engine->gpu_api, game->camera.position, game->light_pos);
  }
}
