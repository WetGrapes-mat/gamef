#include "engine.hxx"

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <vector>

#include "SDL3/SDL_keycode.h"
#include <SDL3/SDL.h>

#include "picopng.hxx"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CHECK_OPENGL()                                                  \
  {                                                                     \
    const int err = static_cast<int>(glGetError());                     \
    if (err != GL_NO_ERROR) {                                           \
      switch (err) {                                                    \
        case GL_INVALID_ENUM:                                           \
          std::cerr << "GL_INVALID_ENUM" << std::endl;                  \
          break;                                                        \
        case GL_INVALID_VALUE:                                          \
          std::cerr << "GL_INVALID_VALUE" << std::endl;                 \
          break;                                                        \
        case GL_INVALID_OPERATION:                                      \
          std::cerr << "GL_INVALID_OPERATION" << std::endl;             \
          break;                                                        \
        case GL_INVALID_FRAMEBUFFER_OPERATION:                          \
          std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; \
          break;                                                        \
        case GL_OUT_OF_MEMORY:                                          \
          std::cerr << "GL_OUT_OF_MEMORY" << std::endl;                 \
          break;                                                        \
      }                                                                 \
      assert(false);                                                    \
    }                                                                   \
  }

namespace grp {
//+++++++++++++++++++++++++++++++EVENT+++++++++++++++++++++++++++++++
using namespace std;

const vector<tuple<SDL_Keycode, string, event, event>> keys {
  {     SDLK_w,      "w",      event::w_pressed,      event::w_released},
  {     SDLK_s,      "s",      event::s_pressed,      event::s_released},
  {     SDLK_a,      "a",      event::a_pressed,      event::a_released},
  {     SDLK_d,      "d",      event::d_pressed,      event::d_released},
  { SDLK_SPACE,  "space",  event::space_pressed,  event::space_released},
  {SDLK_ESCAPE, "escape", event::escape_pressed, event::escape_released}
};
const map<event, std::string> event_names {
  {      event::w_pressed,       "w_pressed"},
  {     event::w_released,      "w_released"},
  {      event::s_pressed,       "s_pressed"},
  {     event::s_released,      "s_released"},
  {      event::d_pressed,       "d_pressed"},
  {     event::d_released,      "d_released"},
  {      event::a_pressed,       "a_pressed"},
  {     event::a_released,      "a_released"},
  {  event::space_pressed,   "space_pressed"},
  { event::space_released,  "space_released"},
  { event::escape_pressed,  "escape_pressed"},
  {event::escape_released, "escape_released"},
  {       event::turn_off,           "close"}
};

//+++++++++++++++++++++++++++++++SOUND++++++++++++++++++++++++++++++

static std::string_view get_sound_format_name(uint16_t format_value) {
  static const std::map<int, std::string_view> format = {
    {    AUDIO_U8,     "AUDIO_U8"},
    {    AUDIO_S8,     "AUDIO_S8"},
    {AUDIO_S16LSB, "AUDIO_S16LSB"},
    {AUDIO_S16MSB, "AUDIO_S16MSB"},
    {AUDIO_S32LSB, "AUDIO_S32LSB"},
    {AUDIO_S32MSB, "AUDIO_S32MSB"},
    {AUDIO_F32LSB, "AUDIO_F32LSB"},
    {AUDIO_F32MSB, "AUDIO_F32MSB"},
  };

  auto it = format.find(format_value);
  return it->second;
}

static std::size_t get_sound_format_size(uint16_t format_value) {
  static const std::map<int, std::size_t> format = {
    {    AUDIO_U8, 1},
    {    AUDIO_S8, 1},
    {AUDIO_S16LSB, 2},
    {AUDIO_S16MSB, 2},
    {AUDIO_S32LSB, 4},
    {AUDIO_S32MSB, 4},
    {AUDIO_F32LSB, 4},
    {AUDIO_F32MSB, 4},
  };

  auto it = format.find(format_value);
  return it->second;
}
//+++++++++++++++++++++++++++++++INIT_HELP+++++++++++++++++++++++++++++++

bool keyStates[6] = {false};

std::ostream& operator<<(std::ostream& out, event e) {
  out << event_names.at(e);
  return out;
}

static std::ostream& operator<<(std::ostream& out, const SDL_version& v) {
  out << static_cast<int>(v.major) << '.';
  out << static_cast<int>(v.minor) << '.';
  out << static_cast<int>(v.patch);
  return out;
}

std::istream& operator>>(std::istream& is, vertex& v) {
  is >> v.x;
  is >> v.y;
  is >> v.z;
  return is;
}

class sound;
isound::~isound() {}

//+++++++++++++++++++++++++++++++TEXTURE+++++++++++++++++++++++++++++++
class opengl_texture : public texture {
  public:
    void bind() {
      glBindTexture(GL_TEXTURE_2D, gl_texture_id);
      CHECK_OPENGL()
    }

