#include "enemy.hxx"
#include "fwd.hpp"
#include <iostream>

float enemy::bullet_speed = 0.0001f;
std::map<std::string, float> enemy::configMap = {};

enemy::enemy(grp::texture* textr,
             grp::texture* bullet_text,
             std::map<std::string, float>& configMap) {
  // pos = {rand_spawn(-0.9f, 0.9f), rand_spawn(1.2f, 10.f), 1.f};
  pos = {rand_spawn(-0.9f, 0.9f), 0.8f, 1.f};
  this->scale = configMap["scale"];
  this->speed_y = -configMap["velocity"];
  this->hp = configMap["hp"];
  this->max_cd = configMap["CD"];
  this->sprite = {pos[0] - 0.1f * configMap["scale"],
                  pos[1] + 0.1f * configMap["scale"],
                  pos[0] + 0.1f * configMap["scale"],
                  pos[1] - 0.1f * configMap["scale"],
                  textr};
  this->bullet_texture = bullet_text;
}

void enemy::shoot(std::vector<bullet>& shoots) {
  bullet shoot_temp = bullet(pos, -bullet_speed, bullet_texture);
  shoots.push_back(shoot_temp);
  cd = 0;
}

void enemy::update(std::vector<bullet>& shoots) {
  sprite.move_matrix = {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, move_pos[1], 1.f};
  // sprite.scale_matrix = {this->scale, 0.f, 0.f, 0.f, this->scale, 0.f, 0.f, 0.f, 1.f};
  sprite.result_matrix = sprite.aspect_matrix * sprite.move_matrix;

  cd += cd_step;
  this->pos[1] += speed_y * configMap["enemys_speed_y"];

  this->move_pos[1] += speed_y * configMap["enemys_speed_y"];

  if (cd > max_cd * configMap["enemys_max_cd"] && this->pos[1] < 1.2f)
    shoot(shoots);
}

void enemy::render(grp::iengine& engine) {
  grp::get_transformed_triangle(this->sprite);

  // engine->render(triangle_low_transformed);
  // engine->render(triangle_high_transformed);

  engine.render(this->sprite);
}

float enemy::rand_spawn(float low, float max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(low, max);
  return dis(gen);
}

float enemy::out_of_screen() {
  return this->pos[1];
}

void enemy::update() {};