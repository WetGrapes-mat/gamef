#include "game.hxx"
#include "enemy.hxx"
#include "nlohmann/json.hpp"
#include "player.hxx"
#include <fstream>

// #define MUSIC

namespace grp {

game::game(iengine& e) : engine(e) {
  read_config();
  player::set_config(configMap);
  enemy::set_config(configMap);

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
  engine.render(map, map_texture);
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
  for (auto it_bulet = hero.shoots.begin(); it_bulet != hero.shoots.end();) {
    bool flag_collision = false;
    it_bulet->update();

    for (auto it_enemy = enemys.begin(); it_enemy != enemys.end();) {
      if (it_bulet->collision(it_enemy->sprite)) {
        it_enemy = enemys.erase(it_enemy);
        flag_collision = true;
#ifdef MUSIC
        kill_sound->play(grp::isound::properties::once);
#endif
        myVariable += 1;
        break;
      } else
        ++it_enemy;
    }
    if (flag_collision)
      it_bulet = hero.shoots.erase(it_bulet);
    else
      ++it_bulet;
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
  for (auto it_bulet = enemy_shoots.begin(); it_bulet != enemy_shoots.end();) {
    it_bulet->update();
    if (it_bulet->collision(hero.sprite)) {
      it_bulet = enemy_shoots.erase(it_bulet);
      std::cout << " - 1 hp" << std::endl;
    } else if (it_bulet->out_of_screen() <= -1.f)
      it_bulet = enemy_shoots.erase(it_bulet);
    else
      ++it_bulet;
  }
}
void game::render_enemy_bullets() {
  for (auto it = enemy_shoots.begin(); it != enemy_shoots.end(); ++it) {
    it->render(engine, *bulet_texture);
  }
}

void game::update_enemys() {
  for (auto it_enemy = enemys.begin(); it_enemy != enemys.end();) {
    it_enemy->update(enemy_shoots);
    if (hero.collision(it_enemy->sprite)) {
      it_enemy = enemys.erase(it_enemy);
      myVariable += 1;
    } else if (it_enemy->out_of_screen() <= -1)
      it_enemy = enemys.erase(it_enemy);
    else
      ++it_enemy;
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
            // std::cout << enemy::speed_x << std::endl;
            enemys.push_back(enemy());
          }
        }
      }
    }
  }
}

//++++++++++++++++++config++++++++++++++++++++

void game::read_config() {
  using json = nlohmann::json;
  std::ifstream file("config_game.json");
  json jsonData;
  file >> jsonData;
  file.close();
  for (const auto& [key, value] : jsonData.items()) {
    configMap[key] = value;
  }
}

void game::ImGui_menu() {
  constexpr ImGuiWindowFlags game_gui_flags =
    0 | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav |
    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
  // clang-format on
  if (ImGui::Begin("Guns", 0, game_gui_flags)) {
    ImGui::SetWindowPos({0, 0});
    ImGui::Checkbox("Debug draw", &debug_draw);
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("Score: %d", myVariable);
    ImGui::SetWindowFontScale(1.f);

    ImGui::Text("Test");

    ImGui::End();
  }
  if (ImGui::Begin("Develop")) {
    ImGui::Checkbox("Debug draw", &debug_draw);
  }
  ImGui::End();
}

} // namespace grp
