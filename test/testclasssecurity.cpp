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
#include "checkclasssecurity.h"
#include "testsuite.h"
#include <sstream>

extern std::ostringstream errout;

class TestClassSecurity : public TestFixture {
public:
    TestClassSecurity() : TestFixture("TestClassSecurity") {
    }

private:
    void run() {
        TEST_CASE(baseDerivedNotVirtual);
		TEST_CASE(baseDerivedVirtual);
		TEST_CASE(baseDerivedNestVirtual);
		TEST_CASE(checkPrivateStaticFunc);
		TEST_CASE(checkDeleteThis);
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
        CheckClassSecurity checker(&tokenizer, &settings, this);
        checker.checkBaseClass();
		checker.checkPrivateStaticMembers();
		checker.checkDeleteThis();
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

	void checkPrivateStaticFunc() {
		check("class A{ public: A(){}; virtual ~A(){}; \n"
			"private: static void f1(){}; \n"
			"protected: static void f2(){}; };\n"
			  );
		std::string result = errout.str();
		ASSERT_EQUALS("[test.cpp:2]: (performance) Function: f1 is private static function.\n", 
			result);
	}

	void checkDeleteThis() {
		check("class A{ public: A(){}; virtual ~A(){}; \n"
			"protected: static void f1(){ delete this; }; \n"
			"protected: static void f2(){ char* p = new char(); delete p; }; };\n"
			  );
		std::string result = errout.str();
		ASSERT_EQUALS("[test.cpp:2]: (performance) Avoid use delete this statement.\n", 
			result);
	}
};

REGISTER_TEST(TestClassSecurity)
