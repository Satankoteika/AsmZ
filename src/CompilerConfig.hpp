#ifndef COMPILERCONFIG_HPP
#define COMPILERCONFIG_HPP

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "Types.hpp"

struct CommandRegistry {
    std::vector<CommandDescriptor*> impl;
    //    const CommandDescriptor* getByName(std::string name) {
    //        auto iterator = std::find_if(
    //            impl.begin(), impl.end(),
    //            [&](const CommandDescriptor* desc) { return desc->name ==
    //            name; });
    //        if (iterator == impl.end())
    //            throw std::runtime_error("No such instruction: " + name +
    //            "!");
    //        return *iterator;
    //    }

    const CommandDescriptor* getByName(std::string name, int opcount) {
        auto iterator = std::find_if(
            impl.begin(), impl.end(), [&](const CommandDescriptor* desc) {
                return desc->name == name && desc->opcount == opcount;
            });
        if (iterator == impl.end())
            throw std::runtime_error("No such instruction: " + name + "!");
        return *iterator;
    }

    const CommandDescriptor* getByType(CommandType type) {
        auto iterator = std::find_if(
            impl.begin(), impl.end(),
            [&](const CommandDescriptor* desc) { return desc->type == type; });
        if (iterator == impl.end())
            throw std::runtime_error(
                "No such instruction: " + std::to_string(type) + "!");
        return *iterator;
    }
};

struct CompilerConfig {
    inline static std::vector<std::string> acceptableFlags;
    inline static CommandRegistry commands;
};

#endif  // COMPILERCONFIG_HPP
