#pragma once
#include "bullet.hxx"
#include "ientity.hxx"

class player : public ientity {
  public:
    player();
    void shoot();
    void update() override;
    void render(std::unique_ptr<grp::iengine, void (*)(grp::iengine*)>& engine,
                std::unique_ptr<grp::texture>& texture) override;

    std::vector<bullet> shoots;
    glm::vec3 my_pos {0.f, 0.f, 1.f};
    float speed_x {0.05f}, speed_y {0.05f};
    bool charge = true;

  private:
    float cd = 0.f;
    float max_cd = 5.f;
    float cd_step = 0.01f;
    glm::mediump_mat3 result_matrix;
};
