#include <nanogui/nanogui.h>

#include <fmt/format.h>

#include <iostream>

using namespace nanogui;

enum test_enum
{
    Item1 = 0,
    Item2,
    Item3
};

bool bvar          = true;
int ivar           = 12345678;
double dvar        = 3.1415926;
float fvar         = (float)dvar;
std::string strval = "A string";
test_enum enumval  = Item2;
Color colval( 0.5f, 0.5f, 0.7f, 1.f );

int main( int /* argc */, char ** /* argv */ )
{
    std::cout << "------------------ main 1" << std::endl;
    nanogui::init();
    std::cout << "------------------ main 2" << std::endl;

    /* scoped variables */
    {
        Screen * screen = new Screen( Vector2i( 500, 700 ), "NanoGUI test" );

        bool enabled       = true;
        FormHelper * gui   = new FormHelper( screen );
        ref<Window> window = gui->add_window( Vector2i( 10, 10 ), "Form helper example" );

        gui->add_group( "Basic types" );
        gui->add_variable( "bool", bvar );
        gui->add_variable( "string", strval );

        gui->add_group( "Validating fields" );
        gui->add_variable( "int", ivar )->set_spinnable( true );
        gui->add_variable( "float", fvar );
        gui->add_variable( "double", dvar )->set_spinnable( true );

        gui->add_group( "Complex types" );
        gui->add_variable( "Enumeration", enumval, enabled )->set_items( { "Item 1", "Item 2", "Item 3" } );
        gui->add_variable( "Color", colval )->set_final_callback( []( const Color & c ) {
            fmt::print( "ColorPicker Final Callback: [{}, {}, {}, {}]\n", c.r(), c.g(), c.b(), c.w() );
        } );

        gui->add_group( "Other widgets" );
        gui->add_button( "A button", []() { fmt::print( "Button pressed.\n" ); } );

        screen->set_visible( true );
        screen->perform_layout();
        window->center();

        nanogui::mainloop();
    }
    std::cerr << "------------------ main 3" << std::endl;

    nanogui::shutdown();
    std::cerr << "------------------ main 4" << std::endl;
    return 0;
}