    void load(const std::string_view path) {
      std::vector<unsigned char> png_file_in_memory;
      std::ifstream ifs(path.data(), std::ios_base::binary);
      if (!ifs) {
        throw std::runtime_error("can't load texture1");
      }
      ifs.seekg(0, std::ios_base::end);
      std::streamoff pos_in_file = ifs.tellg();
      png_file_in_memory.resize(static_cast<size_t>(pos_in_file));
      ifs.seekg(0, std::ios_base::beg);
      if (!ifs) {
        throw std::runtime_error("can't load texture2");
      }
      ifs.read(reinterpret_cast<char*>(png_file_in_memory.data()), pos_in_file);

      if (!ifs.good()) {
        throw std::runtime_error("can't load texture3");
      }

      std::vector<unsigned char> image;

      int error = decodePNG(image,
                            gl_texture_width,
                            gl_texture_height,
                            &png_file_in_memory[0],
                            png_file_in_memory.size(),
                            false);
      if (error != 0) {
        std::cerr << "error: " << error << std::endl;
        throw std::runtime_error("can't load texture4");
      }

      glGenTextures(1, &gl_texture_id);
      CHECK_OPENGL()
      glBindTexture(GL_TEXTURE_2D, gl_texture_id);
      CHECK_OPENGL();

      GLint mipmap_level = 0;
      GLint border = 0;
      GLsizei width_ = static_cast<GLsizei>(gl_texture_width);
      GLsizei height_ = static_cast<GLsizei>(gl_texture_height);
      glTexImage2D(GL_TEXTURE_2D,
                   mipmap_level,
                   GL_RGBA,
                   width_,
                   height_,
                   border,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   &image[0]);

      CHECK_OPENGL()

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      CHECK_OPENGL()
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      CHECK_OPENGL()
    }

  private:
    GLuint gl_texture_id = {};
    unsigned long gl_texture_width = 0;
    unsigned long gl_texture_height = 0;
};

//+++++++++++++++++++++++++++++++ENGINE+++++++++++++++++++++++++++++++

class engine final : public iengine {
  public:
    glm::mat3 matrix {};
    int weight = 640;
    int height = 480;
    std::string initialize(std::string_view /*config*/) final {
      using namespace std;

      stringstream serr;

      const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
      if (init_result != 0) {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_Init: " << err_message << endl;
        return serr.str();
      }

      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

      window = SDL_CreateWindow("title", weight, height, ::SDL_WINDOW_OPENGL);
      if (window == nullptr) {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return serr.str();
      }
      SDL_GetWindowSizeInPixels(window, &weight, &height);

      int gl_major_ver = 4;
      int gl_minor_ver = 1;
      int gl_context_profile = SDL_GL_CONTEXT_PROFILE_CORE;

      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);

      SDL_GLContext gl_context = SDL_GL_CreateContext(window);

      assert(gl_context != nullptr);

      std::clog << "OpenGl " << gl_major_ver << '.' << gl_minor_ver << '\n';

