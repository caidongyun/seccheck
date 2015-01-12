/*
* Seccheck - A tool for security C/C++ code analysis
* Copyright (C) 2014 Wang Anyu
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "tokenize.h"
#include "checkmiscellaneous.h"
#include "testsuite.h"
#include <sstream>

extern std::ostringstream errout;

class TestMiscellaneous : public TestFixture {
public:
    TestMiscellaneous() : TestFixture("TestMiscellaneous") {
    }

private:
    void run() {
        TEST_CASE(time_t_oper);
        TEST_CASE(time_t_oper_good);
        TEST_CASE(bitwise_var_good);
        TEST_CASE(bitwise_var_good2);
        TEST_CASE(bitwise_var_good3);
        TEST_CASE(bitwise_var_good4);
        TEST_CASE(bitwise_var_bad1);
        TEST_CASE(bitwise_var_bad2);
        TEST_CASE(signed_char_err1);
        TEST_CASE(signed_char_err2);
        TEST_CASE(modify_std_err1);
        TEST_CASE(modify_std_ok1);
        TEST_CASE(return_errno_err1);
    }

    void check(const char code[]) {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.addEnabled("performance");

        // FOR DEBUG
        //settings.debug = true;
        //settings.debugwarnings = true;

        // Tokenize..
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(code);
        tokenizer.tokenize(istr, "test.cpp");

        //tokenizer.printDebugOutput();

        // Check char variable usage..
        CheckMiscellaneous checker(&tokenizer, &settings, this);
        checker.runChecks();
    }

    // vector container copying by parameter
    void time_t_oper() {
        check(
            "int do_work(int seconds_to_work) {\n"
            "time_t start = time(NULL);\n"
            "if (start == (time_t)(-1)) {\n"
            "}\n"
            "while (time(NULL) < start + seconds_to_work) {\n"
            "}\n"
            "return 0;\n"
            "}"
            );
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:5]: (warning) There is no safe way to manually perform arithmetic on the time_t type.\n", 
            s);
    }

    // vector container copying by parameter
    void time_t_oper_good() {
        check(
            "int do_work(int seconds_to_work) {\n"
            "time_t start = time(NULL);time_t current = start;\n"
            "if (start == (time_t)(-1)) {\n"
            "}\n"
            "while (difftime(current, start) < seconds_to_work) {\n"
            "current = time(NULL);\n"
            "}\n"
            "return 0;\n"
            "}"
            );
        std::string s = errout.str();
        ASSERT_EQUALS("", 
            s);
    }

    void bitwise_var_good() {
        check("void foo()\n"
              "{\n"
              "    unsigned int a = 10, b = 20;\n"
              "    unsigned int c = a & b;\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("", 
            s);
    }

    void bitwise_var_good2() {
        check("void foo()\n"
              "{\n"
              "    unsigned int a = 10; const unsigned int b = 20;\n"
              "    unsigned int c = a & b;\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("", 
            s);
    }

    // the default type of const number is int
    void bitwise_var_good3() {
        check("void foo()\n"
              "{\n"
              "    unsigned int a = 10; int b = 20;\n"
              "    unsigned int c = a & 40;\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:4]: (warning) Bitwise operators should only be used with unsigned integer operands.\n", 
            s);
    }

    // Not a bitwise operator
    void bitwise_var_good4() {
        check("void foo()\n"
              "{\n"
              "    int a;\n"
              "    int* c = &a;\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("", 
            s);
    }

    void bitwise_var_bad1() {
        check("void foo()\n"
              "{\n"
              "    unsigned int a = 10; int b = 20;\n"
              "    unsigned int c = a & b;\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:4]: (warning) Bitwise operators should only be used with unsigned integer operands.\n", 
            s);
    }

    void bitwise_var_bad2() {
        check("void foo()\n"
              "{\n"
              "    unsigned int a = 10; int b = 20;\n"
              "    unsigned int c = a & (-10);\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:4]: (warning) Bitwise operators should only be used with unsigned integer operands.\n", 
            s);
    }

    void bitwise_var_bad3() {
        check("void foo()\n"
              "{\n"
              "    unsigned int a = 10; int b = 20;\n"
              "    unsigned int c = a & (1.2);\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:4]: (warning) Bitwise operators should only be used with unsigned integer operands.\n", 
            s);
    }

    void signed_char_err1() {
        check("bool foo(const char* s)\n"
              "{\n"
              "    const char *t = s;\n"
              "    return isspace(*t);\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:4]: (warning) Arguments to character handling functions must be representable as an unsigned char.\n", 
            s);
    }

    void signed_char_err2() {
        check("void foo()\n"
              "{\n"
              "    const char a = 'a';\n"
              "    isspace(a);\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:4]: (warning) Arguments to character handling functions must be representable as an unsigned char.\n", 
            s);
    }

    void modify_std_err1() {
        check("namespace std { void foo()\n"
              "{\n"
              "} }");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:1]: (warning) Do not modify the standard namespaces.\n", 
            s);
    }

    void modify_std_ok1() {
        check("namespace std2 { void foo()\n"
              "{\n"
              "} }");
        std::string s = errout.str();
        ASSERT_EQUALS("", s);
    }

    void return_errno_err1() {
        check("int foo()\n"
              "{\n"
              "    errno = 0;\n"
              "    return errno;\n"
              "}");
        std::string s = errout.str();
        ASSERT_EQUALS("[test.cpp:4]: (warning) Functions that return errno should change to a return type of errno_t.\n", s);
    }
};

REGISTER_TEST(TestMiscellaneous)
