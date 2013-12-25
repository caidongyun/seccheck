/*
 * Seccheck - A tool for security C/C++ code analysis
 * Copyright (C) 2013 Wang Anyu
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

//---------------------------------------------------------------------------
#ifndef checkFloatArithmeticH
#define checkFloatArithmeticH
//---------------------------------------------------------------------------

#include "config.h"
#include "check.h"
#include <string>
#include <map>


/// @addtogroup Checks
/// @{

/**
 * @brief Using unsafe functions that are always insecure to use.
 */

class CPPCHECKLIB CheckFloatArithmetic : public Check {
public:
    /** This constructor is used when registering the CheckFloatArithmetic */
    CheckFloatArithmetic() : Check(myName()) {
    }

    /** This constructor is used when running checks. */
    CheckFloatArithmetic(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
    }

    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        CheckFloatArithmetic c(tokenizer, settings, errorLogger);
        c.improperArithmetic();
    }

    /** Check for improper floating arithmetic
	* See CERT C++ Secure Coding Standard:
	* FLP00-CPP. Understand the limitations of floating-point numbers
	*/
    void improperArithmetic();

private:
	void floatEqualsError(const Token *tok);

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        CheckFloatArithmetic c(0, settings, errorLogger);

		c.floatEqualsError(0);
    }

    static std::string myName() {
        return "Improper floating arithmetic (CERT FLP00-CPP)";
    }

	std::string classInfo() const {
        return "Check if there is improper floating arithmetic issues:\n"
               "* float variable equals\n"
               "* and others\n";
    }
};
/// @}
//---------------------------------------------------------------------------
#endif // checkunsafefunctionsH
