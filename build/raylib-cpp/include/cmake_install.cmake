# Install script for directory: /Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/AudioDevice.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/AudioStream.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/AutomationEventList.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/BoundingBox.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Camera2D.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Camera3D.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Color.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/FileData.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/FileText.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Font.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Functions.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Gamepad.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Image.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Keyboard.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Material.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Matrix.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Mesh.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/MeshUnmanaged.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Model.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/ModelAnimation.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Mouse.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Music.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Ray.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/RayCollision.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/RaylibException.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/raylib-cpp-utils.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/raylib-cpp.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/raylib.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/raymath.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Rectangle.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/RenderTexture.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Shader.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Sound.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Text.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Texture.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/TextureUnmanaged.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Touch.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Vector2.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Vector3.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Vector4.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/VrStereoConfig.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Wave.hpp"
    "/Users/zhaohenghe/Documents/Projects/turfwars/raylib-cpp/include/Window.hpp"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/zhaohenghe/Documents/Projects/turfwars/build/raylib-cpp/include/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
