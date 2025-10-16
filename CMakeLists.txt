cmake_minimum_required(VERSION 3.20)
project(PirateEditor CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# ---------------------
# SDL3
# ---------------------
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG main
)
FetchContent_MakeAvailable(SDL3)

# SDL3_image
FetchContent_Declare(
  SDL3_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG main
)
FetchContent_MakeAvailable(SDL3_image)

# SDL3_ttf
FetchContent_Declare(
  SDL3_ttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
  GIT_TAG main
)
FetchContent_MakeAvailable(SDL3_ttf)

# ---------------------
# SDL3_mixer (kun Windows og Linux)
# ---------------------
if(WIN32 OR UNIX AND NOT APPLE)
    FetchContent_Declare(
      SDL3_mixer
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
      GIT_TAG main
    )
    FetchContent_MakeAvailable(SDL3_mixer)
endif()

# ---------------------
# Kildefiler
# ---------------------
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

# ---------------------
# Eksekverbar
# ---------------------
add_executable(PirateEditor ${SRC_FILES})

# ---------------------
# Link biblioteker
# ---------------------
target_link_libraries(PirateEditor
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

# Link SDL_mixer hvis Windows eller Linux
if(WIN32 OR UNIX AND NOT APPLE)
    target_link_libraries(PirateEditor PRIVATE SDL3_mixer::SDL3_mixer)
endif()

# ---------------------
# Include directories
# ---------------------
target_include_directories(PirateEditor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
