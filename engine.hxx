#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <array>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

#include "imgui-src/imgui.h"
#include "imgui-src/imgui_impl_opengl3.h"
#include "imgui-src/imgui_impl_sdl3.h"
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

class iengine;
class igame;

iengine* create_engine();
void destroy_engine(iengine* e);

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

class sprite {
  public:
    sprite() = default;
    sprite(float x1, float y1, float x2, float y2);
    bool collision(std::array<float, 4> collision_entity);
    glm::mediump_mat3 result_matrix;
    grp::triangle triangle_low;
    grp::triangle triangle_high;
    std::array<float, 4> AABB;
    grp::triangle triangle_low_transformed;
    grp::triangle triangle_high_transformed;
};

class isound {
  public:
    enum class properties { once, looped };

    virtual ~isound();
    virtual void play(const properties) = 0;
};

class iengine {
  public:
    virtual ~iengine();

    virtual std::string initialize(std::string_view config) = 0;
    virtual bool input_event(event& e, bool* state_key) = 0;
    virtual void render(const triangle&) = 0;
    virtual void render(const sprite&, texture* const texture) = 0;
    virtual void render(const triangle&, texture* const texture) = 0;
    virtual texture* create_texture(std::string_view path) = 0;

    virtual isound* create_sound(std::string_view path) = 0;
    virtual void destroy_sound(isound*) = 0;

    virtual void set_cursor_visible(bool visible) = 0;
    virtual void draw_imgui() = 0;

    virtual void swap_buffers() = 0;
    virtual void uninitialize() = 0;
    virtual void set_game(igame* g) = 0;

  protected:
    igame* game;
};

class igame {
  public:
    igame() = default;
    virtual ~igame();

    igame(iengine& e);
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void ImGui_menu() = 0;
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
void get_transformed_triangle(sprite& sprite);

} // end namespace grp
