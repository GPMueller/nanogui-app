cmake_minimum_required(VERSION 3.15)
cmake_policy( SET CMP0025 NEW )
cmake_policy( SET CMP0077 NEW )
cmake_policy( SET CMP0091 NEW )


#--------------------------------------------------
project( nanogui-app )
set( EXECUTABLE_NAME app )
message( STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}" )
message( STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})" )
#--------------------------------------------------


#--------------------------------------------------
# Have the binary placed into the source head
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR} )
# Output paths for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
#--------------------------------------------------
set( CMAKE_DISABLE_SOURCE_CHANGES  ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
#--------------------------------------------------


#--------------------------------------------------
set( APP_TARGET_WEB OFF )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" )
    set( APP_TARGET_WEB   ON  )
    message( STATUS ">> Building web-GUI" )
endif()
#--------------------------------------------------


#--------------------------------------------------
#----- fmtlib

add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/thirdparty/fmt )


#--------------------------------------------------
#----- nanogui

set( NANOGUI_BUILD_EXAMPLES OFF )
set( NANOGUI_BUILD_SHARED   OFF )
set( NANOGUI_BUILD_PYTHON   OFF )
# set( NANOGUI_BUILD_GLAD     ON  ) # default is fine
# set( NANOGUI_BUILD_GLFW     ON  ) # default is fine
set( NANOGUI_INSTALL        OFF )
add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/thirdparty/nanogui )


#--------------------------------------------------
#----- main target
add_executable( ${EXECUTABLE_NAME}
    src/main.cpp
    src/application.cpp
    src/gl_canvas.cpp )

target_include_directories( ${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include )

target_link_libraries( ${EXECUTABLE_NAME} PRIVATE nanogui fmt )

# target_compile_definitions( ${EXECUTABLE_NAME} PRIVATE FMT_HEADER_ONLY )

set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_EXTENSIONS OFF)

if( APP_TARGET_WEB )
    ### Set specific flags for emcc and to place the result under webapp/js/
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "-O3 -g -s WASM=1 -s USE_GLFW=3 -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1"
        LINK_FLAGS    "-O3 -g -s WASM=1 -s USE_GLFW=3 -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/webapp/js
        # SUFFIX ".html"
    )

endif()
#--------------------------------------------------