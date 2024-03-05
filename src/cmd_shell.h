#pragma once

#include <cstdint>
#include <thread.hpp>

class cmd_path;
class istream_in;
class istream_out;

namespace rtos = cpp_freertos;

class cmd_shell_thread : rtos::Thread {
  public:
    cmd_shell_thread(cmd_path& path, istream_in& in, istream_out& out);
    cmd_shell_thread(const cmd_shell_thread&) = delete;
    cmd_shell_thread& operator=(const cmd_shell_thread&) = delete;
    cmd_shell_thread(cmd_shell_thread&&) = delete;
    cmd_shell_thread& operator=(cmd_shell_thread&&) = delete;
    ~cmd_shell_thread() = default;

  private:
    void Run() override;

    cmd_path& path_;
    istream_in& in_;
    istream_out& out_;

    enum class state : uint8_t { start, run };
    state state_{state::start};
};
