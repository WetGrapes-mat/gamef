#pragma once
#include "engine.hxx"
#include "glm/glm.hpp"

class bullets {
  public:
    bullets(const glm::vec3& pos);
    void update();
    void render(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine,
                std::unique_ptr<grp::texture>& texture);
    float out_of_screen();

  private:
    grp::triangle triangle_high;
    grp::triangle triangle_low;
    glm::vec3 bullet_pos;
    float speed {0.0006f};
    glm::mediump_mat3 result_matrix;
};
