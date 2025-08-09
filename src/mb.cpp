#include "mb.hpp"

#include <format>
#include <math.h>
#include <raylib.h>

Mandelbrot::Mandelbrot()
{
    using namespace Raylib;
    InitWindow(0, 0, "Mandelbrot Explorer");
    SetTargetFPS(60);

    m_width  = GetScreenWidth();
    m_height = GetScreenHeight();

    // m_camera.target = { 0, 0 };
    // Fit world width to screen width
    const double mandelbrotWidth  = 3.0; // from -2.0 to 1.0
    const double mandelbrotHeight = 3.0; // from -1.5 to 1.5

    double zoomX = m_width / mandelbrotWidth;
    double zoomY = m_height / mandelbrotHeight;

    m_default_zoom   = static_cast<float>(std::min(zoomX, zoomY));
    m_default_target = { -0.5f, 0.0f };

    m_camera.zoom   = m_default_zoom;
    m_camera.target = m_default_target;

    m_pixels = std::vector<Color>(m_width * m_height);

    const Image img = GenImageColor(m_width, m_height, RAYWHITE);
    m_texture       = LoadTextureFromImage(img);
    UnloadImage(img);
}

Mandelbrot::~Mandelbrot()
{
    Raylib::CloseWindow();
}

void
Mandelbrot::nextColor() noexcept
{
    using U       = std::underlying_type_t<Mandelbrot::MBColor>;
    auto count    = static_cast<U>(MBColor::COLOR) + 1;
    m_color       = static_cast<MBColor>((static_cast<U>(m_color) + 1) % count);
    m_needsUpdate = true;
}

void
Mandelbrot::loop() noexcept
{
    using namespace Raylib;
    while (!WindowShouldClose())
    {
        // Camera movement
        // Log scaling makes speed changes less aggressive
        const float baseSpeed  = 10.0f;
        const float zoomFactor = std::pow(2.0f, -std::log2(m_camera.zoom));
        const float moveSpeed  = baseSpeed * zoomFactor;

        if (IsKeyDown(KEY_W)) m_camera.target.y -= moveSpeed;
        if (IsKeyDown(KEY_S)) m_camera.target.y += moveSpeed;
        if (IsKeyDown(KEY_A)) m_camera.target.x -= moveSpeed;
        if (IsKeyDown(KEY_D)) m_camera.target.x += moveSpeed;
        if (IsKeyPressed(KEY_H)) m_hud_shown = !m_hud_shown;
        if (IsKeyPressed(KEY_C)) nextColor();
        if (IsKeyPressed(KEY_I))
        {
            m_max_iter++;
            m_needsUpdate = true;
        }
        if (IsKeyPressed(KEY_U))
        {
            m_max_iter--;
            m_needsUpdate = true;
        }

        // Zoom
        const float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            m_camera.zoom *= (1 + wheel * 0.1f);
            m_zoom += 0.1f;
        }

        if (IsKeyPressed(KEY_R)) zoomReset();

        if (wheel != 0 || IsKeyDown(KEY_W) || IsKeyDown(KEY_A) ||
            IsKeyDown(KEY_S) || IsKeyDown(KEY_D))
            m_needsUpdate = true;

        if (m_needsUpdate)
        {
            const double w = m_width / m_camera.zoom;
            const double h = m_height / m_camera.zoom;
            const double left =
                m_camera.target.x - w / 2 + m_camera.offset.x / m_camera.zoom;
            const double top =
                m_camera.target.y - h / 2 + m_camera.offset.y / m_camera.zoom;

            // Multi-thread render
            const int threads = std::thread::hardware_concurrency();
            std::vector<std::thread> workers;
            const int chunk = m_height / threads;
            for (int t = 0; t < threads; ++t)
            {
                RenderParams params = {
                    left,
                    left + w,
                    top,
                    top + h,
                    t * chunk,
                    (t == threads - 1) ? m_height : (t + 1) * chunk,
                    m_pixels.data()
                };
                workers.emplace_back(&Mandelbrot::renderChunk, this, params);
            }
            for (auto &th : workers)
                th.join();

            UpdateTexture(m_texture, m_pixels.data());
            m_needsUpdate = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(m_texture, 0, 0, WHITE);

        if (m_hud_shown)
        {
            DrawText(std::format("Zoom: {:.1f}", m_zoom).c_str(), 40, 40, 30,
                     RAYWHITE);
            DrawText("C - cycle colors", 40, 80, 30, RAYWHITE);
            DrawText("H - toggle hud", 40, 120, 30, RAYWHITE);
            DrawText(std::format("Iter: {}", m_max_iter).c_str(), 40, 160, 30,
                     RAYWHITE);
            DrawText("I - increase iter by one", 40, 200, 30, RAYWHITE);
            DrawText("U - decrease iter by one", 40, 240, 30, RAYWHITE);
        }
        EndDrawing();
    }
}

void
Mandelbrot::renderChunk(const Mandelbrot::RenderParams &p) noexcept
{

    for (int py = p.startY; py < p.endY; ++py)
    {
        for (int px = 0; px < m_width; ++px)
        {
            const double x0 = map<double>(0, m_width, p.a, p.b, px);
            const double y0 = map<double>(0, m_height, p.c, p.d, py);

            int i     = 0;
            double zx = 0.0, zy = 0.0;
            double zx2 = 0.0, zy2 = 0.0;
            while (i < m_max_iter && zx2 + zy2 <= 4.0)
            {
                zy  = 2.0 * zx * zy + y0;
                zx  = zx2 - zy2 + x0;
                zx2 = zx * zx;
                zy2 = zy * zy;
                ++i;
            }

            Raylib::Color color;
            if (m_color == MBColor::BW)
            {
                // Black inside, white outside
                color = (i == m_max_iter) ? Raylib::BLACK : Raylib::RAYWHITE;
            }
            else if (m_color == MBColor::WB)
            {
                // White inside, HSV outside
                color = (i == m_max_iter) ? Raylib::RAYWHITE : Raylib::BLACK;
            }
            else
            {
                color = (i == m_max_iter)
                            ? Raylib::BLACK
                            : Raylib::ColorFromHSV(
                                  map(0, m_max_iter, 0, 1400, i), 1.0, 1.0);
            }

            p.pixels[py * m_width + px] = color;
        }
    }
}

void
Mandelbrot::init_args(const argparse::ArgumentParser &args) noexcept
{
    if (args.is_used("--color"))
    {
        const std::string color_str = args.get<std::string>("--color");
        if (color_str == "color")
            m_color = MBColor::COLOR;
        else if (color_str == "wb")
            m_color = MBColor::WB;
        else
            m_color = MBColor::BW;
    }
    else if (args.is_used("--iter")) { m_max_iter = args.get<int>("--iter"); }
}
