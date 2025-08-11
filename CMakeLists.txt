cmake_minimum_required(VERSION 3.18)
project(turfwars C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(raylib-cpp)
include(includeable.cmake)

add_executable(turfwars src/turfwars.cpp src/skybox.cpp)
target_link_libraries(turfwars PUBLIC raylib raylib_cpp raylib::buffered)

make_includeable(assets/shaders/cubemap.fs generated/cubemap.fs)
make_includeable(assets/shaders/cubemap.vs generated/cubemap.vs)
make_includeable(assets/shaders/skybox.fs generated/skybox.fs)
make_includeable(assets/shaders/skybox.vs generated/skybox.vs)

configure_file(assets/textures/skybox.png textures/skybox.png COPYONLY)
configure_file("assets/Kenny Space Kit/rocketA.glb" meshes/rocketA.glb COPYONLY)
configure_file("assets/Kenny Car Kit/sedan.glb" meshes/sedan.glb COPYONLY)
configure_file("assets/Kenny Car Kit/delivery.glb" meshes/delivery.glb COPYONLY)
configure_file("assets/Kenny Car Kit/race.glb" meshes/race.glb COPYONLY)
configure_file("assets/Kenny Car Kit/suv.glb" meshes/suv.glb COPYONLY)
configure_file("assets/Kenny Car Kit/taxi.glb" meshes/taxi.glb COPYONLY)

configure_file("assets/Kenny Car Kit/Textures/colormap.png" meshes/Textures/colormap.png COPYONLY)