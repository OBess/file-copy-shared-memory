#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>

#include <copy_file.hpp>
#include <logger.hpp>

int main(int argc, const char *argv[])
{
    std::string in_filepath{};
    std::string out_filepath{};
    std::string name{};

    try
    {
        namespace po = boost::program_options;

        po::options_description desc("Allowed options");

        // clang-format off
        desc.add_options()
            ("help", "produce help message")
            ("in", po::value<std::string>(&in_filepath), "the file to read")
            ("out", po::value<std::string>(&out_filepath), "the file to write")
            ("name", po::value<std::string>(&name), "the name of shared memory");
        // clang-format on

        po::positional_options_description pod;
        pod.add("in", 1);
        pod.add("out", 2);
        pod.add("name", 3);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                      .options(desc)
                      .positional(pod)
                      .run(),
                  vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << '\n';
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        my::log::deflogger()->error("Params exception: {}", e.what());
        return EXIT_FAILURE;
    }

    if (in_filepath.empty())
    {
        my::log::deflogger()->error("There is no file to read!");
        return EXIT_FAILURE;
    }
    else if (out_filepath.empty())
    {
        my::log::deflogger()->error("There is no file to write!");
        return EXIT_FAILURE;
    }
    else if (name.empty())
    {
        my::log::deflogger()->error("There is no name of shared memomy!");
        return EXIT_FAILURE;
    }

    if (std::filesystem::exists(in_filepath) == false)
    {
        my::log::deflogger()->error("This read file does not exist!");
        return EXIT_FAILURE;
    }

    if (in_filepath == out_filepath)
    {
        my::log::deflogger()->error("The filepathes are same!");
        return EXIT_FAILURE;
    }

    try
    {
        inter::copy_file Instance(in_filepath, out_filepath, name);
        Instance.run();
    }
    catch (const std::exception &e)
    {
        my::log::deflogger()->error("{}!", e.what());
    }
    catch (...)
    {
        my::log::deflogger()->error("Unknown exception!");
    }

    return EXIT_SUCCESS;
}
