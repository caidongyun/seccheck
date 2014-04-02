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
#ifndef checkunsafefunctionsH
#define checkunsafefunctionsH
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

class CPPCHECKLIB CheckUnsafeFunctions : public Check {
public:
    /** This constructor is used when registering the CheckUnsafeFunctions */
    CheckUnsafeFunctions() : Check(myName()) {
        initUnsafeFunctions();
    }

    /** This constructor is used when running checks. */
    CheckUnsafeFunctions(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger) {
        initUnsafeFunctions();
    }

    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger) {
        CheckUnsafeFunctions checkUnsafeFunctions(tokenizer, settings, errorLogger);
        checkUnsafeFunctions.unsafeFunctions();
    }

    /** Check for unsafe functions */
    void unsafeFunctions();

private:
    /* function name / error message */
    std::map<std::string, std::string> _unsafeFunctions;
    std::map<std::string, std::string> _unsafeStandardFunctions;

    /** init unsafe functions list ' */
    void initUnsafeFunctions() {
        // Unsafe functions, which messages suggest only one alternative and doesn't contain additional information.
        const struct {
            const char* bad;
            const char* good;
        } unsafe_stdmsgs[] = {
            {"strcpy", "strncpy"},
			{"wcscpy", "wcsncpy"},
            {"strcat", "strncat"},
            {"sprintf", "snprintf"},
			{"vsprintf", "vsnprintf"}
        };

		for (std::size_t i = 0; i < (sizeof(unsafe_stdmsgs) / sizeof(*unsafe_stdmsgs)); ++i) {
            _unsafeFunctions[unsafe_stdmsgs[i].bad] = "Obsolete function '" + std::string(unsafe_stdmsgs[i].bad) 
				+ "' called. It is recommended to use the function '" + unsafe_stdmsgs[i].good + "' instead.";
        }
    }

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        CheckUnsafeFunctions c(0, settings, errorLogger);

        auto it(_unsafeFunctions.begin()), itend(_unsafeFunctions.end());
        for (; it!=itend; ++it) {
            c.reportError(0, Severity::style, "unsafeFunctions"+it->first, it->second);
        }
    }

    static std::string myName() {
        return "Unsafe functions";
    }

    std::string classInfo() const {
        std::string info = "Warn if any of these unsafe functions are used:\n";
        auto it(_unsafeFunctions.begin()), itend(_unsafeFunctions.end());
        for (; it!=itend; ++it) {
            info += "* " + it->first + "\n";
        }
        return info;
    }

private:
	bool isWinExecuteFunction(const Token* tok);
};
/// @}
//---------------------------------------------------------------------------
#endif // checkunsafefunctionsH
