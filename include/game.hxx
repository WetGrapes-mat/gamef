#pragma once
#include <map>
#include <vector>

#include "engine.hxx"

#include "bullet.hxx"
#include "enemy.hxx"
#include "engine.hxx"
#include "player.hxx"

namespace grp {

class game : public igame {
  public:
    player hero;
    std::vector<bullet> enemy_shoots;
    std::vector<enemy> enemys;
    std::map<std::string, std::map<std::string, int>> enemysMap;
    std::map<std::string, float> configMap;

    int wave = 0;
    iengine& engine;

    game(iengine& e);
    ~game() override;

    void update() override;
    void render() override;
    void move_player(bool& flag);
    void ImGui_menu() override;

  private:
    bool debug_draw = false;
    int score = 0;
    float bullet_e_speed = 0.0f;
    float bullet_p_speed = 0.0f;
    float player_spead_x = 0.0f;
    float player_spead_y = 0.0f;

    void read_wave();
    void read_config();

    void update_wave();
    void update_enemy_bullets();
    void render_enemy_bullets();
    void update_hero_bullets();
    void render_hero_bullets();

    void update_enemys();
    void render_enemys();

    void render_gamepad();

    grp::texture* hero_texture;
    grp::texture* bulet_texture;
    grp::texture* map_texture;
    grp::texture* enemy_green_texture;

    grp::isound* music;
    grp::isound* kill_sound;
    grp::isound* shoot_sound;
    grp::sprite map = sprite(-1.f, 1.f, 1.f, -1.f);

    std::vector<grp::sprite> buttons;

    grp::texture* gamepad_left_texture;
    grp::sprite left = sprite(-0.97f, -0.35f, -0.77f, -0.6f);

    grp::texture* gamepad_right_texture;
    grp::sprite right = sprite(-0.57f, -0.35f, -0.37f, -0.6f);

    grp::texture* gamepad_up_texture;
    grp::sprite up = sprite(-0.77f, -0.1f, -0.57f, -0.35f);

    grp::texture* gamepad_down_texture;
    grp::sprite down = sprite(-0.77f, -0.6f, -0.57f, -0.85f);

    grp::texture* shoot_button_texture;
    grp::sprite shoot = sprite(0.57f, -0.35f, 0.77f, -0.6f);
};

} // namespace grp