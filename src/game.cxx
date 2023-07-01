#include "game.hxx"
#include "enemy.hxx"
#include "player.hxx"
#include <fstream>

#define MUSIC

// #define ASSPECT

namespace grp {

game::game(iengine& e) : engine(e) {
  read_config();
  player::set_config(configMap);
  enemy::set_config(configMap);
  bullet_p_speed = configMap["bullet_speed"];
  bullet_e_speed = configMap["bullet_speed"];
  player_spead_x = configMap["player_spead_x"] * 1000;
  player_spead_y = configMap["player_spead_y"] * 1000;

  hero = player();
  hero_texture = e.create_texture("images/pixel_ship_yellow.png");
  bulet_texture = e.create_texture("images/pixel_laser_yellow.png");
  map_texture = e.create_texture("images/background-black.png");
  enemy_green_texture = e.create_texture("images/pixel_ship_green_big.png");

#ifdef __ANDROID__
  gamepad_left_texture = e.create_texture("images/left.png");
  gamepad_right_texture = e.create_texture("images/right.png");
  gamepad_up_texture = e.create_texture("images/up.png");
  gamepad_down_texture = e.create_texture("images/down.png");
  shoot_button_texture = e.create_texture("images/shoot.png");
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
  engine.render(map, map_texture);
  hero.render(engine, *hero_texture);
  render_hero_bullets();
  render_enemy_bullets();
  render_enemys();
#ifdef __ANDROID__
  render_gamepad();
#endif
}

void game::update() {
  update_wave();
  hero.update();
  update_hero_bullets();

  update_enemy_bullets();
  update_enemys();
}
//++++++++++++++++++gamepad++++++++++++++++++++

void game::render_gamepad() {
#ifdef ASSPECT
  left.result_matrix = left.aspect_matrix;
  grp::get_transformed_triangle(left);
#endif

  engine.render(left, gamepad_left_texture);
#ifdef ASSPECT
  right.result_matrix = left.aspect_matrix;
  grp::get_transformed_triangle(right);
#endif

  engine.render(right, gamepad_right_texture);
#ifdef ASSPECT
  up.result_matrix = up.aspect_matrix;
  grp::get_transformed_triangle(up);
#endif

  engine.render(up, gamepad_up_texture);
#ifdef ASSPECT
  down.result_matrix = down.aspect_matrix;
  grp::get_transformed_triangle(down);
#endif

  engine.render(down, gamepad_down_texture);
#ifdef ASSPECT
  shoot.result_matrix = shoot.aspect_matrix;
  grp::get_transformed_triangle(shoot);
#endif

  engine.render(shoot, shoot_button_texture);
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
    it_b->render(engine, *bulet_texture);
  }
}

void game::move_player(bool& flag) {
  grp::event event;

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
  if (grp::keyStates[2] && hero.my_pos[1] < 0.65f) {
    hero.my_pos[1] += hero.speed_y;
  }
  // s
  if (grp::keyStates[3] && -0.65f < hero.my_pos[1]) {
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
      score += 1;
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
  json jsonData = grp::read_data_from_json("config_game.json");
  for (const auto& [key, value] : jsonData.items()) {
    configMap[key] = value;
  }
}

void game::ImGui_menu() {
  ImGuiIO& io = ImGui::GetIO();
  io.FontGlobalScale = engine.height / 480.f;
  float button_scale = engine.height / 480.f;
  // ImGui::SetNextWindowCollapsed(true);
  ImGui::SetNextWindowSize(ImVec2(engine.weight * 0.7f, engine.height * 0.5f));
  if (ImGui::Begin("Develop", 0, 0 | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
    ImGui::SetWindowPos({engine.weight * 0.2f, 0});
    ImGui::SliderFloat("player spead x", &player_spead_x, 0.0f, 10.f);
    ImGui::SliderFloat("player spead y", &player_spead_y, 0.0f, 10.f);
    ImGui::SliderFloat("player cd", &configMap["player_max_cd"], 0.0f, 20.0f);
    ImGui::SliderFloat("player bullet spead", &bullet_p_speed, 0.0f, 3.f);
    ImGui::SliderFloat("enemys speed", &configMap["enemys_speed_y"], 0.0f, 1.f);
    ImGui::SliderFloat("enemys cd", &configMap["enemys_max_cd"], 0.0f, 30.f);
    ImGui::SliderFloat("enemys bullet spead", &bullet_e_speed, 0.0f, 2.f);
    if (ImGui::Button("apply", ImVec2(50 * button_scale, 30 * button_scale))) {
      configMap["player_spead_x"] = player_spead_x / 1000;
      configMap["player_spead_y"] = player_spead_y / 1000;
      player::set_config(configMap);
      enemy::set_config(configMap);
      player::bullet_speed = bullet_p_speed / 100;
      enemy::bullet_speed = bullet_e_speed / 100;
    }
  }
  ImGui::End();

  constexpr ImGuiWindowFlags game_gui_flags =
    0 | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav |
    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
  ImGui::SetNextWindowSize(ImVec2(engine.weight * 0.2f, engine.height * 0.1f));
  if (ImGui::Begin("Score", 0, game_gui_flags)) {
    ImGui::SetWindowPos({0, 0});
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("Score: %d", score);
    ImGui::Text("Wave: %d", wave);

    ImGui::SetWindowFontScale(1.f);
    ImGui::End();
  }
}

} // namespace grp
