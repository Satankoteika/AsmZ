#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <filesystem>
#include <fstream>
#include "CLI.hpp"
#include "CompilerConfig.hpp"

class Translator {
    std::ifstream input;
    std::ofstream output;
    size_t size = 0;
    size_t targetSize = 0;

    void write(std::vector<char>& codes) {
        for (char code : codes) {
            output << std::hex << std::setfill('0') << std::setw(2)
                   << (unsigned int)(code & 0xFF) << std::endl;
        }
        size += codes.size();
    }

    void write(std::vector<char>&& codes) {
        for (int code : codes) {
            output << std::hex << std::setfill('0') << std::setw(2)
                   << (unsigned int)(code & 0xFF) << std::endl;
        }
        size += codes.size();
    }

    void createROData(std::vector<char>& values) {}

    static std::string typeMap(OperandType type) {
        if (type == ACCUMULATOR)
            return "accumulator";
        if (type == REGISTER)
            return "register";
        if (type == LITERAL)
            return "literal";
        throw std::runtime_error("Unknown operand type!");
    }

    inline static const std::vector<char> delimiters = {' ', ','};
    std::vector<std::string> tokenize(std::string line) {
        if (line.find("//") != line.npos)
            line = line.substr(0, line.find("//"));
        if (line.length() == 0)
            return {};

        std::vector<std::string> result;
        int lastPosition = 0;
        for (int i = 0; i < line.length(); i++) {
            if (std::find(delimiters.begin(), delimiters.end(), line[i]) !=
                delimiters.end()) {
                if (lastPosition != i)
                    result.push_back(
                        line.substr(lastPosition, i - lastPosition));
                lastPosition = i + 1;
            }
        }

        if (lastPosition != line.length())
            result.push_back(
                line.substr(lastPosition, line.length() - lastPosition + 1));

        return result;
    }

    std::optional<Expression> parseTokens(std::vector<std::string>&& tokens) {
        CompilerConfig cfg{};

        if (tokens.empty())
            return std::nullopt;

        const CommandDescriptor* command;
        std::vector<Operand> operands;
        command = cfg.commands.getByName(tokens[0], tokens.size() - 1);

        for (int i = 1; i < tokens.size(); i++) {
            operands.push_back({tokens[i]});
        }

        return {{command, operands}};
    }

    void compileStatement(Expression expr) {
        CompilerConfig cfg{};

        if (expr.operands.size() != expr.command->opcount)
            throw std::runtime_error(
                "Wrong number of arguments for " + expr.command->name + "! " +
                std::to_string(expr.operands.size()) + " provided, but " +
                std::to_string(expr.command->opcount) + " needed.");

        for (size_t i = 0; i < expr.command->opcount; i++) {
            if ((expr.operands[i].type &
                 expr.command->suitableOperandTypes[i]) == 0)
                throw std::runtime_error(
                    "Wrong argument " + std::to_string(i) + " type for " +
                    expr.command->name +
                    "! Provided: " + typeMap(expr.operands[i].type) + ".");
        }

        write({expr.command->code});
        write(expr.command->compile(expr.operands));
    }

    //    void compileNOP(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (!operands.empty())
    //            throw std::runtime_error("Too many arguments for NOP!");
    //        unsigned char code = 0b00000000;
    //        write(code);
    //    }

    //    void compileLDA(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 1)
    //            throw std::runtime_error("Too many arguments for LDA!");
    //        else if (operands.size() < 1)
    //            throw std::runtime_error("Not enough arguments for LDA!");
    //        unsigned char code = 0b00000001;
    //        write(code);

    //        code = operands[0].value;
    //        write(code);
    //    }

    //    void compileMV(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 2)
    //            throw std::runtime_error("Too many arguments for MV!");
    //        else if (operands.size() < 2)
    //            throw std::runtime_error("Not enough arguments for MV!");

    //        if (operands[1].type != REGISTER)
    //            throw std::runtime_error(
    //                "Second argument of MV type mismatch, present type: " +
    //                typeMap(operands[1].type) + ", acceptable type(s):
    //                register");

    //        unsigned char code = 0b00000010;
    //        write(code);

    //        code = 0b00000000;

