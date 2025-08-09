#pragma once

#include "argparse.hpp"

#include <thread>
#include <vector>

namespace Raylib
{
#include <raylib.h>
}

class Mandelbrot
{
public:

    enum class MBColor
    {
        BW = 0,
        WB,
        COLOR
    };

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
    void init_args(const argparse::ArgumentParser &args) noexcept;

    template <typename T>
    static inline T map(T ina, T inb, T outa, T outb, T value) noexcept
    {
        return outa + (value - ina) * (outb - outa) / (inb - ina);
    }

    void nextColor() noexcept;
    inline void zoomReset() noexcept
    {
        m_camera.zoom   = m_default_zoom;
        m_camera.target = m_default_target;
        m_needsUpdate   = true;
    }

private:

    Raylib::Vector2 m_default_target;
    float m_default_zoom;
    int m_width, m_height, m_max_iter{ 50 };
    Raylib::Camera2D m_camera{ 0 };
    bool m_needsUpdate{ true };
    bool m_hud_shown{ true };
    bool m_resize_aware{ true };
    float m_zoom{ 1.0f };
    Raylib::Vector2 m_lastMousePos{ 0, 0 };
    std::vector<Raylib::Color> m_pixels{};
    Raylib::Texture2D m_texture;
    MBColor m_color{ MBColor::BW };
};
