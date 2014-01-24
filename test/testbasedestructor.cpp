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
#include "checkbasedestructor.h"
#include "testsuite.h"
#include <sstream>

extern std::ostringstream errout;

class TestBaseDestructor : public TestFixture {
public:
    TestBaseDestructor() : TestFixture("TestBaseDestructor") {
    }

private:
    void run() {
        TEST_CASE(baseDerivedNotVirtual);
		TEST_CASE(baseDerivedVirtual);
		TEST_CASE(baseDerivedNestVirtual);
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
        CheckBaseDestructor checker(&tokenizer, &settings, this);
        checker.checkBaseClass();
    }

    void baseDerivedNotVirtual() {
		check("class A{ public: A(){}; ~A(){}; void f(){};};\n"
			"class B : public A { public: B(){}; ~B(){}; void f2(){}; };\n"
			  );
        ASSERT_EQUALS("[test.cpp:2]: (performance) Destructor of base class: A is not virtual.\n", 
			errout.str());
    }

	void baseDerivedVirtual() {
		check("class A{ public: A(){}; virtual ~A(){}; void f(){};};\n"
			"class B : public A { public: B(){}; ~B(){}; void f2(){}; };\n"
			  );
        ASSERT_EQUALS("", errout.str());
    }

	void baseDerivedNestVirtual() {
		check("class A{ public: A(){}; virtual ~A(){}; void f(){};};\n"
			"class B : public A { public: B(){}; ~B(){}; void f2(){}; };\n"
			"class C : public B { public: C(){}; ~C(){}; void f2(){}; };\n"
			  );
        ASSERT_EQUALS("[test.cpp:3]: (performance) Destructor of base class: B is not virtual.\n", 
			errout.str());
    }
};

REGISTER_TEST(TestBaseDestructor)
