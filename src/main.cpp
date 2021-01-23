#include <application.hpp>

#include <iostream>

#if defined( _WIN32 )
#if defined( APIENTRY )
#undef APIENTRY
#endif
#include <windows.h>
#endif

int main()
try
{
    Application::init_context();
    nanogui::ref<Application> app = new Application( "NanoGUI App" );
    app->run();
    return 0;
}
catch( const std::runtime_error & e )
{
    std::string error_msg = std::string( "Caught a fatal error: " ) + std::string( e.what() );
#if defined( _WIN32 )
    MessageBoxA( nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK );
#else
    std::cerr << error_msg << std::endl;
#endif
    return -1;
}