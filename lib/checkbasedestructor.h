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
#ifndef CheckBaseDestructorH
#define CheckBaseDestructorH
//---------------------------------------------------------------------------

#include "config.h"
#include "check.h"
#include <string>
#include <map>
#include <set>

/// @addtogroup Checks
/// @{

/**
 * @brief Checker for destructor of base class (should be virtual) 
 */

class CPPCHECKLIB CheckBaseDestructor : public Check {
public:
    /** This constructor is used when registering the CheckBaseDestructor */
    CheckBaseDestructor() : Check(myName()) {
    }

    /** This constructor is used when running checks. */
    CheckBaseDestructor(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
    }

    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        CheckBaseDestructor checker(tokenizer, settings, errorLogger);
        checker.checkBaseClass();
    }

    /** Check for destructor of base class */
    void checkBaseClass();

private:
    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        CheckBaseDestructor c(0, settings, errorLogger);
    }

    static std::string myName() {
        return "Base destructor";
    }

    std::string classInfo() const {
        std::string info = "Warn if destructor of base class:\n"
			"* is not virtual.\n";

        return info;
    }
};
/// @}
//---------------------------------------------------------------------------
#endif // CheckBaseDestructorH
