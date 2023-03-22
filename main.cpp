#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>

#include <consumer.hpp>
#include <producer.hpp>

int main(int argc, const char *argv[])
{
    std::string in_filepath{};
    std::string out_filepath{};
    std::string type{};

    try
    {
        namespace po = boost::program_options;

        po::options_description desc("Allowed options");

        // clang-format off
        desc.add_options()
            ("help", "produce help message")
            ("in", po::value<std::string>(&in_filepath), "the file to read")
            ("out", po::value<std::string>(&out_filepath), "the file to write")
            ("type", po::value<std::string>(&type), "the type of process [consumer | producer]");
        // clang-format on

        po::positional_options_description pod;
        pod.add("in", 1);
        pod.add("out", 2);
        pod.add("type", 3);

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
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if (in_filepath.empty())
    {
        std::cerr << "There is no file to read!\n";
        return EXIT_FAILURE;
    }
    else if (out_filepath.empty())
    {
        std::cerr << "There is no file to write!\n";
        return EXIT_FAILURE;
    }
    else if (type.empty())
    {
        std::cerr << "There is no type of process!\n";
        return EXIT_FAILURE;
    }

    if (std::filesystem::exists(in_filepath) == false)
    {
        std::cerr << "This read file does not exist!\n";
        return EXIT_FAILURE;
    }

    if (type == "producer")
    {
        try
        {
            inter::producer Instance(in_filepath);
            Instance.run();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception [producer]: " << e.what() << "!\n";
        }
    }
    else if (type == "consumer")
    {
        try
        {
            inter::consumer Instance(out_filepath);
            Instance.run();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception [consumer]: " << e.what() << "!\n";
        }
    }
    else
    {
        std::cerr << "Unknown type: " << type << "!\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
