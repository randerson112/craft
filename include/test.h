#ifndef TEST_H
#define TEST_H

#include "parser.h"

// Discovers test files in tests/, builds the project (with test targets),
// runs each test binary, reports per-binary pass/fail and an aggregate result.
int handle_test(const command_t* command_data);

#endif // TEST_H
