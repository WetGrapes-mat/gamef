#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include "bullets.hxx"
#include "enemy.hxx"
#include "engine.hxx"
#include "player.hxx"

#include "glm/glm.hpp"

int main(int /*argc*/, char* /*argv*/[]) {
  using namespace std;
  unique_ptr<grp::engine, void (*)(grp::engine*)> engine(grp::create_engine(), grp::destroy_engine);
  int screen_width = 640;
  int screen_height = 480;

  engine->initialize("");
  unique_ptr<grp::texture> hero_texture {engine->create_texture("./img/pixel_ship_yellow.png")};
  unique_ptr<grp::texture> bulet_texture {engine->create_texture("./img/pixel_laser_yellow.png")};
  unique_ptr<grp::texture> map_texture {engine->create_texture("./img/background-black.png")};
  unique_ptr<grp::texture> enemy_green_texture {
    engine->create_texture("./img/pixel_ship_green_big.png")};

  //++++++++++++++++++hero++++++++++++++++++++

  player hero = player();

  vector<bullets> enemy_shoots;
  vector<enemy> enemys;
  for (int i = 0; i < 5; i++) {
    enemys.push_back(enemy());
  }

  //++++++++++++++++++map triandle++++++++++++++++++++

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

  //++++++++++++++++++main loop++++++++++++++++++++
  bool continue_loop = true;
  while (continue_loop) {
    grp::event event;
    while (engine->input_event(event, grp::keyStates)) {
      // a
      if (grp::keyStates[2]) {
        hero.my_pos[0] -= hero.speed_x;
      }
      // d
      if (grp::keyStates[3]) {
        hero.my_pos[0] += hero.speed_x;
      }
      // w
      if (grp::keyStates[0]) {
        hero.my_pos[1] += hero.speed_y;
      }
      // s
      if (grp::keyStates[1]) {
        hero.my_pos[1] -= hero.speed_y;
      }
      // space
      if (grp::keyStates[4] && hero.charge) {
        hero.shoot();
      }
      // escape
      if (grp::keyStates[5]) {
        continue_loop = false;
        break;
      }
    }

    engine->render(triangle_map_low, map_texture.get());
    engine->render(triangle_map_high, map_texture.get());
    //++++++++++++++++++bulets++++++++++++++++++++

    //++++++++++++++++++hero++++++++++++++++++++
    hero.update();
    hero.render(engine, hero_texture);

    for (int i = 0; i < enemys.size(); i++) {
      enemys[i].update(enemy_shoots);
      enemys[i].render(engine, enemy_green_texture);
    }

    for (int i = 0; i < hero.shoots.size(); i++) {
      hero.shoots[i].update();
      hero.shoots[i].render(engine, bulet_texture);
    }

    for (int i = 0; i < enemy_shoots.size(); i++) {
      enemy_shoots[i].update();
      enemy_shoots[i].render(engine, bulet_texture);
    }
    engine->swap_buffers();
  }

  engine->uninitialize();

  return EXIT_SUCCESS;
}
