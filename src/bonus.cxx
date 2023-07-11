#include "bonus.hxx"

bonus::bonus(grp::texture* textr, int typ) {
  this->type = typ;
  this->pos = {rand_spawn(-0.9f, 0.9f), rand_spawn(-0.7f, 0.7f), 1.f};
  this->sprite = {pos[0] - 0.05f, pos[1] + 0.05f, pos[0] + 0.05f, pos[1] - 0.05f, textr};
}

float bonus::rand_spawn(float low, float max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(low, max);
  return dis(gen);
}

void bonus::update() {
  sprite.result_matrix = sprite.aspect_matrix;
  life += 1;
}

void bonus::render(grp::iengine& engine) {
  grp::get_transformed_triangle(this->sprite);
  engine.render(this->sprite);
}