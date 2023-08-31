#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>
namespace fs = std::filesystem;

enum OperandType { ACCUMULATOR, REGISTER, LITERAL };
enum CommandType { NOP, LDA, MV, ADD, SUB, INC, DEC, JMP, JFZ, HLT };

struct Operand {
    OperandType type;
    unsigned char value;
    Operand(std::string string) {
        if (string[0] == 'A')
            type = ACCUMULATOR;
        else if (string[0] == 'R') {
            type = REGISTER;
            int literal = std::stoi(string.data() + 1, 0, 16);
            value = std::clamp(literal, 0, 255);
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
    CommandType command;
    std::vector<Operand> operands;
};

static std::string typeMap(OperandType type) {
    if (type == ACCUMULATOR)
        return "accumulator";
    if (type == REGISTER)
        return "register";
    if (type == LITERAL)
        return "literal";
    throw std::runtime_error("Unknown operand type!");
}

static CommandType map(std::string&& name) {
    if (name == "NOP")
        return NOP;
    if (name == "LDA")
        return LDA;
    if (name == "MV")
        return MV;
    if (name == "ADD")
        return ADD;
    if (name == "SUB")
        return SUB;
    if (name == "INC")
        return INC;
    if (name == "DEC")
        return DEC;
    if (name == "JMP")
        return JMP;
    if (name == "JFZ")
        return JFZ;
    if (name == "HLT")
        return HLT;
    throw std::runtime_error("Unknown command: " + name);
}

unsigned char reverse(unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

static std::vector<char> delimiters = {' ', ','};
static std::optional<Expression> parseInstruction(std::string line) {
    if (line.find("//") != line.npos)
        line = line.substr(0, line.find("//"));
    if (line.length() == 0)
        return std::nullopt;

    CommandType command;
    std::vector<Operand> operands;
    int lastPosition = 0;

    for (int i = 0; i < line.length(); i++) {
        if (std::find(delimiters.begin(), delimiters.end(), line[i]) !=
            delimiters.end()) {
            if (lastPosition == 0)
                command = map(line.substr(lastPosition, i - lastPosition));
            else if (lastPosition != i)
                operands.push_back(
                    {line.substr(lastPosition, i - lastPosition)});

            lastPosition = i + 1;
        }
    }

    if (lastPosition == 0)
        command =
            map(line.substr(lastPosition, line.length() - lastPosition + 1));
    else if (lastPosition != line.length())
        operands.push_back(
            {line.substr(lastPosition, line.length() - lastPosition + 1)});

    return {{command, operands}};
}

static void compileNOP(std::vector<Operand>& operands, std::ofstream& output) {
    if (!operands.empty())
        throw std::runtime_error("Too many arguments for NOP!");
    char code = reverse(0b00000000);
    output.write(&code, 1);
}

static void compileLDA(std::vector<Operand>& operands, std::ofstream& output) {
    if (operands.size() > 1)
        throw std::runtime_error("Too many arguments for LDA!");
    else if (operands.size() < 1)
        throw std::runtime_error("Not enough arguments for LDA!");
    char code = reverse(0b00000001);
    output.write(&code, 1);
    code = reverse(operands[0].value);
    output.write(&code, 1);
}

static void compileMV(std::vector<Operand>& operands, std::ofstream& output) {
    if (operands.size() > 2)
        throw std::runtime_error("Too many arguments for MV!");
    else if (operands.size() < 2)
        throw std::runtime_error("Not enough arguments for MV!");

    if (operands[1].type != REGISTER)
        throw std::runtime_error(
            "Second argument of MV type mismatch, present type: " +
            typeMap(operands[1].type) + ", acceptable type(s): register");

    char code = reverse(0b00000010);
    output.write(&code, 1);
    code = 0b00000000;

    if (operands[0].type == ACCUMULATOR) {
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == REGISTER) {
        code += 128;
        code += operands[0].value * 8;
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == LITERAL) {
        code += 192;
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
        code = operands[0].value;
        code = reverse(code);
        output.write(&code, 1);
    }
}

static void compileADD(std::vector<Operand>& operands, std::ofstream& output) {
    if (operands.size() > 2)
        throw std::runtime_error("Too many arguments for ADD!");
    else if (operands.size() < 2)
        throw std::runtime_error("Not enough arguments for ADD!");

    if (operands[0].type == LITERAL)
        throw std::runtime_error(
            "Second argument of ADD type mismatch, present type: " +
            typeMap(operands[1].type) + ", acceptable type(s): register");

    char code = reverse(0b00000011);
    output.write(&code, 1);
    code = 0b00000000;

    if (operands[0].type == ACCUMULATOR) {
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == REGISTER) {
        code += 128;
        code += operands[0].value * 8;
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == LITERAL) {
        code += 192;
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
        code = operands[0].value;
        code = reverse(code);
        output.write(&code, 1);
    }
}

static void compileSUB(std::vector<Operand>& operands, std::ofstream& output) {
    if (operands.size() > 2)
        throw std::runtime_error("Too many arguments for SUB!");
    else if (operands.size() < 2)
        throw std::runtime_error("Not enough arguments for SUB!");

    if (operands[0].type == LITERAL)
        throw std::runtime_error(
            "Second argument of ADD type mismatch, present type: " +
            typeMap(operands[1].type) + ", acceptable type(s): register");

    char code = reverse(0b00000100);
    output.write(&code, 1);
    code = 0b00000000;

    if (operands[0].type == ACCUMULATOR) {
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == REGISTER) {
        code += 128;
        code += operands[0].value * 8;
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == LITERAL) {
        code += 192;
        code += operands[1].value;
        code = reverse(code);
        output.write(&code, 1);
        code = operands[0].value;
        code = reverse(code);
        output.write(&code, 1);
    }
}

static void compileINC(std::vector<Operand>& operands, std::ofstream& output) {
    if (operands.size() > 0)
        throw std::runtime_error("Too many arguments for INC!");

    char code = reverse(0b10000101);
    output.write(&code, 1);
}

static void compileDEC(std::vector<Operand>& operands, std::ofstream& output) {
    if (operands.size() > 0)
        throw std::runtime_error("Too many arguments for DEC!");

    char code = reverse(0b10000110);
    output.write(&code, 1);
}

static void compileJMP(std::vector<Operand>& operands, std::ofstream& output) {
    if (operands.size() > 1)
        throw std::runtime_error("Too many arguments for JMP!");
    else if (operands.size() < 1)
        throw std::runtime_error("Not enough arguments for JMP!");

    // could literal be JMP argument?

    char code = reverse(0b00000111);
    output.write(&code, 1);
    code = 0b00000000;

    if (operands[0].type == ACCUMULATOR) {
        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == REGISTER) {
        code += 192;
        code += operands[0].value;

        code = reverse(code);
        output.write(&code, 1);
    } else if (operands[0].type == LITERAL) {
        //?????
    }
}

static void compileHLT(std::vector<Operand>& operands, std::ofstream& output) {
    if (!operands.empty())
        throw std::runtime_error("Too many arguments for NOP!");
    char code = reverse(0b11111111);
    output.write(&code, 1);
}

// static void compileJFZ(std::vector<Operand>& operands, std::ofstream& output)
// {
//     if (operands.size() > 2)
//         throw std::runtime_error("Too many arguments for JFZ!");
//     else if (operands.size() < 2)
//         throw std::runtime_error("Not enough arguments for JFZ!");

//    // could literal be first JFZ argument?

//    char code = reverse(0b00001000);
//    output.write(&code, 1);
//    code = 0b00000000;

//    if (operands[0].type == ACCUMULATOR) {
//        code += operands[0].value;
//        code = reverse(code);
//        output.write(&code, 1);
//    } else if (operands[0].type == REGISTER) {
//        code += 192;
//        code += operands[0].value;

//        code = reverse(code);
//        output.write(&code, 1);
//    } else if (operands[0].type == LITERAL) {
//        //?????
//    }
//}

static void compileStatement(Expression& expr, std::ofstream& output) {
    if (expr.command == NOP)
        compileNOP(expr.operands, output);
    else if (expr.command == LDA)
        compileLDA(expr.operands, output);
    else if (expr.command == MV)
        compileMV(expr.operands, output);
    else if (expr.command == ADD)
        compileADD(expr.operands, output);
    else if (expr.command == SUB)
        compileSUB(expr.operands, output);
    else if (expr.command == INC)
        compileINC(expr.operands, output);
    else if (expr.command == DEC)
        compileDEC(expr.operands, output);
    else if (expr.command == JMP)
        compileJMP(expr.operands, output);
    else if (expr.command == JFZ)
        return;
    else if (expr.command == HLT)
        compileHLT(expr.operands, output);
    return;
}

int main(int argc, char* argv[]) {
    if (argc > 3) {
        std::cout << "Too many arguments!\n";
        return 1;
    }
    if (argc < 2) {
        std::cout << "Not enough arguments!\n";
        return 1;
    }

    std::ifstream file;
    if (!fs::exists(argv[1]))
        throw std::runtime_error("No such file!");
    if (fs::path(argv[1]).extension() != ".z" &&
        fs::path(argv[1]).extension() != ".zasm")
        throw std::runtime_error(
            "Wrong file extension, it should be .z or .zasm!");
    file.open(argv[1]);

    std::ofstream outputFile;
    if (argc > 2)
        outputFile.open(argv[2]);
    else
        outputFile.open(fs::path{argv[1]}.parent_path().append("output.bin"));

    std::string line;
    while (getline(file, line)) {
        std::optional<Expression> expr = parseInstruction(line);
        if (expr.has_value())
            compileStatement(expr.value(), outputFile);
    }
}
