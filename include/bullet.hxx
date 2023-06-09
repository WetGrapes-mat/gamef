#pragma once
#include "ientity.hxx"

class bullet : public ientity {
  public:
    bullet(const glm::vec3& pos, float speed, grp::texture* textr);
    void update() override;
    void render(grp::iengine& engine) override;
    float out_of_screen();

  private:
    glm::vec3 bullet_pos;
    glm::vec3 bullet_move_pos;
    float speed;
};
