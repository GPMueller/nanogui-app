#include <gl_canvas.hpp>

#include <nanogui/screen.h>

#include <string>

class Application : public nanogui::Screen
{
public:
    Application( const std::string & title );
    ~Application();
    static void init_context();
    void run();

    virtual bool keyboard_event( int key, int scancode, int action, int modifiers );

    virtual void draw( NVGcontext * ctx );

private:
    GlCanvas * canvas;
};