//
// Created by Hayden Roszell on 9/14/22.
//

#ifndef ESTTS_OBC_FILESYSTEM_H
#define ESTTS_OBC_FILESYSTEM_H

#include "constants.h"

class obc_filesystem {
    std::string currently_opened_file;
    size_t file_size;
    estts::session_config * session;
    std::string base_git_dir = "/tmp/estts";
    std::string git_ssh_url = "git@github.com:ERAU-EagleSat-PR/eaglesat-2-telemetry.git";
    std::string get_file_list_cmd = "ES+D11FL";
    std::string write_file_cmd = "ES+D11FW";
    std::string calculate_file_checksum_cmd = "ES+D11FS";

    estts::Status open_file(const std::string& filename);
    estts::Status close_file();
    std::string read_file(unsigned int position, unsigned int size);
    estts::Status delete_file(const std::string& filename);

    std::string execute_command(const std::string& command);

public:

    explicit obc_filesystem(estts::session_config * config);

    std::string download_file(const std::string& filename);

    void set_git_ssh_url(const std::string& url) { git_ssh_url = url; }
    void set_base_git_dir(const std::string& dir) { base_git_dir = dir; }
};


#endif //ESTTS_OBC_FILESYSTEM_H
