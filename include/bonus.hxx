#pragma once
#include "ientity.hxx"
#include <iostream>
#include <random>

class bonus : public ientity {
  public:
    bonus() = default;

    int type;
    int life = 0;

    bonus(grp::texture* textr, int typ);
    void update() override;
    void render(grp::iengine& engine) override;
    float rand_spawn(float low, float max);

  private:
    glm::vec3 pos;
};