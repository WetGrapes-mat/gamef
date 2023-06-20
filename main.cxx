#include <cstdlib>
#include <iostream>

#include "game.hxx"

int main(int /*argc*/, char* /*argv*/[]) {
  using namespace std;

  grp::iengine* engine = grp::create_engine();
  engine->initialize("");
  grp::game* game = new grp::game(*engine);
  engine->set_game(game);

  bool continue_loop = true;
  while (continue_loop) {
    game->move_player(continue_loop);

    game->update();

    game->render();

    engine->draw_imgui();

    engine->swap_buffers();
  }

  engine->uninitialize();

  return EXIT_SUCCESS;
}
