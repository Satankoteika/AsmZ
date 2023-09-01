#ifndef TYPES_HPP
#define TYPES_HPP

#include <stdexcept>
#include <string>
#include <vector>

enum OperandType {
    ACCUMULATOR = 0b00000001,
    REGISTER = 0b00000010,
    LITERAL = 0b00000100
};

enum CommandType {
    NOP,
    LDA,
    MV,
    ADD,
    SUB,
    INC,
    DEC,
    JMP,
    JFZ,
    IN,
    OUT,
    PUSH,
    POP,
    HLT
};

struct Operand {
    OperandType type;
    unsigned char value;
    Operand(std::string string) {
        if (string == "A")
            type = ACCUMULATOR;
        else if (string[0] == 'R') {
            type = REGISTER;
            int literal = std::stoi(string.data() + 1, 0, 16);
            if (literal >= 0 && literal < 256)
                value = literal;
            else
                throw std::runtime_error(
                    "Register number is not hexadecimal 8-bit integer!");
        } else {
            int literal = std::stoi(string, 0, 16);
            if (literal >= 0 && literal < 256)
                value = literal;
            else
                throw std::runtime_error(
                    "Literal is not hexadecimal 8-bit integer!");
            type = LITERAL;
        }
    }
};

struct CommandDescriptor {
    CommandType type;
    std::string name;
    char code;

    size_t opcount;
    std::vector<char> suitableOperandTypes;

    virtual std::vector<char> compile(std::vector<Operand>&) const = 0;
    virtual ~CommandDescriptor() = default;
};

struct Expression {
    const CommandDescriptor* command;
    std::vector<Operand> operands;
};

#endif  // TYPES_HPP
