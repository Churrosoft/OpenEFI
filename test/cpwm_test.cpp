#include <unity.h>

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_rpm(void)
{
    TEST_ASSERT_EQUAL_INT(1, 1);
}

int run_tests()
{
    RUN_TEST(test_rpm);
}