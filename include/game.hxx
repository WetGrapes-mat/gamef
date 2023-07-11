#pragma once
#include <map>
#include <vector>

#include "engine.hxx"

#include "bonus.hxx"
#include "bullet.hxx"
#include "enemy.hxx"
#include "engine.hxx"
#include "player.hxx"

namespace grp {

class game : public igame {
  public:
    iengine& engine;

    player hero;
    std::vector<bullet> enemy_shoots;
    std::vector<enemy> enemys;
    std::vector<bonus> bonuses;

    std::map<std::string, std::map<std::string, int>> enemysMap;
    std::map<std::string, std::map<std::string, float>> config;
    std::map<std::string, float> configMap = {
      {"player_spead_x", 1.f},
      {"player_spead_y", 1.f},
      { "player_max_cd", 1.f},
      {"bullet_p_speed", 1.f},
      {"enemys_speed_y", 1.f},
      { "enemys_max_cd", 1.f},
      {"bullet_e_speed", 1.f}
    };

    std::map<std::string, grp::texture*> textureMap;

    bool life = true;

    game(iengine& e);
    ~game() override;

    void update() override;
    void render() override;
    void move_player(bool& flag);
    void ImGui_menu() override;
    void ImGui_hp_bar() override;

  private:
    int wave = 0;

    int score = 0;
    int HP = 3;
    int count_frame = 0;

    void read_wave();
    void read_start_config();

    void update_wave();
    void update_enemy_bullets();
    void render_enemy_bullets();
    void update_hero_bullets();
    void render_hero_bullets();

    void update_enemys();
    void render_enemys();

    void update_bonus();
    void render_bonus();

    void render_gamepad();

    void restart();

    grp::sprite map;
    grp::sprite start_button;

    grp::isound* music;
    grp::isound* kill_sound;
    grp::isound* shoot_sound;

    std::vector<grp::sprite> buttons;

    grp::texture* gamepad_left_texture;
    grp::sprite left;

    grp::texture* gamepad_right_texture;
    grp::sprite right;

    grp::texture* gamepad_up_texture;
    grp::sprite up;

    grp::texture* gamepad_down_texture;
    grp::sprite down;

    grp::texture* shoot_button_texture;
    grp::sprite shoot;
};

} // namespace grp