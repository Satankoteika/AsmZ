#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <ranges>
#include <vector>
#include "CLI.hpp"
#include "Commands.hpp"
#include "Translator.hpp"
#include "Types.hpp"
namespace fs = std::filesystem;

#include <chrono>
using namespace std::chrono;

int main(int argc, char* argv[]) {
    CompilerConfig::acceptableFlags = {"--output", "--binary-size"};
    CompilerConfig::commands.impl = {&cNOP, &cLDA, &cMV1,  &cMV2, &cADD,
                                     &cSUB, &cINC, &cDEC,  &cJMP, &cJFZ,
                                     &cIN,  &cOUT, &cPUSH, &cPOP, &cHLT};

    InputInfo info(argc, argv);
    Translator tr(info);
    tr.run();

    //    if (argc > 3) {
    //        std::cout << "Too many arguments!\n";
    //        return 1;
    //    }
    //    if (argc < 2) {
    //        std::cout << "Not enough arguments!\n";
    //        return 1;
    //    }

    //    std::ifstream file;
    //    if (!fs::exists(argv[1]))
    //        throw std::runtime_error("No such file!");
    //    if (fs::path(argv[1]).extension() != ".z" &&
    //        fs::path(argv[1]).extension() != ".zasm")
    //        throw std::runtime_error(
    //            "Wrong file extension, it should be .z or .zasm!");
    //    file.open(argv[1]);

    //    std::ofstream outputFile;
    //    if (argc > 2)
    //        outputFile.open(argv[2]);
    //    else
    //        outputFile.open(fs::path{argv[1]}.parent_path().append("output.bin"));

    //    std::string line;
    //    while (getline(file, line)) {
    //        std::optional<Expression> expr = parseInstruction(line);
    //        if (expr.has_value())
    //            compileStatement(expr.value(), outputFile);
    //    }
}
