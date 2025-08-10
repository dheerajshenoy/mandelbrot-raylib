#include "argparse.hpp"
#include "mb.hpp"

static void
init_args(argparse::ArgumentParser &args)
{
    args.add_argument("--color").default_value("bw").help(
        "Color mode for the Mandelbrot set. (Supported values: bw, color)");

    args.add_argument("--iter").default_value("50").help(
        "Max number of iterations carried out for each "
        "Mandelbrot pixel test.");

    args.add_argument("--hud").default_value("true").help(
        "Visibility of the HUD.");

    args.add_argument("--fps").default_value("60").help("Frames per second.");

    args.add_argument("--julia").flag().help("Switch to julia set mode");

    args.add_argument("-c")
        .help("c value for julia set")
        .nargs(2)
        .default_value(std::vector<double>{ -0.7, 0.27015 })
        .scan<'g', double>();

    args.add_argument("--no-resize")
        .flag()
        .help("On window size change, don't change anything.");
}

int
main(int argc, char *argv[])
{
    argparse::ArgumentParser program("mbray");
    init_args(program);
    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }

    Mandelbrot m;
    m.init_args(program);
    m.loop();

    return 0;
}
