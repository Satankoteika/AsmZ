#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Types.hpp"

struct CommandNOP : CommandDescriptor {
    CommandNOP() {
        type = NOP;
        name = "NOP";
        code = 0b00000000;

        opcount = 0;
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        return {};
    }
};

struct CommandLDA : CommandDescriptor {
    CommandLDA() {
        type = LDA;
        name = "LDA";
        code = 0b00000001;

        opcount = 1;
        suitableOperandTypes = {LITERAL};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        char code = operands[0].value;
        return {code};
    }
};

struct CommandMV1 : CommandDescriptor {
    CommandMV1() {
        type = MV;
        name = "MV";
        code = 0b00010010;

        opcount = 1;
        suitableOperandTypes = {REGISTER};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        char code = 64;
        code += operands[0].value;
        return {code};
    }
};

struct CommandMV2 : CommandDescriptor {
    CommandMV2() {
        type = MV;
        name = "MV";
        code = 0b00010010;

        opcount = 2;
        suitableOperandTypes = {REGISTER, ACCUMULATOR | REGISTER | LITERAL};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        std::vector<char> result;
        char code = 0;

        if (operands[1].type == ACCUMULATOR) {
            code += operands[0].value;
            result.push_back(code);

        } else if (operands[1].type == REGISTER) {
            code += 128;
            code += operands[1].value * 8;
            code += operands[0].value;
            result.push_back(code);

        } else if (operands[1].type == LITERAL) {
            code += 192;
            code += operands[0].value;
            result.push_back(code);

            code = operands[1].value;
            result.push_back(code);
        }

        return result;
    }
};

struct CommandADD : CommandDescriptor {
    CommandADD() {
        type = ADD;
        name = "ADD";
        code = 0b00010011;

        opcount = 2;
        suitableOperandTypes = {REGISTER | ACCUMULATOR, REGISTER | LITERAL};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        std::vector<char> result;
        char code = 0;

        if (operands[0].type == ACCUMULATOR) {
            if (operands[1].type == REGISTER) {
                code += operands[1].value;
                result.push_back(code);
            } else if (operands[1].type == LITERAL) {
                //?
            }
        } else if (operands[0].type == REGISTER) {
            if (operands[1].type == REGISTER) {
                code += 128;
                code += operands[0].value * 8;
                code += operands[1].value;
                result.push_back(code);
            } else if (operands[1].type == LITERAL) {
                code += 192;
                code += operands[0].value;
                result.push_back(code);

                code = operands[1].value;
                result.push_back(code);
            }
        }

        return result;
    }
};

struct CommandSUB : CommandDescriptor {
    CommandSUB() {
        type = SUB;
        name = "SUB";
        code = 0b00010100;

        opcount = 2;
        suitableOperandTypes = {REGISTER | ACCUMULATOR, REGISTER | LITERAL};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        std::vector<char> result;
        char code = 0;

        if (operands[0].type == ACCUMULATOR) {
            if (operands[1].type == REGISTER) {
                code += operands[1].value;
                result.push_back(code);
            } else if (operands[1].type == LITERAL) {
                //?
            }
        } else if (operands[0].type == REGISTER) {
            if (operands[1].type == REGISTER) {
                code += 128;
                code += operands[0].value * 8;
                code += operands[1].value;
                result.push_back(code);
            } else if (operands[1].type == LITERAL) {
                code += 192;
                code += operands[0].value;
                result.push_back(code);

                code = operands[1].value;
                result.push_back(code);
            }
        }

        return result;
    }
};

struct CommandINC : CommandDescriptor {
    CommandINC() {
        type = INC;
        name = "INC";
        code = 0b10000101;

        opcount = 0;
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        return {};
    }
};

struct CommandDEC : CommandDescriptor {
    CommandDEC() {
        type = DEC;
        name = "DEC";
        code = 0b10000110;

        opcount = 0;
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        return {};
    }
};

struct CommandJMP : CommandDescriptor {
    CommandJMP() {
        type = JMP;
        name = "JMP";
        code = 0b00000111;

        opcount = 1;
        suitableOperandTypes = {REGISTER | ACCUMULATOR};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        std::vector<char> result;
        char code = 0;

        if (operands[0].type == ACCUMULATOR) {
            code = 0;
            result.push_back(code);
        } else if (operands[0].type == REGISTER) {
            code += 192;
            code += operands[0].value;
            result.push_back(code);
        }

        return result;
    }
};

struct CommandJFZ : CommandDescriptor {
    CommandJFZ() {
        type = JFZ;
        name = "JFZ";
        code = 0b00001000;

        opcount = 2;
        suitableOperandTypes = {REGISTER | ACCUMULATOR, REGISTER};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        std::vector<char> result;
        char code = 0;

        if (operands[0].type == ACCUMULATOR) {
            code = operands[1].value;
            result.push_back(code);
        } else if (operands[0].type == REGISTER) {
            code += 192;
            code += operands[0].value * 8;
            code += operands[1].value;
            result.push_back(code);
        }

        return result;
    }
};

struct CommandIN : CommandDescriptor {
    CommandIN() {
        type = IN;
        name = "IN";
        code = 0b00000101;

        opcount = 2;
        suitableOperandTypes = {REGISTER, REGISTER};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        char code = operands[0].value;
        code += operands[1].value * 8;
        return {code};
    }
};

struct CommandOUT : CommandDescriptor {
    CommandOUT() {
        type = OUT;
        name = "OUT";
        code = 0b00000110;

        opcount = 2;
        suitableOperandTypes = {REGISTER, REGISTER};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        char code = operands[0].value;
        code += operands[1].value * 8;
        return {code};
    }
};

struct CommandPUSH : CommandDescriptor {
    CommandPUSH() {
        type = PUSH;
        name = "PUSH";
        code = 0b00001001;

        opcount = 2;
        suitableOperandTypes = {REGISTER | ACCUMULATOR};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        char code = operands[0].value;
        code += operands[1].value * 8;
        return {code};
    }
};

struct CommandPOP : CommandDescriptor {
    CommandPOP() {
        type = POP;
        name = "POP";
        code = 0b00001010;

        opcount = 2;
        suitableOperandTypes = {REGISTER | ACCUMULATOR};
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        char code = 0;
        if (operands[0].type == ACCUMULATOR) {
            code = 0;
        } else if (operands[0].type == REGISTER) {
            code += 192;
            code += operands[0].value;
        }
        return {code};
    }
};

struct CommandHLT : CommandDescriptor {
    CommandHLT() {
        type = HLT;
        name = "HLT";
        code = 0b11111111;

        opcount = 0;
    }

    std::vector<char> compile(std::vector<Operand>& operands) const override {
        return {};
    }
};

inline static CommandNOP cNOP{};
inline static CommandLDA cLDA{};
inline static CommandMV1 cMV1{};
inline static CommandMV2 cMV2{};
inline static CommandADD cADD{};
inline static CommandSUB cSUB{};
inline static CommandINC cINC{};
inline static CommandDEC cDEC{};
inline static CommandJMP cJMP{};
inline static CommandJFZ cJFZ{};
inline static CommandIN cIN{};
inline static CommandOUT cOUT{};
inline static CommandPUSH cPUSH{};
inline static CommandPOP cPOP{};
inline static CommandHLT cHLT{};

#endif  // COMMANDS_HPP
