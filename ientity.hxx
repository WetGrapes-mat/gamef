#pragma once
#include "engine.hxx"
#include <iostream>

class ientity {
  public:
    virtual void update() = 0;
    virtual void render(grp::iengine& engine, grp::texture& texture) = 0;
    virtual bool collision(std::array<float, 4> collision_entity) {
      if (this->pos_AABB[2] < collision_entity[0] || this->pos_AABB[0] > collision_entity[2]) {
        return false;
      }
      if (this->pos_AABB[1] < collision_entity[3] || this->pos_AABB[3] > collision_entity[1]) {
        return false;
      }
      // std::cout << "pos_AABB "
      //           << "x: " << pos_AABB[0] << ";y: " << pos_AABB[1] << ";x: " << pos_AABB[2]
      //           << ";y: " << pos_AABB[3] << std::endl;
      // std::cout << "collision_entity "
      //           << "x: " << collision_entity[0] << ";y: " << collision_entity[1]
      //           << ";x: " << collision_entity[2] << ";y: " << collision_entity[3] << std::endl;

      return true;
    };
    virtual void AABB_data() {
      this->pos_AABB[0] = this->triangle_low_transformed.vertices[2].x;
      this->pos_AABB[1] = this->triangle_low_transformed.vertices[2].y;
      this->pos_AABB[2] = this->triangle_high_transformed.vertices[2].x;
      this->pos_AABB[3] = this->triangle_high_transformed.vertices[2].y;
    };
    std::array<float, 4> pos_AABB;

  protected:
    glm::mediump_mat3 result_matrix;
    grp::triangle triangle_low;
    grp::triangle triangle_high;
    grp::triangle triangle_low_transformed;
    grp::triangle triangle_high_transformed;
};