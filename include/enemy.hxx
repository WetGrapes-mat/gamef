#pragma once
#include "bullet.hxx"
#include "engine.hxx"
#include "ientity.hxx"

#include <iostream>
#include <map>
#include <random>

class enemy : public ientity {
  public:
    enemy();
    void update(std::vector<bullet>& shoots);
    void update() override;
    void render(grp::iengine& engine, grp::texture& texture) override;
    static void set_config(std::map<std::string, float>& configMap);

    float out_of_screen();

    glm::vec3 pos;
    glm::vec3 move_pos {0.f, 0.f, 1.f};

    static float speed_x, speed_y;
    static float bullet_speed;

  private:
    float cd = 0.f;
    static float max_cd;
    static float cd_step;
    float scale;
    float rand_spawn(float low, float max);
    void shoot(std::vector<bullet>& shoots);
};
