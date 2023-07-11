#include "game.hxx"
#include "bonus.hxx"
#include "enemy.hxx"
#include "imgui.h"
#include "player.hxx"
#include <fstream>
#include <iostream>

#define MUSIC

// #define ASSPECT

namespace grp {

game::game(iengine& e) : engine(e) {
  read_start_config();

  player::bullet_speed = config["bullet_speed"]["bullet_speed"] / 100.f;
  enemy::bullet_speed = config["bullet_speed"]["bullet_speed"] / 100.f;

  player::configMap = configMap;
  enemy::configMap = configMap;

  textureMap["hero_texture"] = e.create_texture("images/pixel_ship_yellow.png");

  textureMap["bulet_texture"] = e.create_texture("images/pixel_laser_yellow.png");

  textureMap["map_texture"] = e.create_texture("images/background-black.png");

  textureMap["green_b"] = e.create_texture("images/pixel_ship_green_big.png");
  textureMap["green_s"] = e.create_texture("images/pixel_ship_green_small.png");
  textureMap["blue_s"] = e.create_texture("images/pixel_ship_blue_small.png");
  textureMap["blue_m"] = e.create_texture("images/pixel_ship_blue_middle.png");
  textureMap["blue_b"] = e.create_texture("images/pixel_ship_blue_big.png");
  textureMap["red_s"] = e.create_texture("images/pixel_ship_red_small.png");
  textureMap["red_b"] = e.create_texture("images/pixel_ship_red_big.png");

  textureMap["pixel_rocket"] = e.create_texture("images/pixel_rocket.png");
  textureMap["pixel_heart"] = e.create_texture("images/pixel_heart.png");
  textureMap["pixel_health"] = e.create_texture("images/pixel_health.png");

  textureMap["1.0"] = e.create_texture("images/pixel_laser_blue.png");
  textureMap["2.0"] = e.create_texture("images/pixel_laser_red.png");
  textureMap["3.0"] = e.create_texture("images/pixel_laser_green.png");

  hero = player(textureMap["hero_texture"], textureMap["bulet_texture"], config["player"]);
  map = sprite(-1.f, 1.f, 1.f, -1.f, textureMap["map_texture"]);

  start_button = sprite(-0.2f, 0.1f, 0.2f, -0.1f, textureMap["start"]);

#ifdef __ANDROID__
  gamepad_left_texture = e.create_texture("images/left.png");
  left = sprite(-0.97f, -0.35f, -0.77f, -0.6f, gamepad_left_texture);

  gamepad_right_texture = e.create_texture("images/right.png");
  right = sprite(-0.57f, -0.35f, -0.37f, -0.6f, gamepad_right_texture);

  gamepad_up_texture = e.create_texture("images/up.png");
  up = sprite(-0.77f, -0.1f, -0.57f, -0.35f, gamepad_up_texture);

  gamepad_down_texture = e.create_texture("images/down.png");
  down = sprite(-0.77f, -0.6f, -0.57f, -0.85f, gamepad_down_texture);

  shoot_button_texture = e.create_texture("images/shoot.png");
  shoot = sprite(0.57f, -0.35f, 0.77f, -0.6f, shoot_button_texture);

  buttons = {left, right, up, down, shoot};
#endif

#ifdef MUSIC
  music = engine.create_sound("sounds/main.wav");
  kill_sound = engine.create_sound("sounds/E1.wav");
  shoot_sound = engine.create_sound("sounds/laser.wav");
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
  engine.render(map);
  render_bonus();
  render_hero_bullets();
  render_enemy_bullets();
  render_enemys();
  hero.render(engine);

#ifdef __ANDROID__
  render_gamepad();
#endif
}

void game::update() {
  update_wave();
  update_hero_bullets();

  update_enemys();
  hero.update();
  update_enemy_bullets();

  update_bonus();
  count_frame += 1;
}
//++++++++++++++++++bonus++++++++++++++++++++

void game::update_bonus() {
  if (count_frame >= 300) {
    count_frame = 0;
    int type = 1 + rand() % 3;
    if (type == 1) {
      bonuses.push_back(bonus(textureMap["pixel_health"], type));
    }
    if (type == 2) {
      bonuses.push_back(bonus(textureMap["pixel_rocket"], type));
    }
    if (type == 3) {
      bonuses.push_back(bonus(textureMap["pixel_heart"], type));
    }
  }

  for (auto it_bonus = bonuses.begin(); it_bonus != bonuses.end();) {
    it_bonus->update();
    if (it_bonus->collision(hero.sprite)) {
      if (it_bonus->type == 1)
        hero.hp = 100.f;
      if (it_bonus->type == 2) {
        hero.cd_step *= 2;
        hero.bonus_stack += 1;
      }
      if (it_bonus->type == 3)
        HP += 1;
      it_bonus = bonuses.erase(it_bonus);
    } else if (it_bonus->life >= 300)
      it_bonus = bonuses.erase(it_bonus);
    else
      ++it_bonus;
  }
}

void game::render_bonus() {
  for (auto it_bonus = bonuses.begin(); it_bonus != bonuses.end(); ++it_bonus) {
    it_bonus->render(engine);
  }
}

//++++++++++++++++++gamepad++++++++++++++++++++

void game::render_gamepad() {
#ifdef ASSPECT
  left.result_matrix = left.aspect_matrix;
  grp::get_transformed_triangle(left);
#endif

  engine.render(left);
#ifdef ASSPECT
  right.result_matrix = left.aspect_matrix;
  grp::get_transformed_triangle(right);
#endif

  engine.render(right);
#ifdef ASSPECT
  up.result_matrix = up.aspect_matrix;
  grp::get_transformed_triangle(up);
#endif

  engine.render(up);
#ifdef ASSPECT
  down.result_matrix = down.aspect_matrix;
  grp::get_transformed_triangle(down);
#endif

  engine.render(down);
#ifdef ASSPECT
  shoot.result_matrix = shoot.aspect_matrix;
  grp::get_transformed_triangle(shoot);
#endif

  engine.render(shoot);
}

//++++++++++++++++++hero++++++++++++++++++++

void game::update_hero_bullets() {
  for (auto it_bulet = hero.shoots.begin(); it_bulet != hero.shoots.end();) {
    bool flag_collision = false;
    it_bulet->update();

    for (auto it_enemy = enemys.begin(); it_enemy != enemys.end();) {
      if (it_bulet->collision(it_enemy->sprite)) {
        it_enemy->hp -= 1.f;
        flag_collision = true;
        if (it_enemy->hp == 0.f)
          it_enemy = enemys.erase(it_enemy);

#ifdef MUSIC
        kill_sound->play(grp::isound::properties::once);
#endif
        score += 1;
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
    it_b->render(engine);
  }
}

void game::move_player(bool& flag) {
  grp::event event;
  int aspect = engine.weight / engine.height;

#ifdef __ANDROID__
  engine.input_event_android(buttons, grp::keyStates);
#else
  engine.input_event(event, grp::keyStates);
#endif

  // a
  if (grp::keyStates[0] && -0.9f < hero.my_pos[0]) {
    hero.my_pos[0] -= hero.speed_x;
  }
  // d
  if (grp::keyStates[1] && hero.my_pos[0] < 0.9f) {
    hero.my_pos[0] += hero.speed_x;
  }
  // w
  if (grp::keyStates[2] && hero.my_pos[1] < 0.9f / aspect) {
    hero.my_pos[1] += hero.speed_y;
  }
  // s
  if (grp::keyStates[3] && -0.9f / aspect < hero.my_pos[1]) {
    hero.my_pos[1] -= hero.speed_y;
  }
  // space
  if (grp::keyStates[4]) {
#ifdef MUSIC
    shoot_sound->play(grp::isound::properties::once);
#endif
    hero.shoot();
  }
  // escape
  if (grp::keyStates[5]) {
    flag = false;
  }
}

//++++++++++++++++++enemy++++++++++++++++++++

void game::update_enemy_bullets() {
  for (auto it_bulet = enemy_shoots.begin(); it_bulet != enemy_shoots.end();) {
    it_bulet->update();
    if (it_bulet->collision(hero.sprite)) {
      it_bulet = enemy_shoots.erase(it_bulet);
      hero.hp -= 10.f;
      if (hero.hp <= 0)
        life = false;
    } else if (it_bulet->out_of_screen() <= -1.f)
      it_bulet = enemy_shoots.erase(it_bulet);
    else
      ++it_bulet;
  }
}
void game::render_enemy_bullets() {
  for (auto it = enemy_shoots.begin(); it != enemy_shoots.end(); ++it) {
    it->render(engine);
  }
}

void game::update_enemys() {
  for (auto it_enemy = enemys.begin(); it_enemy != enemys.end();) {
    it_enemy->update(enemy_shoots);
    if (hero.collision(it_enemy->sprite)) {
      score += 1;
      hero.hp -= 20.f;
      if (hero.hp <= 0)
        life = false;
      it_enemy->hp -= 1.f;
      if (it_enemy->hp == 0.f)
        it_enemy = enemys.erase(it_enemy);
    } else if (it_enemy->out_of_screen() <= -1) {
      it_enemy = enemys.erase(it_enemy);
      HP -= 1;
      if (HP == 0)
        life = false;
    } else
      ++it_enemy;
  }
}
void game::render_enemys() {
  for (auto it_e = enemys.begin(); it_e != enemys.end(); ++it_e) {
    it_e->render(engine);
  }
}

//++++++++++++++++++wave++++++++++++++++++++

void game::read_wave() {
  using json = nlohmann::json;
  json jsonData = grp::read_data_from_json("wave_list.json");
  for (const auto& [key, value] : jsonData.items()) {
    for (const auto& [type, amount] : value.items()) {
      enemysMap[key][type] = amount;
    }
  }
}

void game::update_wave() {
  if (wave == 20)
    wave = 19;
  if (enemys.empty()) {
    wave += 1;
    std::cout << "wave: " << wave << std::endl;
    for (const auto& pair : enemysMap) {
      if (wave == stoi(pair.first)) {
        for (const auto& pair : pair.second) {
          for (int i = 0; i < pair.second; i++) {
            enemys.push_back(
              enemy(textureMap[pair.first],
                    textureMap[std::to_string(config[pair.first]["laser_img"]).substr(0, 3)],
                    config[pair.first]));
          }
        }
      }
    }
  }
}

//++++++++++++++++++config++++++++++++++++++++

void game::read_start_config() {
  using json = nlohmann::json;
  json jsonData = grp::read_data_from_json("config_list.json");
  for (const auto& [key, value] : jsonData.items()) {
    for (const auto& [type, amount] : value.items()) {
      config[key][type] = amount;
    }
  }
}

//++++++++++++++++++ImGui++++++++++++++++++++
void game::ImGui_menu() {
  ImGuiIO& io = ImGui::GetIO();
  io.FontGlobalScale = engine.height / 480.f;
  float button_scale = engine.height / 480.f;
  // ImGui::SetNextWindowCollapsed(true);
  ImGui::SetNextWindowSize(ImVec2(engine.weight * 0.7f, engine.height * 0.5f));
  if (ImGui::Begin("Develop", 0, 0 | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
    ImGui::SetWindowPos({engine.weight * 0.2f, 0});
    ImGui::SliderFloat("player spead x", &configMap["player_spead_x"], 0.0f, 2.f);
    ImGui::SliderFloat("player spead y", &configMap["player_spead_y"], 0.0f, 2.f);
    ImGui::SliderFloat("player cd", &configMap["player_max_cd"], 0.0f, 2.0f);
    ImGui::SliderFloat("player bullet spead", &configMap["bullet_p_speed"], 0.0f, 2.f);
    ImGui::SliderFloat("enemys speed", &configMap["enemys_speed_y"], 0.0f, 2.f);
    ImGui::SliderFloat("enemys cd", &configMap["enemys_max_cd"], 0.0f, 2.f);
    ImGui::SliderFloat("enemys bullet spead", &configMap["bullet_e_speed"], 0.0f, 2.f);
    if (ImGui::Button("apply", ImVec2(50 * button_scale, 30 * button_scale))) {
      enemy::configMap = configMap;
      enemy::bullet_speed =
        config["bullet_speed"]["bullet_speed"] / 100.f * configMap["bullet_e_speed"];
      hero.speed_x = config["player"]["player_spead_x"] / 100.f * configMap["player_spead_x"];
      hero.speed_y = config["player"]["player_spead_y"] / 100.f * configMap["player_spead_y"];
      hero.max_cd = config["player"]["player_max_cd"] * configMap["player_max_cd"];
      player::bullet_speed =
        config["bullet_speed"]["bullet_speed"] / 100.f * configMap["bullet_p_speed"];
    }
  }
  ImGui::End();

  constexpr ImGuiWindowFlags game_gui_flags =
    0 | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav |
    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
  ImGui::SetNextWindowSize(ImVec2(engine.weight * 0.2f, engine.height * 0.2f));
  if (ImGui::Begin("Score", 0, game_gui_flags)) {
    ImGui::SetWindowPos({0, 0});
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("Score: %d", score);
    ImGui::Text("Wave: %d", wave);
    ImGui::Text("HP: %d", HP);
    ImGui::SetWindowFontScale(1.f);
    ImGui::End();
  }

  if (!life) {
    ImGui::SetNextWindowPos(
      {static_cast<float>(engine.weight / 2.f - 42.f * button_scale), engine.height / 2.f});
    ImGui::SetNextWindowSize(ImVec2(engine.weight * 0.2f, engine.height * 0.2f));
    if (ImGui::Begin("restart", 0, game_gui_flags)) {
      if (ImGui::Button("Restart", ImVec2(70 * button_scale, 40 * button_scale))) {
        life = true;
        restart();
      }
      ImGui::End();
    }
  }
}

void game::ImGui_hp_bar() {
  ImGui::SetNextWindowPos(
    {static_cast<float>(engine.weight / 2.f - 80 * 1), engine.height - 60.f * 1});
  ImGui::SetNextWindowSize({175.f, 200 * 1});
  if (ImGui::Begin("HP",
                   nullptr,
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::SetWindowFontScale(1.f);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImGui::Text("Health");
    ImVec2 health_bar_size = ImVec2(160 * 1.f, 30 * 1.f);

    float fill = static_cast<float>(hero.hp) / 100.f;

    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 p1 = ImVec2(p0.x + health_bar_size.x * (1 - fill), p0.y + health_bar_size.y);
    ImVec2 p2 = ImVec2(p0.x + health_bar_size.x, p0.y);
    ImU32 col_r = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
    ImU32 col_g = ImGui::GetColorU32(IM_COL32(0, 255, 0, 255));
    draw_list->AddRectFilledMultiColor(p0, p1, col_r, col_r, col_r, col_r);
    draw_list->AddRectFilledMultiColor(p1, p2, col_g, col_g, col_g, col_g);

    ImGui::End();
  }
}

void game::restart() {
  enemy_shoots.clear();
  enemys.clear();
  hero.clear();
  bonuses.clear();
  HP = 5;
  wave = 0;
}

} // namespace grp
