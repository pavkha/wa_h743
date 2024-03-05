#include "application.h"

#ifdef NDEBUG
auto app_info = "Locm3 H743 WeAct";
#else
auto app_info = "Locm3 H743 WeAct D";
#endif

auto build_date = __DATE__ " " __TIME__;
auto version = APP_VERSION;

namespace rtos = cpp_freertos;

void application() {
    lcd_app_info();

    static const hw::button k1_button{
            k1_button_port, k1_button_pin, hw::button::active_level::high};
    static const hw::led e3_led{e3_led_port, e3_led_pin, hw::led::active_level::high};

    static sys_time_thread sys_time{};

    static cmd_shell_thread cmd_shell{cmd_path::instance(),
                                      hw::uart4_stream_in::instance(),
                                      hw::uart4_stream_out::instance()};

    static led_blinker_thread led_blink{
            e3_led, e3_led_blink_period_ms, e3_led_blink_pulse_ms};

    LOG_INFO("system start", __FUNCTION__);

    rtos::Thread::StartScheduler();

    while (true) {
    }
}
