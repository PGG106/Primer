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

    int parse_game_result_from_pgn_extract(std::string result) {
        // White Win
        if (result == "\"1-0\"") {
            return 1;
        }
        // Black Win
        else if (result == "\"0-1\"") {
            return -1;
        }
        // Draw
        else {
            return 0;
        }
    }

    // 0.25 -->  0.25 * PawnValueEg
    // #-4  --> -mate_in(4)
    // #3   -->  mate_in(3)
    // -M4  --> -mate_in(4)
    // +M3  -->  mate_in(3)
    Value parse_score_from_pgn_extract(std::string eval, bool& success) {
        success = true;

        if (eval.substr(0, 1) == "#") {
            if (eval.substr(1, 1) == "-") {
                return -mate_in(stoi(eval.substr(2, eval.length() - 2)));
            }
            else {
                return mate_in(stoi(eval.substr(1, eval.length() - 1)));
            }
        }
        else if (eval.substr(0, 2) == "-M") {
            //std::cout << "eval=" << eval << std::endl;
            return -mate_in(stoi(eval.substr(2, eval.length() - 2)));
        }
        else if (eval.substr(0, 2) == "+M") {
            //std::cout << "eval=" << eval << std::endl;
            return mate_in(stoi(eval.substr(2, eval.length() - 2)));
        }
        else {
            char* endptr;
            double value = strtod(eval.c_str(), &endptr);

            if (*endptr != '\0') {
                success = false;
                return VALUE_ZERO;
            }
            else {
                return Value(value * static_cast<double>(PawnValueEg));
            }
        }
    }

    // for Debug
    //#define DEBUG_CONVERT_BIN_FROM_PGN_EXTRACT

    bool is_like_fen(std::string fen) {
        int count_space = std::count(fen.cbegin(), fen.cend(), ' ');
        int count_slash = std::count(fen.cbegin(), fen.cend(), '/');

#if defined(DEBUG_CONVERT_BIN_FROM_PGN_EXTRACT)
        //std::cout << "count_space=" << count_space << std::endl;
        //std::cout << "count_slash=" << count_slash << std::endl;
#endif

        return count_space == 5 && count_slash == 7;
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

    using ConvertFunctionType = void(std::string inputPath, std::string outputPath, std::ios_base::openmode om, bool validate);

    static ConvertFunctionType* get_convert_function(const std::string& input_path, const std::string& output_path)
    {
        if (is_convert_of_type(input_path, output_path, plain_extension, bin_extension))
            return binpack::convertPlainToBin;
        if (is_convert_of_type(input_path, output_path, plain_extension, binpack_extension))
            return binpack::convertPlainToBinpack;

        if (is_convert_of_type(input_path, output_path, bin_extension, plain_extension))
            return binpack::convertBinToPlain;
        if (is_convert_of_type(input_path, output_path, bin_extension, binpack_extension))
            return binpack::convertBinToBinpack;

        if (is_convert_of_type(input_path, output_path, binpack_extension, plain_extension))
            return binpack::convertBinpackToPlain;
        if (is_convert_of_type(input_path, output_path, binpack_extension, bin_extension))
            return binpack::convertBinpackToBin;

        return nullptr;
    }

    static void convert(const std::string& input_path, const std::string& output_path, std::ios_base::openmode om, bool validate)
    {
        if(!file_exists(input_path))
        {
            std::cerr << "Input file does not exist.\n";
            return;
        }

        auto func = get_convert_function(input_path, output_path);
        if (func != nullptr)
        {
            func(input_path, output_path, om, validate);
        }
        else
        {
            std::cerr << "Conversion between files of these types is not supported.\n";
        }
    }

    static void convert(const std::vector<std::string>& args)
    {
        if (args.size() < 2 || args.size() > 4)
        {
            std::cerr << "Invalid arguments.\n";
            std::cerr << "Usage: convert from_path to_path [append] [validate]\n";
            return;
        }

        const bool append = std::find(args.begin() + 2, args.end(), "append") != args.end();
        const bool validate = std::find(args.begin() + 2, args.end(), "validate") != args.end();

        const std::ios_base::openmode openmode =
            append
            ? std::ios_base::app
            : std::ios_base::trunc;

        convert(args[0], args[1], openmode, validate);
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

    static void append_files_from_dir(
        std::vector<std::string>& filenames,
        const std::string& base_dir,
        const std::string& target_dir)
    {
        string kif_base_dir = Path::combine(base_dir, target_dir);

        sys::path p(kif_base_dir); // Origin of enumeration
        std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
            [&](const sys::path& path) {
                if (sys::is_regular_file(path))
                    filenames.push_back(Path::combine(target_dir, path.filename().generic_string()));
            });
    }

    static void rebase_files(
        std::vector<std::string>& filenames,
        const std::string& base_dir)
    {
        for (auto& file : filenames)
        {
            file = Path::combine(base_dir, file);
        }
    }

   
}
    
