#pragma once

#include <vector>
#include <iostream>
#include "types.h"

#define ASSERT_1(expr) if (!(expr)) {throw testing::assertion_failure(std::string("Expression \"") + #expr + "\" failed assertion");}
#define ASSERT_2(expr, msg) if (!(expr)) {throw testing::assertion_failure(msg);}

#define ASSERT_X(x, A, B, FUNC, ...) FUNC
#define ASSERT(...) ASSERT_X(,##__VA_ARGS__, ASSERT_2(__VA_ARGS__), ASSERT_1(__VA_ARGS__))

#define TEST(name) try {\
    name();\
    testing::Results::successes += 1;\
} catch (testing::assertion_failure &e) {\
    std::cerr << e.what() << " in test " << #name << std::endl;\
    testing::Results::failures += 1;\
} catch (std::exception &e) {\
    std::cerr << e.what() << " in test " << #name << std::endl;\
    testing::Results::errors += 1;\
}

#define TEST_CLASS(name) testing::AbstractTest *test##name = new name();\
if (test##name->skip_class()) {\
    std::cout << "Skipped test class " << #name << std::endl;\
    testing::Results::skipped += 1;\
} else {\
    test##name->before_class();\
    try {\
        test##name->run();\
        if (!test##name->delegated) {\
            testing::Results::successes += 1;\
        }\
    } catch (testing::assertion_failure &e) {\
        std::cerr << e.what() << " in test class " << #name << std::endl;\
        if (!test##name->delegated) {\
            testing::Results::failures += 1;\
        }\
    } catch (std::exception &e) {\
        std::cerr << e.what() << " in test class " << #name << std::endl;\
        if (!test##name->delegated) {\
            testing::Results::errors += 1;\
        }\
    }\
    test##name->after_class();\
}\
delete test##name;

#define TEST_METHOD(name) this->delegated = true;\
if (this->skip_test(#name)) {\
    std::cout << "Skipped test method " << #name << std::endl;\
    testing::Results::skipped += 1;\
} else {\
    this->before_test(#name);\
    try {\
        this->name();\
        testing::Results::successes += 1;\
    } catch (testing::assertion_failure &e) {\
        std::cerr << e.what() << " in test method " << #name << std::endl;\
        testing::Results::failures += 1;\
    } catch (std::exception &e) {\
        std::cerr << e.what() << " in test method " << #name << std::endl;\
        testing::Results::errors += 1;\
    }\
    this->after_test(#name);\
}

#define TEST_FILE(name) testing::Results::tests.push_back(&run_##name##_tests);

namespace testing {

struct Results {
    static std::vector<void(*)()> tests;
    static int successes;
    static int failures;
    static int errors;
    static int skipped;
};

class assertion_failure : public std::runtime_error {
public:
    explicit assertion_failure(const std::string& msg);
};

/**
 * A class that contains tests and any necessary boilerplate.
 * To delegate test methods, use the `TEST_METHOD(name)` macro
 * in the run() method definition.
 */
class AbstractTest {

public:
    
    bool delegated = false;
    
    virtual ~AbstractTest();
    
    virtual bool skip_class();
    virtual bool skip_test(std::string name);
    
    virtual void before_class();
    virtual void before_test(std::string name);
    virtual void run() = 0;
    virtual void after_test(std::string name);
    virtual void after_class();
};

/**
 * Run the test suite. Add any files to test with the macro `TEST_FILE(nameThe)` before running this.
 * Test files must define a function `run_[name]_tests()`.
 * In that function, use `TEST(name)` and `TEST_CLASS(name)` to define the function and test
 * classes to be run.
 */
void run_tests(std::string name = "Alpha Utils");

}
