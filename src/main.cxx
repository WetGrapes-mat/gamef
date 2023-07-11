#include <cstdlib>
#include <iostream>
#include <thread>

#include "game.hxx"

#ifdef __ANDROID__
#define main grp_Game
#endif

extern "C" int main(int /*argc*/, char* /*argv*/[]) {
  using namespace std;

  grp::iengine* engine = grp::create_engine();
  engine->initialize("");
  grp::game* game = new grp::game(*engine);
  engine->set_game(game);
  using clock_timer = std::chrono::high_resolution_clock;
  using nano_sec = std::chrono::nanoseconds;
  using milli_sec = std::chrono::milliseconds;
  using time_point = std::chrono::time_point<clock_timer, nano_sec>;
  clock_timer timer;

  time_point start = timer.now();

  bool continue_loop = true;
  while (continue_loop) {
    time_point end_last_frame = timer.now();

    game->move_player(continue_loop);

    milli_sec frame_delta = std::chrono::duration_cast<milli_sec>(end_last_frame - start);

    if (frame_delta.count() < 24) // 1000 % 60 = 16.666 FPS
    {
      std::this_thread::yield();  // too fast, give other apps CPU time
      continue;                   // wait till more time
    }
    if (game->life)
      game->update();

    game->render();

    engine->draw_imgui();

    engine->swap_buffers();
    start = end_last_frame;
  }

  engine->uninitialize();

  return EXIT_SUCCESS;
}
