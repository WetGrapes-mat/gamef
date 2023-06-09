#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <array>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace grp {
/// dendy gamepad emulation events
enum class event {
  a_pressed,
  a_released,
  d_pressed,
  d_released,
  w_pressed,
  w_released,
  s_pressed,
  s_released,
  space_pressed,
  space_released,
  escape_pressed,
  escape_released,
  turn_off
};
const int num_key = 6;
extern bool keyStates[];

std::ostream& operator<<(std::ostream& stream, const event e);

class engine;

/// return not null on success
engine* create_engine();
void destroy_engine(engine* e);

struct vertex {
    float x {};
    float y {};
    float z {};
    float r {};
    float g {};
    float b {};
    float tx {};
    float ty {};
};

struct triangle {
    std::array<vertex, 3> vertices;
};

std::istream& operator>>(std::istream& is, vertex&);
std::istream& operator>>(std::istream& is, triangle&);

class texture {
  public:
    virtual ~texture() = default;
    virtual void load(const std::string_view path) = 0;
    virtual void bind() = 0;
};

class engine {
  public:
    virtual ~engine();

    virtual std::string initialize(std::string_view config) = 0;
    virtual bool input_event(event& e, bool* state_key) = 0;
    virtual void render(const triangle&) = 0;
    virtual void render(const triangle&, texture* const texture) = 0;
    virtual texture* create_texture(std::string_view path) = 0;

    virtual void swap_buffers() = 0;
    virtual void uninitialize() = 0;
};

class opengl_shader_program final {
  public:
    opengl_shader_program() = default;
    ~opengl_shader_program();
    opengl_shader_program(const opengl_shader_program&) = delete;
    opengl_shader_program(opengl_shader_program&&) = delete;
    opengl_shader_program& operator=(const opengl_shader_program&) = delete;
    opengl_shader_program& operator=(opengl_shader_program&&) = delete;

    void load_shader(const GLenum shader_type, const std::string_view shader_path);

    void prepare_program();

    void apply_shader_program();

  private:
    void attach_shaders();
    void link_program() const;
    void validate_program() const;

    std::string get_shader_code_from_file(const std::string_view path) const;

    std::vector<GLuint> shaders_ {};
    GLuint program_ {};
};

triangle get_transformed_triangle(const triangle& t, const glm::mediump_mat3& result_matrix);

} // end namespace grp
