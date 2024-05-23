#pragma once

// tell SDL3 we're defining our own entry point
#define SDL_MAIN_HANDLED

// tell ImGui about our custom ImConfig header
#define IMGUI_USER_CONFIG "rope_demo_imconfig.h"

// omit min and max macros from Windows headers
#define NOMINMAX

// enable experimental glm features
#define GLM_ENABLE_EXPERIMENTAL

#include <Windows.h>

#include <memory>
#include <map>
#include <atomic>

#include <glad/glad.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <glm\gtx\vector_angle.hpp>

#include <SDL3\SDL.h>
#include <SDL3\SDL_opengl.h>

#include <imgui_internal.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>

#include "shaders.h"

class Render
{
    class Layer
    {
    public:
        GLuint                      m_fbo;
        GLuint                      m_rbo;
        GLuint                      m_texture;
        std::shared_ptr<ImDrawList> m_dl;
        std::unique_ptr<ImDrawData> m_drawdata;

        Layer();

        void on_new_frame();
        void on_render();
    };
    using Layers = std::map<std::string, Layer>;

    SDL_Window*        m_window;
    SDL_GLContext      m_gl_ctx;
    ImVec2             m_screen_size;
    std::atomic_bool   m_quit;
    Shaders            m_shaders;
    Layers             m_layers;
    std::vector<float> m_fps_history;

    bool init();
    void frame();
    void render();

public:
    ImVec2 m_min;
    ImVec2 m_max;

    Render();

    void                        run();
    std::string                 get_fps_display();
    std::shared_ptr<ImDrawList> get_dl(std::string_view name);
};

inline std::shared_ptr<Render> g_render;