      auto load_gl_pointer = [](const char* function_name) {
        SDL_FunctionPointer function_ptr = SDL_GL_GetProcAddress(function_name);
        return reinterpret_cast<void*>(function_ptr);
      };
      if (gladLoadGLLoader(load_gl_pointer) == 0) {
        std::clog << "error: failed to initialize glad" << std::endl;
      }

      glViewport(0, 0, weight, height);
      CHECK_OPENGL()

      glGenBuffers(1, &vbo);
      CHECK_OPENGL()

      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      CHECK_OPENGL()

      glGenVertexArrays(1, &vao);
      CHECK_OPENGL()
      glBindVertexArray(vao);
      CHECK_OPENGL()

      triangle_program.load_shader(GL_VERTEX_SHADER, "./shader/triangle_vertex.shader");
      triangle_program.load_shader(GL_FRAGMENT_SHADER, "./shader/triangle_fragment.shader");
      triangle_program.prepare_program();

      texture_program.load_shader(GL_VERTEX_SHADER, "./shader/texture_vertex.shader");
      texture_program.load_shader(GL_FRAGMENT_SHADER, "./shader/texture_fragment.shader");
      texture_program.prepare_program();

      glEnable(GL_BLEND);
      CHECK_OPENGL()
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      CHECK_OPENGL()

      // initialize audio
      audio_device_spec.freq = 48000;
      audio_device_spec.format = AUDIO_S16LSB;
      audio_device_spec.channels = 2;
      audio_device_spec.samples = 1024; // must be power of 2
      audio_device_spec.callback = engine::audio_callback;
      audio_device_spec.userdata = this;

      const int num_audio_drivers = SDL_GetNumAudioDrivers();
      for (int i = 0; i < num_audio_drivers; ++i) {
        std::cout << "audio_driver #:" << i << " " << SDL_GetAudioDriver(i) << '\n';
      }
      std::cout << std::flush;

      const char* default_audio_device_name = nullptr;

      // SDL_FALSE - mean get only OUTPUT audio devices
      const int num_audio_devices = SDL_GetNumAudioDevices(SDL_FALSE);
      if (num_audio_devices > 0) {
        default_audio_device_name = SDL_GetAudioDeviceName(num_audio_devices - 1, SDL_FALSE);
        for (int i = 0; i < num_audio_devices; ++i) {
          std::cout << "audio device #" << i << ": " << SDL_GetAudioDeviceName(i, SDL_FALSE)
                    << '\n';
        }
      }
      std::cout << std::flush;

      audio_device = SDL_OpenAudioDevice(
        default_audio_device_name, 0, &audio_device_spec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);

      if (audio_device == 0) {
        std::cerr << "failed open audio device: " << SDL_GetError();
        throw std::runtime_error("audio failed");
      } else {
        std::cout << "--------------------------------------------\n";
        std::cout << "audio device selected: " << default_audio_device_name << '\n'
                  << "freq: " << audio_device_spec.freq << '\n'
                  << "format: " << get_sound_format_name(audio_device_spec.format) << '\n'
                  << "channels: " << static_cast<uint32_t>(audio_device_spec.channels) << '\n'
                  << "samples: " << audio_device_spec.samples << '\n'
                  << std::flush;

        // unpause device
        SDL_PlayAudioDevice(audio_device);
      }

      return "";
    }

