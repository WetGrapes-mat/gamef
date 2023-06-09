#pragma once
#include "bullets.hxx"
#include "engine.hxx"
#include "player.hxx"

#include "glm/glm.hpp"

class game {
  public:
    void update();
    void render(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine);
    int screen_width = 640;
    int screen_height = 480;
    game(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine);

    std::unique_ptr<grp::texture> hero_texture;
    std::unique_ptr<grp::texture> bulet_texture;
    std::unique_ptr<grp::texture> map_texture;
    player hero;

    // clang-format off
   grp::triangle triangle_map_low {
      {-1.f, -1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 1.f,
       1.f , -1.f, -1.f, 0.f,0.f, 0.f, 1.f, 1.f,
        -1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f},
    };

    grp::triangle triangle_map_high {
      {-1.f, 1.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
       1.f, 1.f, -1.f, 0.f,0.f, 0.f, 1.f, 0.f, 
       1.f , -1.f, -1.f, 0.f, 0.f, 0.f, 1.f, 1.f},
    };

    // clang-format on
};