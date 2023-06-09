#pragma once
#include <iostream>

#include "bullets.hxx"
#include "player.hxx"
player::player() {}
void player::shoot() {
  bullets shoot_temp = bullets(my_pos, 0.0006f);
  shoots.push_back(shoot_temp);
  charge = false;
  cd = 0;
}

void player::update() {
  const glm::mediump_mat3x3 aspect_matrix {1.f, 0.f, 0.f, 0.f, 640.f / 480.f, 0.f, 0.f, 0.f, 1.f};
  const glm::mediump_mat3x3 move_matrix {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, my_pos[0], my_pos[1], 1.f};
  this->result_matrix = aspect_matrix * move_matrix;
  cd += cd_step;
  if (cd > max_cd)
    charge = true;
  for (auto it = shoots.begin(); it != shoots.end();) {
    if (it->out_of_screen() >= 1.f) {
      it = shoots.erase(it);
    } else
      ++it;
  }
}

void player::render(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine,
                    std::unique_ptr<grp::texture>& texture) {
  grp::triangle triangle_low_transformed =
    grp::get_transformed_triangle(triangle_low, result_matrix);
  grp::triangle triangle_high_transformed =
    grp::get_transformed_triangle(triangle_high, result_matrix);

  engine->render(triangle_low_transformed, texture.get());
  engine->render(triangle_high_transformed, texture.get());
}