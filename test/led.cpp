#include "iled.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::AtLeast;

class led_mock : public iled {
  public:
    using iled::iled;

    MOCK_METHOD(void, pin_set, (), (const, override));
    MOCK_METHOD(void, pin_reset, (), (const, override));
    MOCK_METHOD(void, pin_toggle, (), (const, override));
};

TEST(led_test, active_level_high_on) {
    const led_mock led_active_level_high{iled::active_level::high};

    EXPECT_CALL(led_active_level_high, pin_set()).Times(AtLeast(1));
    led_active_level_high.on();
}

TEST(led_test, active_level_high_off) {
    const led_mock led_active_level_high{iled::active_level::high};

    EXPECT_CALL(led_active_level_high, pin_reset()).Times(AtLeast(1));
    led_active_level_high.off();
}

TEST(led_test, active_level_high_toggle) {
    const led_mock led_active_level_high{iled::active_level::high};

    EXPECT_CALL(led_active_level_high, pin_toggle()).Times(AtLeast(1));
    led_active_level_high.toggle();
}

TEST(led_test, active_level_low_on) {
    const led_mock led_active_level_low{iled::active_level::low};

    EXPECT_CALL(led_active_level_low, pin_reset()).Times(AtLeast(1));
    led_active_level_low.on();
}

TEST(led_test, active_level_low_off) {
    const led_mock led_active_level_low{iled::active_level::low};

    EXPECT_CALL(led_active_level_low, pin_set()).Times(AtLeast(1));
    led_active_level_low.off();
}

TEST(led_test, active_level_low_toggle) {
    const led_mock led_active_level_low{iled::active_level::low};

    EXPECT_CALL(led_active_level_low, pin_toggle()).Times(AtLeast(1));
    led_active_level_low.toggle();
}
