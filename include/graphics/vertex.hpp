#pragma once
#ifndef VERTEX_HPP_
#define VERTEX_HPP_

#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

#endif  // VERTEX_HPP_