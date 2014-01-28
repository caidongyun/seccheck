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
#include "checkcomplexcopying.h"
#include "testsuite.h"
#include <sstream>

extern std::ostringstream errout;

class TestComplexCopying : public TestFixture {
public:
    TestComplexCopying() : TestFixture("TestComplexCopying") {
    }

private:
    void run() {
        TEST_CASE(vectorcopying);
		TEST_CASE(vectorEqual);
		TEST_CASE(vectorRefEqual);
		TEST_CASE(vectorIterator);
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
        CheckComplexCopying checkComplexCopying(&tokenizer, &settings, this);
        checkComplexCopying.checkComplexParameters();
    }

	// vector container copying by parameter
    void vectorcopying() {
        check("void foo(stl::vector<std::string> p)\n"
              "{\n"
              "    stl::vector<std::string> a = p;\n"
              "}");
        ASSERT_EQUALS("[test.cpp:1]: (performance) Complex objects copying "
			"in Function foo may slow down system performance.\n"
			"[test.cpp:3]: (performance) Complex objects equation "
			"may slow down system performance.\n", 
			errout.str());
    }

	// vector container equation
	void vectorEqual() {
        check("void foo()\n"
              "{\n"
			  "    stl::vector<int> p;\n"
			  "    p.push_back(123);\n"
              "    stl::vector<int> a = p;\n"
              "}");
        ASSERT_EQUALS("[test.cpp:5]: (performance) Complex objects equation may slow down system performance.\n", errout.str());
    }

	// vector container pointer equation
	void vectorRefEqual() {
        check("void foo()\n"
              "{\n"
			  "    stl::vector<int> p;\n"
			  "    p.push_back(123);\n"
              "    stl::vector<int>* a = &p;\n"
              "}");
        ASSERT_EQUALS("", errout.str());
    }

	// iterator
	void vectorIterator() {
        check("void foo()\n"
              "{\n"
			  "    stl::vector<int> p;\n"
			  "    p.push_back(123);\n"
			  "    stl::vector<int>::iterator itr = p.begin();\n"
              "}");
        ASSERT_EQUALS("", errout.str());
    }
};

REGISTER_TEST(TestComplexCopying)
