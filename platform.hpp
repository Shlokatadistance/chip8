#pragma once
#include <glad/gl.h>
#include <cstdint>
#include <SDL.h>


class Platform
{
    friend class Imgui;

public:
    Platform(char const* title, int window_width, int window_height, int texture_width, int texture_height); // constructor
    ~Platform(); // destructor
    void Update(void const * buffer, int pitch);
    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window{};
    SDL_GLContext gl_context{};
    GLuint framebuffer_texture;
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
};
