set(UNITY_SOURCES vendor/unity.c)


add_executable(test_msgtok test_msgtok.c ${UNITY_SOURCES} ../src/msgtok.c)