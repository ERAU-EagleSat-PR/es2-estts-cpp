//
// Created by Hayden Roszell on 9/10/22.
//

#ifndef ESTTS_SESSION_MANAGER_MODIFIER_H
#define ESTTS_SESSION_MANAGER_MODIFIER_H

#include <map>
#include "groundstation_manager.h"

class session_manager_modifier {
private:
    std::map<std::string, std::function<estts::Status(std::string)>> modifier_command_map;
public:
    session_manager_modifier() = default;

    void insert_execution_modifier(const std::string& command, const std::function<estts::Status(std::string)>& handler) {
        modifier_command_map.insert(std::pair<std::string, std::function<estts::Status(std::string)>>(command, handler));
    }

    estts::Status execute_command_modifier(std::string command) {
        std::string validator = command;
        if (validator.size() > 8)
            validator.resize(8);

        if (modifier_command_map.find(validator) != modifier_command_map.end()) {
            SPDLOG_INFO("Command {} has an execution modifier; branching into its handler.", command);
            auto modifier = modifier_command_map[validator];
            if (modifier)
                return modifier(command);
        }
        return estts::ES_NOTFOUND;
    }
};


#endif //ESTTS_SESSION_MANAGER_MODIFIER_H
