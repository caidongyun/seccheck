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
    }

    void check(const char code[]) {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.addEnabled("performance");

        // Tokenize..
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(code);
        tokenizer.tokenize(istr, "test.cpp");

        // Check char variable usage..
        CheckMiscellaneous checker(&tokenizer, &settings, this);
        checker.improperArithmetic();
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
};

REGISTER_TEST(TestMiscellaneous)
