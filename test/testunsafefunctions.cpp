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
#include "checkunsafefunctions.h"
#include "testsuite.h"
#include <sstream>

extern std::ostringstream errout;

class TestUnsafeFunctions : public TestFixture {
public:
    TestUnsafeFunctions() : TestFixture("TestUnsafeFunctions") {
    }

private:
    void run() {
        TEST_CASE(tc_stdstrcpy);
		TEST_CASE(tc_strcpy);
		TEST_CASE(tc_strcpydefined);
		TEST_CASE(tc_passwordinvariablename);
		TEST_CASE(tc_passwordotherwise);
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
        CheckUnsafeFunctions checker(&tokenizer, &settings, this);
        checker.unsafeFunctions();
    }

	void tc_stdstrcpy() {
        check("char * foo()\n"
              "{\n"
			  "    char src[10];\n"
			  "    char des[10];\n"
			  "    std::strcpy(des, src, 10);\n"
              "}\n");
		std::string result = errout.str();
        ASSERT_EQUALS("[test.cpp:5]: (style) Obsolete function 'strcpy' called. "
			"It is recommended to use the function 'strncpy' instead.\n", result);
    }

	void tc_strcpy() {
        check("char * foo()\n"
              "{\n"
			  "    char src[10];\n"
			  "    char des[10];\n"
			  "    strcpy(des, src, 10);\n"
              "}\n");
        std::string result = errout.str();
        ASSERT_EQUALS("[test.cpp:5]: (style) Obsolete function 'strcpy' called. "
			"It is recommended to use the function 'strncpy' instead.\n", result);
    }

	// Obsolete function name already be defined, should return empty. 
	void tc_strcpydefined() {
        check(
			"void strcpy(char* src, char* des, int len){};\n"
			"char * foo()\n"
              "{\n"
			  "    char src[10];\n"
			  "    char des[10];\n"
			  "    strcpy(des, src, 10);\n"
              "}\n");
        std::string result = errout.str();
        ASSERT_EQUALS("", result);
    }

	void tc_passwordinvariablename() {
        check("char * foo()\n"
              "{\n"
			  "    char password[10];\n"
			  "    std::string pwd = \"123456\";\n"
			  "    int passwd = 1234;\n"
              "}\n");
		std::string result = errout.str();
        ASSERT_EQUALS("[test.cpp:3]: (style) Suspicious variable name: password maybe identify the hard-coded password.\n"
			"[test.cpp:4]: (style) Suspicious variable name: pwd maybe identify the hard-coded password.\n"
			"[test.cpp:5]: (style) Suspicious variable name: passwd maybe identify the hard-coded password.\n", 
			result);
    }

	void tc_passwordotherwise() {
        check("char * foo()\n"
              "{\n"
			  "    void password(int para){ return para + 1; };\n"
			  "    std::string p1 = \"pwd\";\n"
              "}\n");
		std::string result = errout.str();
        ASSERT_EQUALS("", result);
    }
};

REGISTER_TEST(TestUnsafeFunctions)
