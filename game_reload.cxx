#include "game.hxx"

game::game(std::unique_ptr<grp::engine, void (*)(grp::engine*)>& engine) {
  hero_texture =
    std::make_unique<grp::texture>(engine.get()->create_texture("./img/pixel_ship_yellow.png"));
}