#include <iostream>

#include "bullet.hxx"
#include "player.hxx"

player::player() {
  this->sprite = {-0.1f, 0.1f, 0.1f, -0.1f};
}
void player::shoot() {
  if (charge) {
    bullet shoot_temp = bullet(my_pos, bullet_speed);
    shoots.push_back(shoot_temp);
    charge = false;
    cd = 0;
  }
}

void player::update() {
  sprite.move_matrix = {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, my_pos[0], my_pos[1], 1.f};
  sprite.scale_matrix = {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f};
  sprite.result_matrix = sprite.aspect_matrix * sprite.move_matrix * sprite.scale_matrix;

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

void player::render(grp::iengine& engine, grp::texture& texture) {
  grp::get_transformed_triangle(this->sprite);
  engine.render(this->sprite, &texture);
}

float player::speed_x = 0.05;
float player::speed_y = 0.05;
float player::max_cd = 5.f;
float player::cd_step = 0.01;
float player::bullet_speed = 0.05f;

void player::set_config(std::map<std::string, float>& configMap) {
  speed_x = configMap["player_spead_x"] / 100;
  speed_y = configMap["player_spead_y"] / 100;
  max_cd = configMap["player_max_cd"];
  cd_step = configMap["player_cd_step"];
  bullet_speed = configMap["bullet_speed"] / 100;
}
