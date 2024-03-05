#include "cmd_path.h"

#include <cstring>

cmd_path::cmd_path() {
    for (auto& cmd : path_) {
        cmd = empty_cmd_;
    }
}

bool cmd_path::add_cmd(const cmd_path::cmd& acmd) {
    for (auto& cmd : path_) {
        if (cmd.name_ == empty_cmd_.name_) {
            cmd = acmd;
            ++cmd_count_;
            return true;
        }
    }

    return false;
}

bool cmd_path::remove_cmd(const cmd_path::cmd& rcmd) {
    for (auto& cmd : path_) {
        if (cmd.name_ == rcmd.name_) {
            cmd = empty_cmd_;
            --cmd_count_;
            return true;
        }
    }

    return false;
}

bool cmd_path::get_cmd(cmd_path::cmd& gcmd) const {
    for (auto& cmd : path_) {
        if (cmd.name_ == gcmd.name_) {
            gcmd.exec_ = cmd.exec_;
            return true;
        }
    }

    return false;
}

bool cmd_path::get_cmd(cmd_path::cmd& gcmd, uint8_t cmdi) const {
    if (cmdi < cmds_num) {
        gcmd = path_[cmdi];
        return true;
    }

    return false;
}

bool operator==(const cmd_path::cmd_name& lname, const cmd_path::cmd_name& rname) {
    return std::strcmp(reinterpret_cast<const char*>(lname.data()),
                       reinterpret_cast<const char*>(rname.data())) == 0;
}

bool operator==(const cmd_path::cmd_args& array, const char* string) {
    return std::strcmp(reinterpret_cast<const char*>(array.data()), string) == 0;
}

bool operator!=(const cmd_path::cmd_args& array, const char* string) {
    return !operator==(array, string);
}

bool operator==(const cmd_path::cmd_name& lname, const char* rname) {
    return std::strcmp(reinterpret_cast<const char*>(lname.data()), rname) == 0;
}
