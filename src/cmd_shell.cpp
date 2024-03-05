#include "cmd_shell.h"
#include "FreeRTOSConfig.h"
#include "cmd_path.h"
#include "log.h"
#include "rtc_sys_time.h"
#include "sys_time.h"
#include "text_colors.h"
#include "uart_stream_in.h"
#include "uart_stream_out.h"

#include <critical.hpp>

#include <cctype>
#include <cstdio>
#include <cstring>

extern "C" uint32_t get_dev_idc(void);
static void add_path_commands(cmd_path& path);

const auto cmd_shell_task_name{"cmd_shell"};
const auto cmd_shell_task_stack_size{configMINIMAL_STACK_SIZE * 8};
const auto cmd_shell_task_priority{configMAX_PRIORITIES - 1};

extern const char* app_info;
extern const char* version;
extern const char* build_date;

namespace rtos = cpp_freertos;

static inline bool check_cmd_args_empty(cmd_path::cmd_args& args, const char* name) {
    if (std::strlen(reinterpret_cast<const char*>(args.data())) != std::strlen(name)) {
        std::snprintf(reinterpret_cast<char*>(args.data()),
                      args.max_size(),
                      "\r\ncommand has no arguments, usage: %s\r\n",
                      name);
        return false;
    }

    return true;
}

cmd_shell_thread::cmd_shell_thread(cmd_path& path, istream_in& in, istream_out& out)
    : Thread(cmd_shell_task_name, cmd_shell_task_stack_size, cmd_shell_task_priority),
      path_{path},
      in_{in},
      out_{out} {
    this->Start();
}

void cmd_shell_thread::Run() {
    static cmd_path::cmd_args cmd_line{""};
    static cmd_path::cmd curr_cmd{};
    static cmd_path::cmd_name curr_cmd_name{""};

    uint8_t rx_byte{};
    auto byte_count{cmd_line.max_size()};
    auto spec_symbol_code{'\033'};
    auto backspace{'\177'};
    auto rus_utf8_code_0{'\320'};
    auto rus_utf8_code_1{'\321'};

    auto out_cmd_line = [this] {
        out_.write(cmd_line.data(),
                   static_cast<uint16_t>(
                           std::strlen(reinterpret_cast<const char*>(cmd_line.data()))));
    };

    auto out_cmd_prompt = [&] {
        std::snprintf(reinterpret_cast<char*>(cmd_line.data()),
                      cmd_line.max_size(),
                      "%s",
                      "\r\ncmd_shell> ");

        out_cmd_line();

        byte_count = 0;
    };

    auto out_app_info = [&] {
        std::snprintf(reinterpret_cast<char*>(cmd_line.data()),
                      cmd_line.max_size(),
                      "\r\n%s %s\r\nbuild: %s\r\n",
                      app_info,
                      version,
                      build_date);

        out_cmd_line();
    };

    auto out_byte = [this](uint8_t byte) { out_.write(&byte, 1); };

    auto out_endl = [this] {
        out_.write(reinterpret_cast<uint8_t*>(const_cast<char*>("\r\n")), 2);
    };

    auto set_text_color = [this](const char* color) {
        out_.write(reinterpret_cast<uint8_t*>(const_cast<char*>(color)),
                   static_cast<uint16_t>(std::strlen(color)));
    };

    auto get_cmd_name = [] {
        auto i = 0U;

        for (; i < cmd_path::cmd_name_size; ++i) {
            if (cmd_line[i] != ' ') {
                curr_cmd_name[i] = cmd_line[i];
            } else {
                break;
            }
        }

        curr_cmd_name[i] = '\0';
    };

    auto picocom_backspace = [&] {
        if (byte_count != 0) {
            --byte_count;
            out_byte(rx_byte = '\b');
            out_byte(' ');
        }
    };

    auto in_read_line = [&] {
        while (in_.read(&rx_byte, 1) != 0) {
            if (rx_byte != '\r') {
                if (rx_byte != spec_symbol_code) {
                    if (rx_byte == rus_utf8_code_0 || rx_byte == rus_utf8_code_1) {
                        continue;
                    }

                    if (rx_byte == backspace) {
                        picocom_backspace();
                    } else {
                        cmd_line[byte_count++] = rx_byte;
                        byte_count %= cmd_line.max_size();
                    }

                    out_byte(rx_byte);
                }
            } else {
                if (byte_count != 0) {
                    cmd_line[byte_count] = '\0';
                    LOG_INFO(reinterpret_cast<const char*>(cmd_line.data()), GetName());
                    break;
                } else {
                    out_cmd_prompt();
                }
            }
        }
    };

    auto ls_cmd = [&, this]() {
        if (check_cmd_args_empty(cmd_line, "ls")) {
            if (path_.cmd_count() == 0) {
                std::snprintf(reinterpret_cast<char*>(cmd_line.data()),
                              cmd_line.max_size(),
                              "\r\nno installed commands\r\n");
            } else {
                out_endl();
                set_text_color(text_color_blue_bold);

                for (uint8_t i = 0; path_.get_cmd(curr_cmd, i); ++i) {
                    if (curr_cmd.name_ != path_.empty_cmd_.name_) {
                        std::snprintf(reinterpret_cast<char*>(cmd_line.data()),
                                      cmd_line.max_size(),
                                      "%s\t",
                                      curr_cmd.name_.data());
                        out_cmd_line();
                    }
                }

                set_text_color(text_color_default);
                out_endl();
            }
        } else {
            out_cmd_line();
        }
    };

    auto cmd_exec = [&] {
        if (curr_cmd_name == "ls") {
            ls_cmd();
        } else {
            curr_cmd.name_ = curr_cmd_name;
            curr_cmd.exec_ = [](cmd_path::cmd_args& args) {
                std::snprintf(reinterpret_cast<char*>(args.data()),
                              args.max_size(),
                              "\r\ncommand not found\r\n");
            };

            path_.get_cmd(curr_cmd);
            curr_cmd.exec_(cmd_line);

            if (cmd_line != "ok") {
                out_cmd_line();
            }
        };
    };

    LOG_INFO("start", GetName());

    while (true) {
        switch (state_) {
            case state::start:
                out_app_info();
                add_path_commands(path_);
                out_cmd_prompt();
                state_ = state::run;
                break;
            case state::run:
                in_read_line();
                get_cmd_name();
                cmd_exec();
                out_cmd_prompt();
                break;
            default:
                break;
        }
    }
}

