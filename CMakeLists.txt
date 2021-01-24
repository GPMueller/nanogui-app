cmake_minimum_required(VERSION 3.15)
cmake_policy( SET CMP0025 NEW )
cmake_policy( SET CMP0077 NEW )
cmake_policy( SET CMP0091 NEW )


option( BUNDLE_APP "Create a .app bundle on macOS" ON )


#--------------------------------------------------
project( nanogui-app )
set( EXECUTABLE_NAME app )
message( STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}" )
message( STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})" )
#--------------------------------------------------


#--------------------------------------------------
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" )
#----- Set a default install directory in case none is passed
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR NOT CMAKE_INSTALL_PREFIX )
    set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
    message( STATUS ">> No installation directory given. Using: '${CMAKE_INSTALL_PREFIX}'" )
else()
    message( STATUS ">> Installation directory given: '${CMAKE_INSTALL_PREFIX}'" )
endif()
#--------------------------------------------------


#--------------------------------------------------
#----- Meta information about the project
set( META_PROJECT_NAME         "NanoGUI App" )
set( META_PROJECT_DESCRIPTION  "" )
set( META_AUTHOR_ORGANIZATION  "" )
set( META_AUTHOR_DOMAIN        "" )
set( META_AUTHOR_MAINTAINER    "Gideon Mueller" )
set( META_AUTHOR_EMAIL         "gpmueller@outlook.com" )
#----- Compiler
set( META_COMPILER             "${CMAKE_CXX_COMPILER_ID}" )
set( META_COMPILER_VERSION     "${CMAKE_CXX_COMPILER_VERSION}" )
set( META_COMPILER_FULL        "${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})" )
#----- Version info
include( Version )
git_get_latest_tag(
    META_GIT_TAG
    META_GIT_BRANCH
    META_GIT_REVISION
    META_GIT_DIFF
    META_VERSION_MAJOR
    META_VERSION_MINOR
    META_VERSION_PATCH )
set( META_VERSION              "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}" )
set( META_VERSION_REVISION     "${META_GIT_REVISION}" )
#--------------------------------------------------


#--------------------------------------------------
#----- RPATH setup
if( APPLE AND PLANNER_BUNDLE_APP )
    # Use the full RPATH of the build tree
    set( CMAKE_SKIP_BUILD_RPATH FALSE )

    # When building, don't use the install RPATH, it will still be used
    # later on in the install phase
    set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )

    # Add the automaticaly determined parts of the RPATH which point to
    # directories outside of the build tree to the install RPATH
    set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
endif()
#--------------------------------------------------


#--------------------------------------------------
set( CMAKE_DISABLE_SOURCE_CHANGES  ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
#--------------------------------------------------


#--------------------------------------------------
set( APP_TARGET_WEB OFF )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" )
    set( APP_TARGET_WEB ON  )
    set( BUNDLE_APP     OFF )
    message( STATUS ">> Building web-GUI" )
endif()
#--------------------------------------------------


#--------------------------------------------------
#----- fmtlib
add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/thirdparty/fmt )
#--------------------------------------------------


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


#--------------------------------------------------
#----- Bundling on macOS and Windows
set( OS_BUNDLE_NAME )
if( BUNDLE_APP )
    if( APPLE )
        set( OS_BUNDLE_NAME MACOSX_BUNDLE )
    elseif( WIN32 )
        set( OS_BUNDLE_NAME WIN32 )
        #----- Avoiding terminal window on Windows
        set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup" )
    endif()
endif()
#--------------------------------------------------


