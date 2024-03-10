#include "convert.h"

#include "uci.h"

#include "extra/nnue_data_binpack_format.h"

#include <sstream>
#include <fstream>
#include <unordered_set>
#include <iomanip>
#include <list>
#include <cmath>    // std::exp(),std::pow(),std::log()
#include <cstring>  // memcpy()
#include <memory>
#include <limits>
#include <optional>
#include <chrono>
#include <random>
#include <regex>
#include <filesystem>
#include <misc.h>
#include "settings.h"

using namespace std;
namespace sys = std::filesystem;

namespace Stockfish::Tools
{

    static inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
            }));
    }

    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    static inline void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    static inline const std::string plain_extension = ".plain";
    static inline const std::string bin_extension = ".bin";
    static inline const std::string binpack_extension = ".binpack";

    static bool file_exists(const std::string& name)
    {
        std::ifstream f(name);
        return f.good();
    }

    static bool ends_with(const std::string& lhs, const std::string& end)
    {
        if (end.size() > lhs.size()) return false;

        return std::equal(end.rbegin(), end.rend(), lhs.rbegin());
    }

    static bool is_convert_of_type(
        const std::string& input_path,
        const std::string& output_path,
        const std::string& expected_input_extension,
        const std::string& expected_output_extension)
    {
        return ends_with(input_path, expected_input_extension)
            && ends_with(output_path, expected_output_extension);
    }

    using ConvertFunctionType = void(std::string inputPath, std::string outputPath, std::ios_base::openmode om, parser_settings settings);

    static ConvertFunctionType* get_convert_function(const std::string& input_path, const std::string& output_path)
    {
        if (is_convert_of_type(input_path, output_path, binpack_extension, plain_extension))
            return binpack::convertBinpackToPlain;
        if (is_convert_of_type(input_path, output_path, binpack_extension, bin_extension))
            return binpack::convertBinpackToBin;

        return nullptr;
    }

    static void convert(const std::string& input_path, const std::string& output_path, std::ios_base::openmode om, parser_settings settings)
    {
        if(!file_exists(input_path))
        {
            std::cerr << "Input file does not exist.\n";
            return;
        }

        auto func = get_convert_function(input_path, output_path);
        if (func != nullptr)
        {
            func(input_path, output_path, om, settings);
        }
        else
        {
            std::cerr << "Conversion between files of these types is not supported.\n";
        }
    }

    static void convert(const std::vector<std::string>& args)
    {
        if (args.size() < 2 || args.size() > 5)
        {
            std::cerr << "Invalid arguments.\n";
            std::cerr << "Usage: convert from_path to_path [append] --max-score <score>\n";
            return;
        }

        bool append = false;
        parser_settings settings;
        // loop over all the tokens and parse the commands
        for (size_t i = 1; i < args.size(); i++)
        {
            if (args.at(i) == "append")
                append = true;
            else if (args.at(i) == "--max-score")
            {
                settings.filter_score = true;
                try
                {
                    settings.max_score = std::stoi(args.at(i + 1));
                    if (settings.max_score < 0)
                    {
                        std::cerr << "The score used for sign filtering is used as an absolute value, please use a positive number\n";
                        return;
                    }
                }
                catch (...)
                {
                    std::cerr << "Invalid number for score filtering\n";
                    return;
                }
            }
        }

        const std::ios_base::openmode openmode =
            append
                ? std::ios_base::app
                : std::ios_base::trunc;

        convert(args[0], args[1], openmode, settings);
    }

    void convert(istringstream& is)
    {
        std::vector<std::string> args;

        while (true)
        {
            std::string token = "";
            is >> token;
            if (token == "")
                break;

            args.push_back(token);
        }

        convert(args);
    }
}
    
