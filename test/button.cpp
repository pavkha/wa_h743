#include "ibutton.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class fake_button : public ibutton {
  public:
    fake_button(active_level level, active_level current_level)
        : ibutton{level}, pin_level_{current_level} {}

    active_level pin_read() const override { return pin_level_; }

  private:
    active_level pin_level_;
};

TEST(button_test, active_level_high_pushed) {
    const fake_button button{ibutton::active_level::high, ibutton::active_level::high};

    ASSERT_TRUE(button.pushed());
}

TEST(button_test, active_level_high_released) {
    const fake_button button{ibutton::active_level::high, ibutton::active_level::low};

    ASSERT_FALSE(button.pushed());
}

TEST(button_test, active_level_low_pushed) {
    const fake_button button{ibutton::active_level::low, ibutton::active_level::low};

    ASSERT_TRUE(button.pushed());
}

TEST(button_test, active_level_low_released) {
    const fake_button button{ibutton::active_level::low, ibutton::active_level::high};

    ASSERT_FALSE(button.pushed());
}