#--------------------------------------------------
#----- main target
add_executable( ${EXECUTABLE_NAME} ${OS_BUNDLE_NAME}
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


#--------------------------------------------------
#----- Installing executable
# The bundle is at the root of the install tree,
# otherwiese it'll go into the bin directory.
if( APPLE )
    install( TARGETS ${EXECUTABLE_NAME}
        BUNDLE  DESTINATION .   COMPONENT bundle
        RUNTIME DESTINATION bin COMPONENT runtime )
else()
    # install( FILES ${CMAKE_CURRENT_LIST_DIR}/resources/AppIcon.ico DESTINATION . )
    install( TARGETS ${EXECUTABLE_NAME} DESTINATION . COMPONENT bundle )
endif()

#----- Installing documentation
install( FILES ${CMAKE_CURRENT_LIST_DIR}/README.md DESTINATION . RENAME Readme.md COMPONENT docs )

#----- Installing licenses
install( FILES ${CMAKE_CURRENT_LIST_DIR}/LICENSE DESTINATION . RENAME License.txt COMPONENT licenses )

install( FILES ${CMAKE_CURRENT_LIST_DIR}/thirdparty/fmt/LICENSE.rst                  DESTINATION thirdparty-licenses RENAME fmt.rst          COMPONENT licenses )
install( FILES ${CMAKE_CURRENT_LIST_DIR}/thirdparty/nanogui/LICENSE.txt              DESTINATION thirdparty-licenses RENAME nanogui.txt      COMPONENT licenses )
install( FILES ${CMAKE_CURRENT_LIST_DIR}/thirdparty/nanogui/ext/coro/LICENSE         DESTINATION thirdparty-licenses RENAME coro.txt         COMPONENT licenses )
install( FILES ${CMAKE_CURRENT_LIST_DIR}/thirdparty/nanogui/ext/glfw/LICENSE.md      DESTINATION thirdparty-licenses RENAME glfw.md          COMPONENT licenses )
install( FILES ${CMAKE_CURRENT_LIST_DIR}/thirdparty/nanogui/ext/nanovg/LICENSE.txt   DESTINATION thirdparty-licenses RENAME nanovg.txt       COMPONENT licenses )
install( FILES ${CMAKE_CURRENT_LIST_DIR}/thirdparty/nanogui/ext/nanovg_metal/LICENSE DESTINATION thirdparty-licenses RENAME nanovg_metal.txt COMPONENT licenses )
install( FILES ${CMAKE_CURRENT_LIST_DIR}/thirdparty/nanogui/ext/pybind11/LICENSE     DESTINATION thirdparty-licenses RENAME pybind11.txt     COMPONENT licenses )
#--------------------------------------------------


#--------------------------------------------------
#----- Bundling paths
set( APPS "\${CMAKE_INSTALL_PREFIX}/bin/${EXECUTABLE_NAME}" )

if( WIN32 )
    set( APPS "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}.exe" )
endif( WIN32 )

#----- Bundling properties on macOS
if( BUNDLE_APP AND APPLE )
    set( APPS "\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}.app" )

    ### Apple bundle resource files
    # For Apple set the icns file containing icons
    # set where in the bundle to put the icns file
    # set_source_files_properties(
    #     ${CMAKE_CURRENT_LIST_DIR}/resources/AppIcon.icns
    #     ${CMAKE_CURRENT_LIST_DIR}/resources/Icon.png
    #     PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

    ### Apple bundle Info.plist file
    set_target_properties( ${EXECUTABLE_NAME} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME    "Nanogui App"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.gpmueller.NanoGuiApp"
        MACOSX_BUNDLE_ICON_FILE      "AppIcon.icns"
        MACOSX_BUNDLE_INFO_STRING    "Nanogui App"
        MACOSX_BUNDLE_COPYRIGHT      "© Gideon Müller")
endif()

#----- Bundle install
if( BUNDLE_APP AND APPLE )
    # Now the work of copying dependencies into the bundle/package
    # The quotes are escaped and variables to use at install time have their $ escaped
    # An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).

    ### Need to make the app and binary executable
    install( CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")
        " COMPONENT bundle )

    install( CODE "
        execute_process( COMMAND chmod +x \"\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}.app\" )
        execute_process( COMMAND chmod +x \"\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}.app/Contents/MacOS/${EXECUTABLE_NAME}\" )
        " COMPONENT bundle )
endif()
#--------------------------------------------------


#--------------------------------------------------
#----- Packaging
# To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake"
# on Mac OS X where CPackConfig.cmake is created by including CPack.
# And then there's ways to customize this as well
set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION . )
include( InstallRequiredSystemLibraries )

if( APPLE  )
    set( CPACK_GENERATOR "DragNDrop" )
    set( CPACK_BINARY_DRAGNDROP ON )
    set( CPACK_COMPONENTS_ALL bundle )
elseif( WIN32 )
    set( CPACK_GENERATOR "ZIP" )
    set( CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE )
    set( CPACK_COMPONENTS_ALL bundle icons licenses docs )
else()
    set( CPACK_GENERATOR "ZIP" )
    set( CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE )
    set( CPACK_COMPONENTS_ALL bundle icons licenses docs )
endif()

set( CPACK_ARCHIVE_COMPONENT_INSTALL ON )
set( CPACK_PACKAGE_VERSION_MAJOR ${META_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${META_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${META_VERSION_PATCH} )

include( CPack )
#--------------------------------------------------