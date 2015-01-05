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
 * @brief Check improper float comparisons
 */

class CPPCHECKLIB CheckMiscellaneous : public Check {
public:
    /** This constructor is used when registering the CheckMiscellaneous */
    CheckMiscellaneous() : Check(myName()) {
    }

    /** This constructor is used when running checks. */
    CheckMiscellaneous(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
    }

    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        CheckMiscellaneous c(tokenizer, settings, errorLogger);
        c.runChecks();
    }
        
    void runChecks();

private:
	void floatEqualsError(const Token *tok);
    void timetOperError(const Token *tok);
    void SignedBitOperError(const Token *tok);
    void SignedCharError(const Token *tok);
    void modifyStdError(const Token *tok);

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        CheckMiscellaneous c(0, settings, errorLogger);

		c.floatEqualsError(0);
    }

    static std::string myName() {
        return "Improper arithmetic (CERT FLP00-CPP and MSC05-CPP)";
    }

	std::string classInfo() const {
        return "Check if there is improper arithmetic issues:\n"
               "* variable equals\n"
               "* and others\n";
    }
};
/// @}
//---------------------------------------------------------------------------
#endif // checkFloatArithmeticH