cmd_path::cmd uname_cmd{"uname", [](cmd_path::cmd_args& args) {
                            if (!check_cmd_args_empty(args, "uname")) {
                                return;
                            }

                            std::snprintf(reinterpret_cast<char*>(args.data()),
                                          args.max_size(),
                                          "\r\n%s %s %s cpuidc 0x%lX\r\n",
                                          app_info,
                                          version,
                                          build_date,
                                          get_dev_idc());
                        }};

cmd_path::cmd clock_cmd{
        "clock",
        [](cmd_path::cmd_args& args) {
            static const uint8_t argc{4};
            static const uint8_t max_arg_length{16};
            static std::array<std::array<char, max_arg_length>, argc> argv;
            enum : uint8_t { name, sub_cmd, date, time };

            auto args_format{"%5s%3s%8s%8s"};
            auto date_format{"%d-%d-%d"};
            auto time_format{"%d:%d:%d"};

            auto sscanf_res = std::sscanf(reinterpret_cast<const char*>(args.data()),
                                          args_format,
                                          argv[name].data(),
                                          argv[sub_cmd].data(),
                                          argv[date].data(),
                                          argv[time].data());

            auto usage = [&args] {
                std::snprintf(reinterpret_cast<char*>(args.data()),
                              args.max_size(),
                              "\r\nusage: clock set DD-MM-YY HH:MM:SS\r\n");
            };

            if (sscanf_res != argc || std::strcmp(argv[sub_cmd].data(), "set") != 0) {
                usage();
                return;
            }

            int day{}, month{}, year{};

            std::sscanf(reinterpret_cast<const char*>(argv[date].data()),
                        date_format,
                        &day,
                        &month,
                        &year);

            rtc_date new_date{};

            new_date.week_day = 1;
            new_date.day = static_cast<uint8_t>(day);
            new_date.month = static_cast<uint8_t>(month);
            new_date.year = static_cast<uint8_t>(year);

            int hours{}, minuts{}, seconds{};

            std::sscanf(reinterpret_cast<const char*>(argv[time].data()),
                        time_format,
                        &hours,
                        &minuts,
                        &seconds);

            rtc_time new_time{};

            new_time.hours = static_cast<uint8_t>(hours);
            new_time.minutes = static_cast<uint8_t>(minuts);
            new_time.seconds = static_cast<uint8_t>(seconds);

            if (hw::rtc_int::instance().set(new_date, new_time)) {
                std::snprintf(
                        reinterpret_cast<char*>(args.data()), args.max_size(), "ok");
            } else {
                usage();
            }
        }};

cmd_path::cmd date_cmd{"date", [](cmd_path::cmd_args& args) {
                           static systime_string time{};

                           if (!check_cmd_args_empty(args, "date")) {
                               return;
                           }

                           if (get_sys_time(time)) {
                               std::snprintf(reinterpret_cast<char*>(args.data()),
                                             args.max_size(),
                                             "\r\n%s\r\n",
                                             reinterpret_cast<char*>(time.data()));
                           } else {
                               std::snprintf(reinterpret_cast<char*>(args.data()),
                                             args.max_size(),
                                             "\r\nerror\r\n");
                           }
                       }};

static void add_path_commands(cmd_path& path) {
    path.add_cmd(uname_cmd);
    path.add_cmd(clock_cmd);
    path.add_cmd(date_cmd);
}