    bool input_event(event& e, bool* state_key) {
      SDL_Event event;
      // std::cout << state_key[0] << state_key[1] << state_key[2] << state_key[3] << state_key[4]
      //           << state_key[5] << std::endl;
      if (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
          state_key[5] = true;
          e = event::turn_off;
          return true;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
          for (int i = 0; i < keys.size(); i++) {
            if (std::get<0>(keys[i]) == event.key.keysym.sym) {
              state_key[i] = true;
              e = std::get<2>(keys[i]);
              return true;
            }
          }
        } else if (event.type == SDL_EVENT_KEY_UP) {
          for (int i = 0; i < keys.size(); i++) {
            if (std::get<0>(keys[i]) == event.key.keysym.sym) {
              state_key[i] = false;
              e = std::get<3>(keys[i]);
              return true;
            }
          }
        }
      }
      return false;
    }

    void render(const triangle& t) final {
      glEnableVertexAttribArray(0);
      CHECK_OPENGL()

      glEnableVertexAttribArray(1);
      CHECK_OPENGL()

      triangle_program.apply_shader_program();

      glBufferData(GL_ARRAY_BUFFER, sizeof(t.vertices), t.vertices.data(), GL_STATIC_DRAW);
      CHECK_OPENGL()

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(0));
      CHECK_OPENGL()

      glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(3 * sizeof(float)));

      CHECK_OPENGL()

      glDrawArrays(GL_TRIANGLES, 0, 3);
      CHECK_OPENGL()

      glDisableVertexAttribArray(0);
      CHECK_OPENGL()

      glDisableVertexAttribArray(1);
      CHECK_OPENGL()
    }

    void render(const triangle& triangle, texture* const texture) {
      glEnableVertexAttribArray(0);
      CHECK_OPENGL()

      glEnableVertexAttribArray(1);
      CHECK_OPENGL()

      glEnableVertexAttribArray(2);
      CHECK_OPENGL()

      texture_program.apply_shader_program();
      texture->bind();

      glBufferData(
        GL_ARRAY_BUFFER, sizeof(triangle.vertices), triangle.vertices.data(), GL_STATIC_DRAW);
      CHECK_OPENGL()

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(0));
      CHECK_OPENGL()

      glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(3 * sizeof(float)));
      CHECK_OPENGL()

      glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(6 * sizeof(float)));
      CHECK_OPENGL()

      glDrawArrays(GL_TRIANGLES, 0, 3);
      CHECK_OPENGL()

      glDisableVertexAttribArray(0);
      CHECK_OPENGL()

      glDisableVertexAttribArray(1);
      CHECK_OPENGL()

      glDisableVertexAttribArray(2);
      CHECK_OPENGL()
    }

    void swap_buffers() final {
      SDL_GL_SwapWindow(window);
      glClearColor(1.f, 1.f, 1.f, .0f);
      CHECK_OPENGL()
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      CHECK_OPENGL()
    }
    void uninitialize() final {}

    texture* create_texture(std::string_view path) {
      texture* r = new opengl_texture();
      r->load(path);
      return r;
      ;
    }

    isound* create_sound(std::string_view path) final;
    void destroy_sound(isound* sound) final {
      // TODO FIXME first remove from sounds collection
      delete sound;
    }

  private:
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;
    GLuint vbo {};
    GLuint vao {};
    opengl_shader_program triangle_program {};
    opengl_shader_program texture_program {};

    friend class sound; // for audio_mutex
    std::vector<sound*> sounds;
    static void audio_callback(void*, uint8_t*, int);
    static std::mutex audio_mutex;
    SDL_AudioDeviceID audio_device;
    SDL_AudioSpec audio_device_spec;
};
iengine::~iengine() {}

//+++++++++++++++++++++++++++++++SHADER_PROGRAM+++++++++++++++++++++++++++++++

opengl_shader_program::~opengl_shader_program() {
  std::for_each(shaders_.begin(), shaders_.end(), [](const GLuint shader_id) {
    glDeleteShader(shader_id);
    CHECK_OPENGL();
  });

  glDeleteProgram(program_);
  CHECK_OPENGL();
}

void opengl_shader_program::load_shader(const GLenum shader_type,
                                        const std::string_view shader_path) {
  GLuint shader_id = glCreateShader(shader_type);
  CHECK_OPENGL();

  const std::string shader_code_string = get_shader_code_from_file(shader_path);
  const char* shader_code = shader_code_string.data();

  glShaderSource(shader_id, 1, &shader_code, nullptr);
  CHECK_OPENGL();

  glCompileShader(shader_id);
  CHECK_OPENGL();

  GLint shader_compiled {};
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);
  CHECK_OPENGL();
  if (!shader_compiled) {
    glDeleteShader(shader_id);
    CHECK_OPENGL();
    throw std::runtime_error {"Error on compiling shader"};
  }

  shaders_.push_back(shader_id);
}

