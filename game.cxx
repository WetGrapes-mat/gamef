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

#include "bullet.hxx"
#include "enemy.hxx"
#include "engine.hxx"
#include "ientity.hxx"
#include "imgui-src/imgui_impl_sdl3.h"
#include "player.hxx"

#include "nlohmann/json.hpp"

int main(int /*argc*/, char* /*argv*/[]) {
  using namespace std;
  unique_ptr<grp::iengine, void (*)(grp::iengine*)> engine(grp::create_engine(),
                                                           grp::destroy_engine);
  int screen_width = 640;
  int screen_height = 480;

  engine->initialize("");
  unique_ptr<grp::texture> hero_texture {
    engine->create_texture("./assets/images/pixel_ship_yellow.png")};
  unique_ptr<grp::texture> bulet_texture {
    engine->create_texture("./assets/images/pixel_laser_yellow.png")};
  unique_ptr<grp::texture> map_texture {
    engine->create_texture("./assets/images/background-black.png")};
  unique_ptr<grp::texture> enemy_green_texture {
    engine->create_texture("./assets/images/pixel_ship_green_big.png")};

  //++++++++++++++++++init++++++++++++++++++++
  player hero = player();
  vector<bullet> enemy_shoots;
  vector<enemy> enemys;
  map<std::string, map<std::string, int>> enemysMap;
  int wave = 0;

  using json = nlohmann::json;
  std::ifstream file("wave_list.json");
  json jsonData;
  file >> jsonData;
  file.close();

  for (const auto& [key, value] : jsonData.items()) {
    for (const auto& [type, amount] : value.items()) {
      enemysMap[key][type] = amount;
    }
  }

  grp::isound* music = engine->create_sound("./assets/sounds/main.wav");
  grp::isound* kill_sound = engine->create_sound("./assets/sounds/E1.wav");
  grp::isound* shoot_sound = engine->create_sound("./assets/sounds/laser.wav");
  // engine->set_cursor_visible(true);

  assert(music != nullptr);
  assert(kill_sound != nullptr);
  assert(shoot_sound != nullptr);

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
  // music->play(grp::isound::properties::looped);

  bool continue_loop = true;
  while (continue_loop) {
    grp::event event;
    while (engine->input_event(event, grp::keyStates)) {
      // a
      // std::cout << hero.my_pos[0] << " " << hero.my_pos[1] << std::endl;

      if (grp::keyStates[2] && -0.9f < hero.my_pos[0]) {
        hero.my_pos[0] -= hero.speed_x;
      }
      // d
      if (grp::keyStates[3] && hero.my_pos[0] < 0.9f) {
        hero.my_pos[0] += hero.speed_x;
      }
      // w
      if (grp::keyStates[0] && hero.my_pos[1] < 0.65f) {
        hero.my_pos[1] += hero.speed_y;
      }
      // s
      if (grp::keyStates[1] && -0.65f < hero.my_pos[1]) {
        hero.my_pos[1] -= hero.speed_y;
      }
      // space
      if (grp::keyStates[4] && hero.charge) {
        hero.shoot();
        // shoot_sound->play(grp::isound::properties::once);
      }
      // escape
      if (grp::keyStates[5]) {
        continue_loop = false;
        break;
      }
    }

    engine->render(triangle_map_low, map_texture.get());
    engine->render(triangle_map_high, map_texture.get());

    //++++++++++++++++++hero++++++++++++++++++++
    hero.update();
    hero.render(engine, hero_texture);

    //++++++++++++++++++enemys++++++++++++++++++++

    if (enemys.empty()) {
      wave += 1;
      std::cout << "wave: " << wave << std::endl;
      for (const auto& pair : enemysMap) {
        if (wave == stoi(pair.first)) {
          for (const auto& pair : pair.second) {
            for (int i = 0; i < pair.second; i++) {
              enemys.push_back(enemy());
            }
          }
        }
      }
    }
    //++++++++++++++++++bulets++++++++++++++++++++
    for (auto it_b = hero.shoots.begin(); it_b != hero.shoots.end();) {
      bool flag_collision = false;
      it_b->update();
      it_b->render(engine, bulet_texture);
      for (auto it_e = enemys.begin(); it_e != enemys.end();) {
        if (it_b->collision(it_e->pos_AABB)) {
          it_e = enemys.erase(it_e);
          flag_collision = true;
          // kill_sound->play(grp::isound::properties::once);
          break;
        } else
          ++it_e;
      }
      if (flag_collision)
        it_b = hero.shoots.erase(it_b);
      else
        ++it_b;
    }

    for (auto it = enemy_shoots.begin(); it != enemy_shoots.end();) {
      it->update();
      it->render(engine, bulet_texture);
      if (it->collision(hero.pos_AABB)) {
        it = enemy_shoots.erase(it);
        std::cout << " - 1 hp" << std::endl;
      } else if (it->out_of_screen() <= -1.f)
        it = enemy_shoots.erase(it);
      else
        ++it;
    }

    for (auto it_e = enemys.begin(); it_e != enemys.end();) {
      it_e->update(enemy_shoots);
      it_e->render(engine, enemy_green_texture);
      if (hero.collision(it_e->pos_AABB))
        it_e = enemys.erase(it_e);
      else if (it_e->out_of_screen() <= -1)
        it_e = enemys.erase(it_e);
      else
        ++it_e;
    }

    engine->draw_imgui();

    engine->swap_buffers();
  }

  engine->uninitialize();

  return EXIT_SUCCESS;
}
