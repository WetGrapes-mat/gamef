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
  sprite.move_matrix = {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0, this->bullet_move_pos[1], 1.f};
  sprite.scale_matrix = {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f};
  sprite.result_matrix = sprite.aspect_matrix * sprite.move_matrix * sprite.scale_matrix;

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
