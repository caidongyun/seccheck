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

//---------------------------------------------------------------------------
#ifndef checkcomplexCopyingH
#define checkcomplexCopyingH
//---------------------------------------------------------------------------

#include "config.h"
#include "check.h"
#include <string>
#include <map>
#include <set>

/// @addtogroup Checks
/// @{

/**
 * @brief Checker for complex objects or containers as function parameters
 */

class CPPCHECKLIB CheckComplexCopying : public Check {
public:
    /** This constructor is used when registering the CheckComplexCopying */
    CheckComplexCopying() : Check(myName()) {
    }

    /** This constructor is used when running checks. */
    CheckComplexCopying(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
    }

    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        CheckComplexCopying checkComplexParams(tokenizer, settings, errorLogger);
        checkComplexParams.checkComplexParameters();
    }

    /** Check for complex objects or containers */
    void checkComplexParameters();

private:
	/** Check for complex objects as argument */
    void checkComplexParametersAsArgument(const Scope* scope);

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        CheckComplexCopying c(0, settings, errorLogger);
    }

    static std::string myName() {
        return "Complex parameters";
    }

    std::string classInfo() const {
        std::string info = "Warn if complex object copying:\n"
			"* Function parameters are complex classes or containers.\n"
			"* Complex classes or containers equation.\n";

        return info;
    }

	bool containerAsParam(const Token* tok) const;
};
/// @}
//---------------------------------------------------------------------------
#endif // checkcomplexparamsH
