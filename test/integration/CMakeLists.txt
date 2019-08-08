enable_testing()
add_test(NAME test_register
         COMMAND ${PYTHON_EXECUTABLE}
         ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_register.py)