void opengl_shader_program::apply_shader_program() {
  glUseProgram(program_);
  CHECK_OPENGL();
}

void opengl_shader_program::prepare_program() {
  attach_shaders();
  link_program();
  validate_program();
}

void opengl_shader_program::attach_shaders() {
  program_ = glCreateProgram();
  CHECK_OPENGL();

  std::for_each(shaders_.begin(), shaders_.end(), [this](const GLuint shader_id) {
    glAttachShader(program_, shader_id);
    CHECK_OPENGL();
  });
}

void opengl_shader_program::link_program() const {
  glLinkProgram(program_);
  CHECK_OPENGL();

  GLint linked {};
  glGetProgramiv(program_, GL_LINK_STATUS, &linked);
  CHECK_OPENGL();

  if (!linked) {
    glDeleteProgram(program_);
    CHECK_OPENGL();
    throw std::runtime_error {"Error on linking shader program"};
  }
}

void opengl_shader_program::validate_program() const {
  glValidateProgram(program_);
  CHECK_OPENGL();

  GLint is_validated {};
  glGetProgramiv(program_, GL_VALIDATE_STATUS, &is_validated);
  CHECK_OPENGL();

  if (!is_validated) {
    glDeleteProgram(program_);
    CHECK_OPENGL();
    throw std::runtime_error {"Error on validating shader program"};
  }
}

std::string opengl_shader_program::get_shader_code_from_file(const std::string_view path) const {
  std::string shader_code {};

  std::ifstream file {path.data()};

  const std::filesystem::path fs_path {path.data()};

  std::streamsize bytes_to_read = std::filesystem::file_size(fs_path);

  shader_code.resize(bytes_to_read);

  file.read(shader_code.data(), bytes_to_read);

  file.close();
  return shader_code;
}

static bool already_exist = false;

iengine* create_engine() {
  if (already_exist) {
    throw std::runtime_error("engine already exist");
  }
  iengine* result = new engine();
  already_exist = true;
  return result;
}

void destroy_engine(iengine* e) {
  if (already_exist == false) {
    throw std::runtime_error("engine not created");
  }
  if (nullptr == e) {
    throw std::runtime_error("e is nullptr");
  }
  delete e;
}

grp::triangle get_transformed_triangle(const grp::triangle& t,
                                       const glm::mediump_mat3& result_matrix) {
  grp::triangle result {t};

  std::for_each(result.vertices.begin(), result.vertices.end(), [&](grp::vertex& v) {
    glm::vec3 v_pos_source {v.x, v.y, 1.f};
    glm::vec3 v_pos_result = result_matrix * v_pos_source;

    v.x = v_pos_result[0];
    v.y = v_pos_result[1];
  });

  return result;
}

//+++++++++++++++++++++++++++++++SOUND++++++++++++++++++++++++++++++
class sound final : public isound {
  public:
    sound(std::string_view path, SDL_AudioDeviceID device, SDL_AudioSpec audio_spec);
    ~sound() final;

    void play(const properties prop) final {
      std::lock_guard<std::mutex> lock(engine::audio_mutex);
      // here we can change properties
      // of sound and dont collade with multithreaded playing
      current_index = 0;
      is_playing = true;
      is_looped = (prop == properties::looped);
    }

    std::unique_ptr<uint8_t[]> tmp_buf;
    uint8_t* buffer;
    uint32_t length;
    uint32_t current_index = 0;
    SDL_AudioDeviceID device;
    bool is_playing = false;
    bool is_looped = false;
};

