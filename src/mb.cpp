#include "mb.hpp"

Mandelbrot::Mandelbrot()
{

    using namespace Raylib;
    InitWindow(0, 0, "Mandelbrot Explorer");
    SetTargetFPS(60);

    m_width  = GetScreenWidth();
    m_height = GetScreenHeight();

    m_camera.target = { 0, 0 };
    m_camera.zoom   = 1.0f;

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

        // Zoom
        const float wheel = GetMouseWheelMove();
        if (wheel != 0) m_camera.zoom *= (1 + wheel * 0.1f);

        if (IsKeyPressed(KEY_R))
        { // reset
            m_camera.target = { 0, 0 };
            m_camera.zoom   = 1.0f;
        }

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
            if (i == m_max_iter)
                color = Raylib::BLACK;
            else
                color = Raylib::RAYWHITE;
            // color = Raylib::ColorFromHSV(map(0, m_max_iter, 0, 720, i), 1.0,
            // 1.0);

            p.pixels[py * m_width + px] = color;
        }
    }
}
