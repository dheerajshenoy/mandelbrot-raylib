#pragma once

#include <thread>
namespace Raylib
{
#include <raylib.h>
}
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <vector>

class Mandelbrot
{
public:

    Mandelbrot();
    ~Mandelbrot();
    struct RenderParams
    {
        double a, b, c, d;
        int startY, endY;
        Raylib::Color *pixels;
    };
    void loop() noexcept;
    void renderChunk(const RenderParams &p) noexcept;

    template <typename T>
    static inline T map(T ina, T inb, T outa, T outb, T value) noexcept
    {
        return outa + (value - ina) * (outb - outa) / (inb - ina);
    }

private:

    int m_width, m_height, m_max_iter{ 50 };
    Raylib::Camera2D m_camera{ 0 };
    bool m_needsUpdate{ true };
    Raylib::Vector2 m_lastMousePos{ 0, 0 };
    std::vector<Raylib::Color> m_pixels{};
    Raylib::Texture2D m_texture;
};
