#include "game.hxx"
#include "nlohmann/json.hpp"
#include "player.hxx"
#include <fstream>

// #define MUSIC

namespace grp {

game::game(iengine& e) : engine(e) {
  hero = player();
  hero_texture = e.create_texture("./assets/images/pixel_ship_yellow.png");
  bulet_texture = e.create_texture("./assets/images/pixel_laser_yellow.png");
  map_texture = e.create_texture("./assets/images/background-black.png");
  enemy_green_texture = e.create_texture("./assets/images/pixel_ship_green_big.png");
#ifdef MUSIC
  music = engine.create_sound("./assets/sounds/main.wav");
  kill_sound = engine.create_sound("./assets/sounds/E1.wav");
  shoot_sound = engine.create_sound("./assets/sounds/laser.wav");
  assert(music != nullptr);
  assert(kill_sound != nullptr);
  assert(shoot_sound != nullptr);
  music->play(grp::isound::properties::looped);
#endif

  read_wave();
}
game::~game() {}

igame::~igame() = default;
void game::render() {
  engine.render(triangle_map_low, map_texture);
  engine.render(triangle_map_high, map_texture);
  hero.render(engine, *hero_texture);
  render_hero_bullets();

  render_enemy_bullets();
  render_enemys();
}

void game::update() {
  update_wave();
  hero.update();
  update_hero_bullets();

  update_enemy_bullets();
  update_enemys();
}

//++++++++++++++++++hero++++++++++++++++++++

void game::update_hero_bullets() {
  for (auto it_b = hero.shoots.begin(); it_b != hero.shoots.end();) {
    bool flag_collision = false;
    it_b->update();

    for (auto it_e = enemys.begin(); it_e != enemys.end();) {
      if (it_b->collision(it_e->pos_AABB)) {
        it_e = enemys.erase(it_e);
        flag_collision = true;
#ifdef MUSIC
        kill_sound->play(grp::isound::properties::once);
#endif
        engine.myVariable += 1;
        break;
      } else
        ++it_e;
    }
    if (flag_collision)
      it_b = hero.shoots.erase(it_b);
    else
      ++it_b;
  }
}

void game::render_hero_bullets() {
  for (auto it_b = hero.shoots.begin(); it_b != hero.shoots.end(); ++it_b) {
    it_b->render(engine, *bulet_texture);
  }
}

void game::move_player(bool& flag) {
  grp::event event;
  while (engine.input_event(event, grp::keyStates)) {
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
#ifdef MUSIC
      shoot_sound->play(grp::isound::properties::once);
#endif
    }
    // escape
    if (grp::keyStates[5]) {
      flag = false;
      break;
    }
  }
}

//++++++++++++++++++enemy++++++++++++++++++++

void game::update_enemy_bullets() {
  for (auto it = enemy_shoots.begin(); it != enemy_shoots.end();) {
    it->update();
    if (it->collision(hero.pos_AABB)) {
      it = enemy_shoots.erase(it);
      std::cout << " - 1 hp" << std::endl;
    } else if (it->out_of_screen() <= -1.f)
      it = enemy_shoots.erase(it);
    else
      ++it;
  }
}
void game::render_enemy_bullets() {
  for (auto it = enemy_shoots.begin(); it != enemy_shoots.end(); ++it) {
    it->render(engine, *bulet_texture);
  }
}

void game::update_enemys() {
  for (auto it_e = enemys.begin(); it_e != enemys.end();) {
    it_e->update(enemy_shoots);
    if (hero.collision(it_e->pos_AABB)) {
      it_e = enemys.erase(it_e);
      engine.myVariable += 1;
    } else if (it_e->out_of_screen() <= -1)
      it_e = enemys.erase(it_e);
    else
      ++it_e;
  }
}
void game::render_enemys() {
  for (auto it_e = enemys.begin(); it_e != enemys.end(); ++it_e) {
    it_e->render(engine, *enemy_green_texture);
  }
}

//++++++++++++++++++wave++++++++++++++++++++

void game::read_wave() {
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
}

void game::update_wave() {
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
}

} // namespace grp
