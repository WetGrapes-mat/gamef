#pragma once
#include <map>

#include "bullet.hxx"
#include "engine.hxx"
#include "ientity.hxx"

class player : public ientity {
  public:
    player() = default;

    player(grp::texture* textr,
           grp::texture* bullet_textr,
           std::map<std::string, float>& configMap);
    void shoot();
    void update() override;
    void render(grp::iengine& engine) override;
    void clear();

    std::vector<bullet> shoots;

    glm::vec3 my_pos {0.f, 0.f, 1.f};
    bool charge;
    float speed_x, speed_y;
    static float bullet_speed;
    float hp = 100.f;
    grp::texture* bullet_texture;
    float max_cd;
    static std::map<std::string, float> configMap;

    float cd_step = 0.1f;
    int bonus_stack = 0;

  private:
    float cd = 0.f;
    int frame_count = 0;
};
