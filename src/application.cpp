#include <application.hpp>

#include <GLFW/glfw3.h>

#include <nanogui/button.h>
#include <nanogui/layout.h>
#include <nanogui/window.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

EM_JS( void, resize_canvas, (), { jsResizeCanvas(); } );
EM_JS( int, canvas_get_width, (), { return Module.canvas.width; } );
EM_JS( int, canvas_get_height, (), { return Module.canvas.height; } );
#endif

constexpr float Pi = 3.14159f;

void Application::init_context()
{
    nanogui::init();
}

Application::Application( const std::string & title ) : nanogui::Screen( nanogui::Vector2i( 800, 600 ), title.c_str() )
{
    using nanogui::Vector2i;
    using nanogui::Vector4i;

    canvas = new GlCanvas( this );
    canvas->set_background_color( { 100, 100, 100, 255 } );
    canvas->set_fixed_size( { 400, 400 } );
    canvas->set_size( this->size() );


    nanogui::Window * window = new nanogui::Window( this, "Canvas widget demo" );
    window->set_position( Vector2i( 15, 15 ) );
    window->set_layout( new nanogui::GroupLayout() );

    nanogui::Widget * tools = new nanogui::Widget( window );
    tools->set_layout( new nanogui::BoxLayout( nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 5 ) );

    nanogui::Button * b0 = new nanogui::Button( tools, "Random Background" );
    b0->set_callback(
        [this]() { canvas->set_background_color( Vector4i( rand() % 256, rand() % 256, rand() % 256, 255 ) ); } );

    nanogui::Button * b1 = new nanogui::Button( tools, "Random Rotation" );
    b1->set_callback( [this]() { canvas->set_rotation( (float)Pi * rand() / (float)RAND_MAX ); } );

    perform_layout();
}

Application::~Application()
{
    nanogui::shutdown();
}

void Application::run()
{
    this->draw_all();
    this->set_visible( true );
    nanogui::mainloop( 1 / 60.f * 1000 );
}

bool Application::keyboard_event( int key, int scancode, int action, int modifiers )
{
    if( Screen::keyboard_event( key, scancode, action, modifiers ) )
        return true;
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        set_visible( false );
        return true;
    }
    return false;
}

void Application::draw( NVGcontext * ctx )
{
#ifdef __EMSCRIPTEN__
    resize_canvas();
    nanogui::Vector2i size;
    size[0] = canvas_get_width();
    size[1] = canvas_get_height();
    Screen::set_size( size );
    this->canvas->set_size( size );
#endif
    Screen::draw( ctx );
}