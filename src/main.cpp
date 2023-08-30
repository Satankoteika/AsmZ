#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
namespace fs = std::filesystem;

enum OperandType { ACCUMULATOR, REGISTER, LITERAL };
enum ComandType {
    LOAD_ACCUMULATOR,
};
struct Operand {
    OperandType type;
    unsigned char value;
    Operand(std::string string) {
        if (string[0] == 'A')
            type = ACCUMULATOR;
        else if (string[0] == 'R') {
            type = REGISTER;
            value = string[1] - '0';
        } else {
            int literal = std::stoi(string, 0, 16);
            if (literal > 0 && literal < 256)
                value = literal;
            else
                std::runtime_error("Literal is not hexadecimal 8-bit integer!");
            type = LITERAL;
        }
    }
};

struct Expression {
    std::string comand;
    std::vector<Operand> operands;
};

static std::vector<char> delimiters = {' ', ','};
static Expression parseInstruction(std::string line) {
    std::string comand;
    std::vector<Operand> operands;
    int lastPosition = 0;

    for (int i = 0; i < line.length(); i++) {
        if (std::find(delimiters.begin(), delimiters.end(), line[i]) !=
            delimiters.end()) {
            if (lastPosition == 0)
                comand = line.substr(lastPosition, i - lastPosition);
            else if (lastPosition != i)
                operands.push_back(
                    {line.substr(lastPosition, i - lastPosition)});

            lastPosition = i + 1;
        }
    }

    if (lastPosition == 0)
        comand = line.substr(lastPosition, line.length() - lastPosition + 1);
    else
        operands.push_back(
            {line.substr(lastPosition, line.length() - lastPosition + 1)});

    return {comand, operands};
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Too many arguments!\n";
        return 1;
    }

    std::ifstream file(fs::canonical(fs::path{argv[1]}));
    if (!file.is_open())
        throw std::runtime_error("No such file!");

    std::string line;
    while (getline(file, line)) {
        std::cout << parseInstruction(line).comand << " ";
        int i = 0;
        for (auto& aboba : parseInstruction(line).operands) {
            std::cout << i << ":{ " << aboba.type << " " << (int)aboba.value
                      << " } ";
            i++;
        }
        std::cout << std::endl;
    }
}