    //        if (operands[0].type == ACCUMULATOR) {
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == REGISTER) {
    //            code += 128;
    //            code += operands[0].value * 8;
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == LITERAL) {
    //            code += 192;
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //            code = operands[0].value;
    //            code = code;
    //            write(code);
    //        }
    //    }

    //    void compileADD(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 2)
    //            throw std::runtime_error("Too many arguments for ADD!");
    //        else if (operands.size() < 2)
    //            throw std::runtime_error("Not enough arguments for ADD!");

    //        if (operands[0].type == LITERAL)
    //            throw std::runtime_error(
    //                "Second argument of ADD type mismatch, present type: " +
    //                typeMap(operands[1].type) + ", acceptable type(s):
    //                register");

    //        unsigned char code = 0b00000011;
    //        write(code);

    //        code = 0b00000000;

    //        if (operands[0].type == ACCUMULATOR) {
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == REGISTER) {
    //            code += 128;
    //            code += operands[0].value * 8;
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == LITERAL) {
    //            code += 192;
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //            code = operands[0].value;
    //            code = code;
    //            write(code);
    //        }
    //    }

    //    void compileSUB(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 2)
    //            throw std::runtime_error("Too many arguments for SUB!");
    //        else if (operands.size() < 2)
    //            throw std::runtime_error("Not enough arguments for SUB!");

    //        if (operands[0].type == LITERAL)
    //            throw std::runtime_error(
    //                "Second argument of ADD type mismatch, present type: " +
    //                typeMap(operands[1].type) + ", acceptable type(s):
    //                register");

    //        unsigned char code = 0b00000100;
    //        write(code);

    //        code = 0b00000000;

    //        if (operands[0].type == ACCUMULATOR) {
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == REGISTER) {
    //            code += 128;
    //            code += operands[0].value * 8;
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == LITERAL) {
    //            code += 192;
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //            code = operands[0].value;
    //            code = code;
    //            write(code);
    //        }
    //    }

    //    void compileINC(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 0)
    //            throw std::runtime_error("Too many arguments for INC!");

    //        unsigned char code = 0b10000101;
    //        write(code);
    //    }

    //    void compileDEC(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 0)
    //            throw std::runtime_error("Too many arguments for DEC!");

    //        unsigned char code = 0b10000110;
    //        write(code);
    //    }

    //    void compileJMP(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 1)
    //            throw std::runtime_error("Too many arguments for JMP!");
    //        else if (operands.size() < 1)
    //            throw std::runtime_error("Not enough arguments for JMP!");

    //        if (operands[0].type == LITERAL)
    //            throw std::runtime_error(
    //                "JMP argument type mismatch, literals are not
    //                supported!");

    //        unsigned char code = 0b00000111;
    //        write(code);

    //        code = 0b00000000;

    //        if (operands[0].type == ACCUMULATOR) {
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == REGISTER) {
    //            code += 192;
    //            code += operands[0].value;
    //            code = code;
    //            write(code);
    //        }
    //    }

    //    void compileHLT(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (!operands.empty())
    //            throw std::runtime_error("Too many arguments for NOP!");
    //        unsigned char code = 0b11111111;
    //        write(code);
    //    }

    //    void compileJFZ(std::vector<Operand>& operands, std::ofstream& output)
    //    {
    //        if (operands.size() > 2)
    //            throw std::runtime_error("Too many arguments for JFZ!");
    //        else if (operands.size() < 2)
    //            throw std::runtime_error("Not enough arguments for JFZ!");

    //        if (operands[0].type == LITERAL)
    //            throw std::runtime_error(
    //                "First JFZ argument type mismatch, literals are not "
    //                "supported!");
    //        if (operands[1].type != REGISTER)
    //            throw std::runtime_error(
    //                "Second JFZ argument type mismatch, literals and
    //                accumulator " "are " "not supported!");

    //        unsigned char code = 0b00001000;
    //        write(code);

    //        code = 0b00000000;

    //        if (operands[0].type == ACCUMULATOR) {
    //            code += operands[1].value;
    //            code = code;
    //            write(code);

    //        } else if (operands[0].type == REGISTER) {
    //            code += 192;
    //            code += operands[0].value * 8;
    //            code += operands[1].value;
    //            code = code;
    //            write(code);
    //        }
    //    }

    //    void compileStatement(Expression& expr, std::ofstream& output) {
    //        if (expr.command == NOP)
    //            compileNOP(expr.operands, output);
    //        else if (expr.command == LDA)
    //            compileLDA(expr.operands, output);
    //        else if (expr.command == MV)
    //            compileMV(expr.operands, output);
    //        else if (expr.command == ADD)
    //            compileADD(expr.operands, output);
    //        else if (expr.command == SUB)
    //            compileSUB(expr.operands, output);
    //        else if (expr.command == INC)
    //            compileINC(expr.operands, output);
    //        else if (expr.command == DEC)
    //            compileDEC(expr.operands, output);
    //        else if (expr.command == JMP)
    //            compileJMP(expr.operands, output);
    //        else if (expr.command == JFZ)
    //            compileJFZ(expr.operands, output);
    //        else if (expr.command == HLT)
    //            compileHLT(expr.operands, output);
    //        return;
    //    }

  public:
    Translator(InputInfo& info) {
        if (!std::filesystem::exists(info.getInputPath()))
            throw std::runtime_error("No such file!");

        std::string extension =
            std::filesystem::path(info.getInputPath()).extension();
        if (extension != ".z" && extension != ".zasm")
            throw std::runtime_error(
                "Wrong file extension, it should be .z or .zasm!");
        input.open(info.getInputPath());

        if (info.getFlag("--output").has_value()) {
            output.open(info.getFlag("--output").value());
        } else {
            output.open(
                std::filesystem::path{info.getInputPath()}.parent_path().append(
                    "output.bin"));
        }
        if (info.getFlag("--binary-size").has_value()) {
            targetSize = std::stoull(info.getFlag("--binary-size").value());
        }
    }

    void run() {
        std::string line;
        while (getline(input, line)) {
            std::optional<Expression> expr = parseTokens(tokenize(line));
            if (expr.has_value())
                compileStatement(expr.value());
        }
        if (targetSize > 0 && size > targetSize)
            throw std::runtime_error(
                "Source code is too big to be compiled to file of size: " +
                std::to_string(targetSize));
        else if (size < targetSize) {
            write(std::vector<char>(targetSize - size));
        }
    };

    ~Translator() { output.close(); }
};

#endif  // TRANSLATOR_HPP
