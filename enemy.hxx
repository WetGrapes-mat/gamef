#pragma once
#include "bullet.hxx"
#include "engine.hxx"
#include "ientity.hxx"

#include <iostream>
#include <random>

class enemy : public ientity {
  public:
    enemy();
    void update(std::vector<bullet>& shoots);
    void update() override;
    void render(std::unique_ptr<grp::iengine, void (*)(grp::iengine*)>& engine,
                std::unique_ptr<grp::texture>& texture) override;

    float out_of_screen();

    glm::vec3 pos;
    glm::vec3 move_pos {0.f, 0.f, 1.f};

    float speed_x {0.003f}, speed_y {-0.0002f};

  private:
    float cd = 0.f;
    float max_cd = 40.f;
    float cd_step = 0.01f;
    float scale;
    float rand_spawn(float low, float max);
    void shoot(std::vector<bullet>& shoots);
};
