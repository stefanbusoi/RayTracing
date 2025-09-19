cmake_minimum_required(VERSION 3.16)
project(RayTracing)
set(CMAKE_GENERATOR_PLATFORM x64)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# -----------------------------
# Project sources
# -----------------------------
file(GLOB_RECURSE RAYTRACING_SRC CONFIGURE_DEPENDS
        "RayTracing/src/*.cpp"
        "RayTracing/src/*.h"
)

add_executable(RayTracing ${RAYTRACING_SRC})
target_include_directories(RayTracing PRIVATE RayTracing/src)

include(FetchContent)

# -----------------------------
# GLM (header-only)
# -----------------------------
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e
)
FetchContent_MakeAvailable(glm)
target_link_libraries(RayTracing PRIVATE glm::glm)

# -----------------------------
# GLFW
# -----------------------------
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)
target_link_libraries(RayTracing PRIVATE glfw)

# -----------------------------
# Vulkan
# -----------------------------
find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Found Vulkan: ${Vulkan_LIBRARY}")
else()
    message(FATAL_ERROR "Vulkan not found! Make sure VULKAN_SDK is set.")
endif()
# -----------------------------
# ImGui (static) with Vulkan + GLFW
# -----------------------------
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    set(IMGUI_DIR ${imgui_SOURCE_DIR})
    set(IMGUI_SOURCES
            ${IMGUI_DIR}/imgui.cpp
            ${IMGUI_DIR}/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp
            ${IMGUI_DIR}/imgui_tables.cpp
            ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
            ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp   # 🔑 Vulkan backend
    )

    add_library(imgui STATIC ${IMGUI_SOURCES})
    target_include_directories(imgui PUBLIC
            ${IMGUI_DIR}
            ${IMGUI_DIR}/backends
    )
    target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)
endif()

# -----------------------------
# Link everything to RayTracing
# -----------------------------
target_link_libraries(RayTracing PRIVATE imgui glm::glm glfw Vulkan::Vulkan)
