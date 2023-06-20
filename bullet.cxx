#include "bullet.hxx"
#include <iostream>

bullet::bullet(const glm::vec3& pos, float speed) {
  this->bullet_pos = pos;
  if (speed < 0.f) {
    this->bullet_pos[1] -= 0.1f;
  };
  this->sprite = {
    bullet_pos[0] - 0.01f, bullet_pos[1] + 0.05f, bullet_pos[0] + 0.01f, bullet_pos[1] + 0.1f};
  this->bullet_move_pos = {0.f, 0.f, 1.f};

  this->speed = speed;
}

void bullet::update() {
  const glm::mediump_mat3x3 aspect_matrix {1.f, 0.f, 0.f, 0.f, 640.f / 480.f, 0.f, 0.f, 0.f, 1.f};
  const glm::mediump_mat3x3 move_matrix {
    1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0, this->bullet_move_pos[1], 1.f};
  this->sprite.result_matrix = aspect_matrix * move_matrix;
  this->bullet_pos[1] += speed;
  this->bullet_move_pos[1] += speed;
}

void bullet::render(grp::iengine& engine, grp::texture& texture) {
  grp::get_transformed_triangle(this->sprite);
  engine.render(this->sprite, &texture);
}

float bullet::out_of_screen() {
  return this->bullet_pos[1];
}
