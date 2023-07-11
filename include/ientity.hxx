#pragma once
#include "engine.hxx"
#include <iostream>

class ientity {
  public:
    virtual void update() = 0;
    virtual void render(grp::iengine& engine) = 0;
    virtual bool collision(grp::sprite& collision_entity) {
      return sprite.collision(collision_entity.AABB);
    };
    grp::sprite sprite;
};