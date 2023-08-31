#ifndef CLI_HPP
#define CLI_HPP

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "CompilerConfig.hpp"

struct InputInfo {
    InputInfo(int argc, char** argv) {
        CompilerConfig cfg{};
        if (argc < 2)
            throw std::runtime_error("Not enough arguments!");
        input = argv[1];

        for (int i = 2; i < argc; i++) {
            std::string name;
            std::string value;

            std::string arg(argv[i]);
            int index = arg.find('=');
            if (index != std::string::npos) {
                name = arg.substr(0, index);
                value = arg.substr(index + 1, arg.length() - index - 1);
            } else
                name = arg;

            if (std::find(cfg.acceptableFlags.begin(),
                          cfg.acceptableFlags.end(),
                          name) == cfg.acceptableFlags.end())
                throw std::runtime_error("Unknown flag: " + name);

            if (flags.contains(name))
                throw std::runtime_error("Duplicate flag: " + name);
            flags[name] = value;
        }
    }

    const std::string& getInputPath() { return input; }
    const std::optional<std::string> getFlag(std::string flag) {
        if (!flags.contains(flag))
            return std::nullopt;
        return {flags.at(flag)};
    }

    // private:
    std::unordered_map<std::string, std::string> flags;
    std::string input;
};

// static InputInfo parseCLI(int argc, char** argv) {
//     std::string inputFilename = argv[0];

//    //    std::ifstream file;
//    //    if (!fs::exists(argv[1]))
//    //        throw std::runtime_error("No such file!");
//    //    if (fs::path(argv[1]).extension() != ".z" &&
//    //        fs::path(argv[1]).extension() != ".zasm")
//    //        throw std::runtime_error(
//    //            "Wrong file extension, it should be .z or .zasm!");
//    //    file.open(argv[1]);

//    //    std::ofstream outputFile;
//    //    if (argc > 2)
//    //        outputFile.open(argv[2]);
//    //    else
//    // outputFile.open(fs::path{argv[1]}.parent_path().append("output.bin"));

//    //    std::string line;
//    //    while (getline(file, line)) {
//    //        std::optional<Expression> expr = parseInstruction(line);
//    //        if (expr.has_value())
//    //            compileStatement(expr.value(), outputFile);
//    //    }
// }

#endif  // CLI_HPP
