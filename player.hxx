#pragma once
#include "bullets.hxx"
#include "engine.hxx"
#include "glm/glm.hpp"

class player {
  public:
    player();
    void shoot();
    void update();
    void render(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine,
                std::unique_ptr<grp::texture>& texture);
    std::vector<bullets> shoots;
    glm::vec3 my_pos {0.f, 0.f, 1.f};
    float speed_x {0.05f}, speed_y {0.05f};
    bool charge = true;

  private:
    float cd = 0.f;
    float max_cd = 5.f;
    float cd_step = 0.01f;
    glm::mediump_mat3 result_matrix;
    grp::triangle triangle_low {
      {-0.1f, -0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.1f, -0.1f, 0.f, 0.f,
       0.f, 0.f, 1.f, 1.f, -0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
    };

    grp::triangle triangle_high {
      {-0.1f, 0.1f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.1f, 0.1f, 0.f, 0.f,
       0.f, 0.f, 1.f, 0.f, 0.1f, -0.1f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f},
    };
};
