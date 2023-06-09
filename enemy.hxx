#pragma once
#include "bullets.hxx"
#include "engine.hxx"
#include "glm/glm.hpp"

#include <iostream>
#include <random>

class enemy {
  public:
    enemy();

    void update(std::vector<bullets>& shoots);
    void render(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine,
                std::unique_ptr<grp::texture>& texture);
    glm::vec3 pos;
    glm::vec3 move_pos {0.f, 0.f, 1.f};

    float speed_x {0.003f}, speed_y {-0.0003f};

  private:
    float cd = 0.f;
    float max_cd = 9.f;
    float cd_step = 0.01f;
    float scale;
    glm::mediump_mat3 result_matrix;
    grp::triangle triangle_low;

    grp::triangle triangle_high;
    float rand_spawn(float low, float max);
    void shoot(std::vector<bullets>& shoots);
};
