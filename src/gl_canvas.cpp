#include <gl_canvas.hpp>

#include <GLFW/glfw3.h>

#include <nanogui/button.h>
#include <nanogui/canvas.h>
#include <nanogui/layout.h>
#include <nanogui/renderpass.h>
#include <nanogui/screen.h>
#include <nanogui/shader.h>
#include <nanogui/window.h>

#if defined( _WIN32 )
#if defined( APIENTRY )
#undef APIENTRY
#endif
#include <windows.h>
#endif

constexpr float Pi = 3.14159f;

GlCanvas::GlCanvas( nanogui::Widget * parent ) : nanogui::Canvas( parent, 1 ), m_rotation( 0.f )
{
    m_shader = new nanogui::Shader(
        render_pass(),

        // An identifying name
        "a_simple_shader",

#if defined( NANOGUI_USE_OPENGL )
        // Vertex shader
        R"(#version 330
            uniform mat4 mvp;
            in vec3 position;
            in vec3 color;
            out vec4 frag_color;
            void main() {
                frag_color = vec4(color, 1.0);
                gl_Position = mvp * vec4(position, 1.0);
            })",

        // Fragment shader
        R"(#version 330
            out vec4 color;
            in vec4 frag_color;
            void main() {
                color = frag_color;
            })"
#elif defined( NANOGUI_USE_GLES )
        // Vertex shader
        R"(precision highp float;
            uniform mat4 mvp;
            attribute vec3 position;
            attribute vec3 color;
            varying vec4 frag_color;
            void main() {
                frag_color = vec4(color, 1.0);
                gl_Position = mvp * vec4(position, 1.0);
            })",

        // Fragment shader
        R"(precision highp float;
            varying vec4 frag_color;
            void main() {
                gl_FragColor = frag_color;
            })"
#elif defined( NANOGUI_USE_METAL )
        // Vertex shader
        R"(using namespace metal;

            struct VertexOut {
                float4 position [[position]];
                float4 color;
            };

            vertex VertexOut vertex_main(const device packed_float3 *position,
                                         const device packed_float3 *color,
                                         constant float4x4 &mvp,
                                         uint id [[vertex_id]]) {
                VertexOut vert;
                vert.position = mvp * float4(position[id], 1.f);
                vert.color = float4(color[id], 1.f);
                return vert;
            })",

        /* Fragment shader */
        R"(using namespace metal;

            struct VertexOut {
                float4 position [[position]];
                float4 color;
            };

            fragment float4 fragment_main(VertexOut vert [[stage_in]]) {
                return vert.color;
            })"
#endif
    );

    uint32_t indices[3 * 12] = { 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4, 4, 0, 3, 3, 7, 4,
                                 1, 5, 6, 6, 2, 1, 0, 1, 2, 2, 3, 0, 7, 6, 5, 5, 4, 7 };

    float positions[3 * 8] = { -1.f, 1.f, 1.f,  -1.f, -1.f, 1.f,  1.f, -1.f, 1.f,  1.f, 1.f, 1.f,
                               -1.f, 1.f, -1.f, -1.f, -1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, -1.f };

    float colors[3 * 8] = { 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0 };

    m_shader->set_buffer( "indices", nanogui::VariableType::UInt32, { 3 * 12 }, indices );
    m_shader->set_buffer( "position", nanogui::VariableType::Float32, { 8, 3 }, positions );
    m_shader->set_buffer( "color", nanogui::VariableType::Float32, { 8, 3 }, colors );
}

void GlCanvas::set_rotation( float rotation )
{
    m_rotation = rotation;
}

void GlCanvas::draw_contents()
{
    using nanogui::Matrix4f;
    using nanogui::Vector3f;

    Matrix4f view = Matrix4f::look_at( Vector3f( 0, -2, -10 ), Vector3f( 0, 0, 0 ), Vector3f( 0, 1, 0 ) );

    Matrix4f model = Matrix4f::rotate( Vector3f( 0, 1, 0 ), (float)glfwGetTime() );

    Matrix4f model2 = Matrix4f::rotate( Vector3f( 1, 0, 0 ), m_rotation );

    Matrix4f proj = Matrix4f::perspective( float( 25 * Pi / 180 ), 0.1f, 20.f, m_size.x() / (float)m_size.y() );

    Matrix4f mvp = proj * view * model * model2;

    m_shader->set_uniform( "mvp", mvp );

    // Draw 12 triangles starting at index 0
    m_shader->begin();
    m_shader->draw_array( nanogui::Shader::PrimitiveType::Triangle, 0, 12 * 3, true );
    m_shader->end();
}