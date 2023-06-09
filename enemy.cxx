#include "enemy.hxx"

enemy::enemy() {
  pos = {rand_spawn(-0.9f, 0.9f), rand_spawn(1.2f, 10.f), 1.f};
  std::cout << pos[0] << std::endl;
  this->scale = 1.f;
  // clang-format off
  this->triangle_low = {
    {pos[0]-0.1f, pos[1]-0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
     pos[0]+0.1f, pos[1]-0.1f, 0.f, 0.f,0.f, 0.f, 1.f, 1.f,
      pos[0]-0.1f, pos[1]+0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
  };

  this-> triangle_high =  {
    {pos[0]-0.1f, pos[1]+0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
     pos[0]+0.1f, pos[1]+0.1f, 0.f, 0.f,0.f, 0.f, 1.f, 0.f,
      pos[0]+0.1f, pos[1]-0.1f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f},
  };
  // clang-format on
}

void enemy::shoot(std::vector<bullets>& shoots) {
  bullets shoot_temp = bullets(this->pos, -0.0006f);
  shoots.push_back(shoot_temp);
  cd = 0;
}

void enemy::update(std::vector<bullets>& shoots) {
  const glm::mediump_mat3x3 aspect_matrix {1.f, 0.f, 0.f, 0.f, 640.f / 480.f, 0.f, 0.f, 0.f, 1.f};
  const glm::mediump_mat3x3 move_matrix {1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, move_pos[1], 1.f};
  const glm::mediump_mat3x3 scale_matrix {
    this->scale, 0.f, 0.f, 0.f, this->scale, 0.f, 0.f, 0.f, 1.f};
  this->result_matrix = aspect_matrix * move_matrix * scale_matrix;
  cd += cd_step;
  this->pos[1] += speed_y;
  this->move_pos[1] += speed_y;

  if (cd > max_cd && this->pos[1] < 1.2f)
    shoot(shoots);
  for (auto it = shoots.begin(); it != shoots.end();) {
    if (it->out_of_screen() <= -1.f) {
      it = shoots.erase(it);
    } else
      ++it;
  }
}

void enemy::render(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine,
                   std::unique_ptr<grp::texture>& texture) {
  grp::triangle triangle_low_transformed =
    grp::get_transformed_triangle(triangle_low, result_matrix);
  grp::triangle triangle_high_transformed =
    grp::get_transformed_triangle(triangle_high, result_matrix);

  engine->render(triangle_low_transformed, texture.get());
  engine->render(triangle_high_transformed, texture.get());
}

float enemy::rand_spawn(float low, float max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(low, max);
  return dis(gen);
}