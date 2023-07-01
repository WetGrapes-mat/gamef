#include "enemy.hxx"

enemy::enemy() {
  pos = {rand_spawn(-0.9f, 0.9f), rand_spawn(1.2f, 10.f), 1.f};
  // pos = {rand_spawn(-0.9f, 0.9f), 0.8f, 1.f};

  this->scale = 1.f;
  this->sprite = {pos[0] - 0.1f, pos[1] + 0.1f, pos[0] + 0.1f, pos[1] - 0.1f};
}

void enemy::shoot(std::vector<bullet>& shoots) {
  bullet shoot_temp = bullet(this->pos, -bullet_speed);
  shoots.push_back(shoot_temp);
  cd = 0;
}

void enemy::update(std::vector<bullet>& shoots) {
  sprite.move_matrix = {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, move_pos[1], 1.f};
  sprite.scale_matrix = {this->scale, 0.f, 0.f, 0.f, this->scale, 0.f, 0.f, 0.f, 1.f};
  sprite.result_matrix = sprite.aspect_matrix * sprite.move_matrix * sprite.scale_matrix;

  cd += cd_step;
  this->pos[1] += speed_y;
  // std::cout << speed_x << std::endl;

  this->move_pos[1] += speed_y;

  if (cd > max_cd && this->pos[1] < 1.2f)
    shoot(shoots);
}

void enemy::render(grp::iengine& engine, grp::texture& texture) {
  grp::get_transformed_triangle(this->sprite);

  // engine->render(triangle_low_transformed);
  // engine->render(triangle_high_transformed);

  engine.render(this->sprite, &texture);
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

float enemy::speed_x = 0.05;
float enemy::speed_y = 0.05;
float enemy::max_cd = 5.f;
float enemy::cd_step = 0.01;
float enemy::bullet_speed = 0.0001f;

void enemy::set_config(std::map<std::string, float>& configMap) {
  speed_x = configMap["enemys_speed_x"] / 100;
  speed_y = -configMap["enemys_speed_y"] / 100;
  max_cd = configMap["enemys_max_cd"];
  cd_step = configMap["enemys_cd_step"];
  bullet_speed = configMap["bullet_speed"] / 100;
}

void enemy::update() {};