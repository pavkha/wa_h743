#pragma once

#include "singleton.h"

#include <array>
#include <cstdint>

class cmd_path : public static_singleton<cmd_path> {
  public:
    friend class static_singleton<cmd_path>;

    static const uint8_t cmd_name_size{32};
    static const uint8_t cmds_num{32};
    static const uint16_t cmd_args_size{256};

    using cmd_args = std::array<uint8_t, cmd_args_size>;
    using cmd_name = std::array<uint8_t, cmd_name_size + 1>;

    struct cmd {
        cmd_name name_{""};
        void (*exec_)(cmd_args&){[](cmd_args&) {}};
    };

    const cmd empty_cmd_{" empty "};

    bool add_cmd(const cmd& acmd);
    bool remove_cmd(const cmd& rcmd);
    bool get_cmd(cmd& gcmd) const;
    bool get_cmd(cmd& gcmd, uint8_t cmdi) const;
    uint8_t cmd_count() { return cmd_count_; }

  private:
    cmd_path();

    std::array<cmd, cmds_num> path_;
    uint8_t cmd_count_{};
};

bool operator==(const cmd_path::cmd_name& lname, const cmd_path::cmd_name& rname);
bool operator==(const cmd_path::cmd_args& array, const char* string);
bool operator!=(const cmd_path::cmd_args& array, const char* string);
bool operator==(const cmd_path::cmd_name& lname, const char* rname);
