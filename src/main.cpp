#include <complex>
#include <raylib.h>
#include <thread>
#include <vector>

#define WIDTH    800
#define HEIGHT   600
#define MAX_ITER 500

static float
map(float ina, float inb, float outa, float outb, float value) noexcept
{
    return outa + (value - ina) * (outb - outa) / (inb - ina);
}

struct RenderParams
{
    float a, b, c, d;
    int startY, endY;
    Color *pixels;
};

void
renderChunk(RenderParams p)
{
    for (int py = p.startY; py < p.endY; ++py)
    {
        for (int px = 0; px < WIDTH; ++px)
        {
            float x0 = map(0, WIDTH, p.a, p.b, px);
            float y0 = map(0, HEIGHT, p.c, p.d, py);

            std::complex<double> z(0.0, 0.0);
            std::complex<double> c(x0, y0);

            int i = 0;
            while (i < MAX_ITER && std::norm(z) <= 4.0)
            {
                z = z * z + c;
                ++i;
            }

            Color color;
            if (i == MAX_ITER)
                color = BLACK;
            else
                color = ColorFromHSV(map(0, MAX_ITER, 0, 720, i), 1.0, 1.0);

            p.pixels[py * WIDTH + px] = color;
        }
    }
}

int
main()
{
    InitWindow(WIDTH, HEIGHT, "Mandelbrot Explorer");
    SetTargetFPS(60);

    Camera2D camera = { { 0, 0 }, { WIDTH / 2.0f, HEIGHT / 2.0f }, 0, 1.0f };
    std::vector<Color> pixels(WIDTH * HEIGHT);
    Image img     = GenImageColor(WIDTH, HEIGHT, RAYWHITE);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    bool needsUpdate = true;

    while (!WindowShouldClose())
    {
        // Camera movement
        float moveSpeed = 0.5f / camera.zoom;
        if (IsKeyDown(KEY_W)) camera.target.y -= moveSpeed;
        if (IsKeyDown(KEY_S)) camera.target.y += moveSpeed;
        if (IsKeyDown(KEY_A)) camera.target.x -= moveSpeed;
        if (IsKeyDown(KEY_D)) camera.target.x += moveSpeed;

        // Zoom
        float wheel = GetMouseWheelMove();
        if (wheel != 0) camera.zoom *= (1 + wheel * 0.1f);

        if (IsKeyPressed(KEY_R))
        { // reset
            camera.target = { 0, 0 };
            camera.zoom   = 1.0f;
        }

        if (wheel != 0 || IsKeyDown(KEY_W) || IsKeyDown(KEY_A) ||
            IsKeyDown(KEY_S) || IsKeyDown(KEY_D))
            needsUpdate = true;

        if (needsUpdate)
        {
            float w = WIDTH / camera.zoom;
            float h = HEIGHT / camera.zoom;
            float left =
                camera.target.x - w / 2 + camera.offset.x / camera.zoom;
            float top = camera.target.y - h / 2 + camera.offset.y / camera.zoom;

            // Multi-thread render
            int threads = std::thread::hardware_concurrency();
            std::vector<std::thread> workers;
            int chunk = HEIGHT / threads;
            for (int t = 0; t < threads; ++t)
            {
                RenderParams params = {
                    left,         left + w,
                    top,          top + h,
                    t * chunk,    (t == threads - 1) ? HEIGHT : (t + 1) * chunk,
                    pixels.data()
                };
                workers.emplace_back(renderChunk, params);
            }
            for (auto &th : workers)
                th.join();

            UpdateTexture(tex, pixels.data());
            needsUpdate = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
