#include <map>
#include <vector>

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
    int wave = 0;
    iengine& engine;

    game(iengine& e);
    ~game() override;

    void update() override;
    void render() override;
    void move_player(bool& flag);

  private:
    void read_wave();
    void update_wave();
    void update_enemy_bullets();
    void render_enemy_bullets();
    void update_hero_bullets();
    void render_hero_bullets();

    void update_enemys();
    void render_enemys();

    grp::texture* hero_texture;
    grp::texture* bulet_texture;
    grp::texture* map_texture;
    grp::texture* enemy_green_texture;

    grp::isound* music;
    grp::isound* kill_sound;
    grp::isound* shoot_sound;
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

} // namespace grp