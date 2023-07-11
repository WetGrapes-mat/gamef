#include <iostream>

#include "bullet.hxx"
#include "player.hxx"

float player::bullet_speed = 0.0001f;
std::map<std::string, float> player::configMap = {};

player::player(grp::texture* textr,
               grp::texture* bullet_text,
               std::map<std::string, float>& configMap) {
  this->sprite = {-0.1f, 0.1f, 0.1f, -0.1f, textr};
  this->bullet_texture = bullet_text;
  this->speed_x = configMap["player_spead_x"] / 100.f;
  this->speed_y = configMap["player_spead_y"] / 100.f;
  this->cd_step = configMap["player_cd_step"];
  this->max_cd = configMap["player_max_cd"];
}
void player::shoot() {
  if (charge) {
    bullet shoot_temp = bullet(my_pos, bullet_speed, bullet_texture);
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
  if (cd > max_cd * configMap["player_max_cd"])
    charge = true;
  for (auto it = shoots.begin(); it != shoots.end();) {
    if (it->out_of_screen() >= 1.f) {
      it = shoots.erase(it);
    } else
      ++it;
  }

  if (bonus_stack != 0) {
    frame_count++;
    if (frame_count >= 300) {
      frame_count = 0;
      bonus_stack--;
      cd_step /= 2;
    }
  }
}

void player::render(grp::iengine& engine) {
  grp::get_transformed_triangle(this->sprite);
  engine.render(this->sprite);
}

void player::clear() {
  hp = 100.f;
  charge = true;
  my_pos = {0.f, 0.f, 1.f};
  shoots.clear();
  cd = 0;
  if (bonus_stack != 0) {
    cd_step /= 2 * bonus_stack;
    bonus_stack = 0;
    frame_count = 0;
  }
}