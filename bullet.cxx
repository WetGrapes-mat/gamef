#include "bullet.hxx"
#include <iostream>

bullet::bullet(const glm::vec3& pos, float speed) {
  this->bullet_pos = pos;
  if (speed < 0.f) {
    this->bullet_pos[1] -= 0.1f;
  };

  this->triangle_low = {
    {this->bullet_pos[0] - 0.01f, this->bullet_pos[1] + 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
     this->bullet_pos[0] + 0.01f, this->bullet_pos[1] + 0.1f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f,
     this->bullet_pos[0] - 0.01f, this->bullet_pos[1] + 0.05f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
  };
  this->triangle_high = {
    {this->bullet_pos[0] - 0.01f, this->bullet_pos[1] + 0.05f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
     this->bullet_pos[0] + 0.01f, this->bullet_pos[1] + 0.05f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f,
     this->bullet_pos[0] + 0.01f, this->bullet_pos[1] + 0.1f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f},
  };
  this->bullet_move_pos = {0.f, 0.f, 1.f};

  this->speed = speed;
}

void bullet::update() {
  const glm::mediump_mat3x3 aspect_matrix {1.f, 0.f, 0.f, 0.f, 640.f / 480.f, 0.f, 0.f, 0.f, 1.f};
  const glm::mediump_mat3x3 move_matrix {
    1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0, this->bullet_move_pos[1], 1.f};
  this->result_matrix = aspect_matrix * move_matrix;
  this->bullet_pos[1] += speed;
  this->bullet_move_pos[1] += speed;
}

void bullet::render(std::unique_ptr<grp::iengine, void (*)(grp::iengine*)>& engine,
                    std::unique_ptr<grp::texture>& texture) {
  this->triangle_low_transformed = grp::get_transformed_triangle(triangle_low, result_matrix);
  this->triangle_high_transformed = grp::get_transformed_triangle(triangle_high, result_matrix);

  engine->render(triangle_low_transformed, texture.get());
  engine->render(triangle_high_transformed, texture.get());
  AABB_data();
}

float bullet::out_of_screen() {
  return this->bullet_pos[1];
}