sound::sound(std::string_view path, SDL_AudioDeviceID device_, SDL_AudioSpec device_audio_spec) :
  buffer(nullptr), length(0), device(device_) {
  SDL_RWops* file = SDL_RWFromFile(path.data(), "rb");
  if (file == nullptr) {
    throw std::runtime_error(std::string("can't open audio file: ") + path.data());
  }

  // freq, format, channels, and samples - used by SDL_LoadWAV_RW
  SDL_AudioSpec file_audio_spec;

  if (nullptr == SDL_LoadWAV_RW(file, 1, &file_audio_spec, &buffer, &length)) {
    throw std::runtime_error(std::string("can't load wav: ") + path.data());
  }

  std::cout << "--------------------------------------------\n";
  std::cout << "audio format for: " << path << '\n'
            << "format: " << get_sound_format_name(file_audio_spec.format) << '\n'
            << "sample_size: " << get_sound_format_size(file_audio_spec.format) << '\n'
            << "channels: " << static_cast<uint32_t>(file_audio_spec.channels) << '\n'
            << "frequency: " << file_audio_spec.freq << '\n'
            << "length: " << length << '\n'
            << "time: "
            << static_cast<double>(length) /
                 (file_audio_spec.channels * static_cast<uint32_t>(file_audio_spec.freq) *
                  get_sound_format_size(file_audio_spec.format))
            << "sec" << std::endl;
  std::cout << "--------------------------------------------\n";

  if (file_audio_spec.channels != device_audio_spec.channels ||
      file_audio_spec.format != device_audio_spec.format ||
      file_audio_spec.freq != device_audio_spec.freq) {
    Uint8* output_bytes;
    int output_length;

    int convert_status = SDL_ConvertAudioSamples(file_audio_spec.format,
                                                 file_audio_spec.channels,
                                                 file_audio_spec.freq,
                                                 buffer,
                                                 static_cast<int>(length),
                                                 device_audio_spec.format,
                                                 device_audio_spec.channels,
                                                 device_audio_spec.freq,
                                                 &output_bytes,
                                                 &output_length);
    if (0 != convert_status) {
      std::stringstream message;
      message << "failed to convert WAV byte stream: " << SDL_GetError();
      throw std::runtime_error(message.str());
    }

    SDL_free(buffer);
    buffer = output_bytes;
    length = static_cast<uint32_t>(output_length);
  } else {
    // no need to convert buffer, use as is
  }
}

sound::~sound() {
  if (!tmp_buf) {
    SDL_free(buffer);
  }
  buffer = nullptr;
  length = 0;
}

isound* engine::create_sound(std::string_view path) {
  sound* s = new sound(path, audio_device, audio_device_spec);
  {
    // push_backsound_buffer_impl
    std::lock_guard<std::mutex> lock(audio_mutex);
    sounds.push_back(s);
  }
  return s;
}

std::mutex engine::audio_mutex;

void engine::audio_callback(void* engine_ptr, uint8_t* stream, int stream_size) {
  std::lock_guard<std::mutex> lock(audio_mutex);
  // no sound default
  std::fill_n(stream, stream_size, '\0');

  engine* e = static_cast<engine*>(engine_ptr);

  for (sound* snd : e->sounds) {
    if (snd->is_playing) {
      uint32_t rest = snd->length - snd->current_index;
      uint8_t* current_buff = &snd->buffer[snd->current_index];

      if (rest <= static_cast<uint32_t>(stream_size)) {
        // copy rest to buffer
        SDL_MixAudioFormat(
          stream, current_buff, e->audio_device_spec.format, rest, SDL_MIX_MAXVOLUME);
        snd->current_index += rest;
      } else {
        SDL_MixAudioFormat(stream,
                           current_buff,
                           e->audio_device_spec.format,
                           static_cast<uint32_t>(stream_size),
                           SDL_MIX_MAXVOLUME);
        snd->current_index += static_cast<uint32_t>(stream_size);
      }

      if (snd->current_index == snd->length) {
        if (snd->is_looped) {
          // start from begining
          snd->current_index = 0;
        } else {
          snd->is_playing = false;
        }
      }
    }
  }
}

} // end namespace grp
