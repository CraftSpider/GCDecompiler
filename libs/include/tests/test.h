#pragma once

#include <vector>
#include <iostream>
#include "types.h"
#include "tests/abstract_test.h"

#define ASSERT_1(expr) if (!(expr)) throw testing::assertion_failure(std::string("Expression \"") + #expr + "\" failed assertion")
#define ASSERT_2(expr, msg) if (!(expr)) throw testing::assertion_failure(msg)

#define ASSERT_X(x, A, B, FUNC, ...) FUNC
#define ASSERT(...) ASSERT_X(,##__VA_ARGS__, ASSERT_2(__VA_ARGS__), ASSERT_1(__VA_ARGS__))

#define TEST(name) try {\
    name();\
    testing::__on_success(#name);\
} catch (testing::assertion_failure &e) {\
    testing::__on_failure(#name, e);\
} catch (testing::skip_test &e) {\
    testing::__on_skip(#name, e);\
} catch (std::exception &e) {\
    testing::__on_error(#name, e);\
}

// TODO: Skip error, any test that throws it is counted as 'skipped'
#define TEST_CLASS(name) testing::AbstractTest* test##name = new name();\
if (test##name->skip_class()) {\
    testing::__on_skip(#name, testing::CLASS);\
} else {\
    test##name->before_class();\
    try {\
        test##name->run();\
        if (!test##name->delegated) {\
            testing::__on_success(#name, testing::CLASS);\
        }\
    } catch (testing::assertion_failure &e) {\
        if (!test##name->delegated) {\
            testing::__on_failure(#name, e, testing::CLASS);\
        }\
    } catch (testing::skip_test &e) {\
        testing::__on_skip(#name, e, testing::CLASS);\
    } catch (std::exception &e) {\
        if (!test##name->delegated) {\
            testing::__on_error(#name, e, testing::CLASS);\
        }\
    }\
    test##name->after_class();\
}\
delete test##name;

#define TEST_METHOD(name) this->delegated = true;\
if (this->skip_test(#name)) {\
    testing::__on_skip(#name, testing::METHOD);\
} else {\
    this->before_test(#name);\
    try {\
        this->name();\
        testing::__on_success(#name, testing::METHOD);\
    } catch (testing::assertion_failure &e) {\
        testing::__on_failure(#name, e, testing::METHOD);\
    } catch (testing::skip_test &e) {\
        testing::__on_skip(#name, e, testing::METHOD);\
    } catch (std::exception &e) {\
        testing::__on_error(#name, e, testing::METHOD);\
    }\
    this->after_test(#name);\
}

#define TEST_FILE(name) testing::__ToRun::tests.push_back(&run_##name##_tests);

namespace testing {

struct __ToRun {
    static std::vector<void(*)()> tests;
};

struct __Results {
    static std::vector<std::string> failure_messages;
    static std::vector<std::string> skip_messages;
    static std::vector<std::string> error_messages;
};

enum TestType {
    FUNCTION, METHOD, CLASS
};

struct Results {
    static int successes;
    static int failures;
    static int errors;
    static int skipped;

    static int total();
    static int success_percent();
    static int failure_percent();
    static int error_percent();
    static int skipped_percent();
};

class test_error : public std::runtime_error {
protected:
    explicit test_error(const std::string& msg);
};

class assertion_failure : public test_error {
public:
    explicit assertion_failure(const std::string& msg);
};

class skip_test : public test_error {
public:
    skip_test();
    explicit skip_test(const std::string& msg);
};

void __on_success(std::string name, TestType type = FUNCTION);
void __on_failure(std::string name, assertion_failure& e, TestType type = FUNCTION);
void __on_skip(std::string name, TestType type = FUNCTION);
void __on_skip(std::string name, skip_test& e, TestType type = FUNCTION);
void __on_error(std::string name, std::exception& e, TestType type = FUNCTION);

/**
 * Run the test suite. Add any files to test with the macro `TEST_FILE(nameThe)` before running this.
 * Test files must define a function `run_[name]_tests()`.
 * In that function, use `TEST(name)` and `TEST_CLASS(name)` to define the function and test
 * classes to be run.
 */
void run_tests(std::string name = "Alpha Tools");

}
