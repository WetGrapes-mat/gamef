#pragma once
#include "bullets.hxx"
#include <iostream>

bullets::bullets(const glm::vec3& pos) {
  this->triangle_high = {
    {pos[0] - 0.1f, pos[1] + 0.2f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
     pos[0] + 0.1f, pos[1] + 0.2f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f,
     pos[0] - 0.1f, pos[1] - 0.15f + 0.2f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
  };
  this->triangle_low = {
    {pos[0] - 0.1f, pos[1] - 0.15f + 0.2f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
     pos[0] + 0.1f, pos[1] - 0.15f + 0.2f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f,
     pos[0] + 0.1f, pos[1] + 0.2f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f},
  };
  this->bullet_pos = {0.f, 0.f, 1.f};
}

void bullets::update() {
  const glm::mediump_mat3x3 aspect_matrix {1.f, 0.f, 0.f, 0.f, 640.f / 480.f, 0.f, 0.f, 0.f, 1.f};
  const glm::mediump_mat3x3 move_matrix {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0, this->bullet_pos[1], 1.f};
  this->result_matrix = aspect_matrix * move_matrix;
  this->bullet_pos[1] += speed;
}

void bullets::render(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine,
                     std::unique_ptr<grp::texture>& texture) {
  grp::triangle triangle_low_transformed =
    grp::get_transformed_triangle(triangle_low, result_matrix);
  grp::triangle triangle_high_transformed =
    grp::get_transformed_triangle(triangle_high, result_matrix);

  engine->render(triangle_low_transformed, texture.get());
  engine->render(triangle_high_transformed, texture.get());
}

float bullets::out_of_screen() {
  return this->bullet_pos[1];
}