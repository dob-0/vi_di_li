#include <unity.h>

#include "vizzz_core.h"

void test_pack_universe_uses_15bit_layout() {
  TEST_ASSERT_EQUAL_UINT16(0x123, vizzz::packUniverse(0x01, 0x02, 0x03));
  TEST_ASSERT_EQUAL_UINT16(0x7ff, vizzz::packUniverse(0x07, 0x0f, 0x0f));
}

void test_apply_master_scales_levels() {
  TEST_ASSERT_EQUAL_UINT8(255, vizzz::applyMaster(255, 255));
  TEST_ASSERT_EQUAL_UINT8(0, vizzz::applyMaster(200, 0));
  TEST_ASSERT_EQUAL_UINT8(127, vizzz::applyMaster(255, 128));
  TEST_ASSERT_EQUAL_UINT8(50, vizzz::applyMaster(100, 128));
}

void test_clamp_level_applies_ceiling() {
  TEST_ASSERT_EQUAL_UINT8(120, vizzz::clampLevel(120, 200));
  TEST_ASSERT_EQUAL_UINT8(200, vizzz::clampLevel(255, 200));
}

void test_slew_toward_limits_step_size() {
  TEST_ASSERT_EQUAL_UINT8(110, vizzz::slewToward(100, 200, 10));
  TEST_ASSERT_EQUAL_UINT8(190, vizzz::slewToward(200, 100, 10));
  TEST_ASSERT_EQUAL_UINT8(200, vizzz::slewToward(195, 200, 10));
  TEST_ASSERT_EQUAL_UINT8(100, vizzz::slewToward(105, 100, 10));
  TEST_ASSERT_EQUAL_UINT8(200, vizzz::slewToward(100, 200, 0));
}

#if defined(ARDUINO)
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_pack_universe_uses_15bit_layout);
  RUN_TEST(test_apply_master_scales_levels);
  RUN_TEST(test_clamp_level_applies_ceiling);
  RUN_TEST(test_slew_toward_limits_step_size);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_pack_universe_uses_15bit_layout);
  RUN_TEST(test_apply_master_scales_levels);
  RUN_TEST(test_clamp_level_applies_ceiling);
  RUN_TEST(test_slew_toward_limits_step_size);
  return UNITY_END();
}
#endif