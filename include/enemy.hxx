#pragma once
#include "bullet.hxx"
#include "engine.hxx"
#include "ientity.hxx"

#include <iostream>
#include <map>
#include <random>

class enemy : public ientity {
  public:
    enemy() = default;
    enemy(grp::texture* textr, grp::texture* bullet_text, std::map<std::string, float>& configMap);
    void update(std::vector<bullet>& shoots);
    void update() override;
    void render(grp::iengine& engine) override;
    float out_of_screen();

    glm::vec3 pos;
    glm::vec3 move_pos {0.f, 0.f, 1.f};

    float speed_y;
    static float bullet_speed;
    grp::texture* bullet_texture;
    float hp;
    float max_cd;
    static std::map<std::string, float> configMap;

  private:
    float cd = 0.f;
    float cd_step = 0.1f;
    float scale;
    float rand_spawn(float low, float max);
    void shoot(std::vector<bullet>& shoots);
};
