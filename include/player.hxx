#pragma once
#include <map>

#include "bullet.hxx"
#include "engine.hxx"
#include "ientity.hxx"

class player : public ientity {
  public:
    player();
    void shoot();
    void update() override;
    void render(grp::iengine& engine, grp::texture& texture) override;
    static void set_config(std::map<std::string, float>& configMap);

    std::vector<bullet> shoots;
    glm::vec3 my_pos {0.f, 0.f, 1.f};
    bool charge;
    static float speed_x, speed_y;
    static float bullet_speed;

  private:
    float cd = 0.f;
    static float max_cd;
    static float cd_step;
};
