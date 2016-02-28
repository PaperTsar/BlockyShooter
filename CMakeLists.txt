cmake_minimum_required(VERSION 3.3)
project(Dummy)

#sets common flags, mostly warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-enum -Wconversion -Wformat=2 -Wuninitialized -Winit-self")

#sets debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
#sets release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O3")


#copies the resources to the output directory
file(COPY res DESTINATION ${PROJECT_BINARY_DIR})

#sets the path to the project cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
#this processes the SDL cmake find moduels
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

#includes the sdl headers
include_directories(${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR})

#enumerates the sources
set(SOURCE_FILES Game/SRC/main.c Graphics/SRC/graphics_man.c  Graphics/SRC/textsprite.c Events/SRC/timer.c Utility/SRC/vector.c Graphics/HEAD/graphics_man.h Graphics/HEAD/textsprite.h Events/HEAD/timer.h Utility/HEAD/vector.h  Collision/SRC/AABB.c Collision/HEAD/AABB.h Collision/SRC/physics.c Collision/HEAD/physics.h Utility/SRC/bag.c Utility/HEAD/bag.h Game/SRC/player.c Game/HEAD/player.h Events/SRC/input.c Events/HEAD/input.h Events/SRC/Timer_man.c Events/HEAD/Timer_man.h Game/SRC/GameState.c Game/HEAD/GameState.h Game/SRC/MenuState.c Game/HEAD/MenuState.h Game/HEAD/main.h  Game/SRC/LevelSelState.c Game/HEAD/LevelSelState.h)
#adds te target executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#links SDL against our executable
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        m)
