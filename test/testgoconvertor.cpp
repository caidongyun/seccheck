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
#include "goconvertor.h"
#include "testsuite.h"
#include "settings.h"

#include <sstream>

using namespace std;

extern std::ostringstream errout;

class TestGoConvertor : public TestFixture {
public:
    TestGoConvertor() : TestFixture("TestGoConvertor") {
    }

private:
    void run() {
        TEST_CASE(simpleForClause);
    }

    void convert(const char code[]) {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.addEnabled("performance");

        // Tokenize..
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(code);
        tokenizer.tokenize(istr, "test.cpp");
		
		GoConvertor goconv(&tokenizer);
		std::string res = goconv.convert();
    }

    void simpleForClause() {
		convert("int main() { int sum = 0; for (int i = 0; i < 50; i++) { sum+=2;}; return sum;}\n"
			  );
    }
};

REGISTER_TEST(TestGoConvertor)
