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
    //std::map<std::string, std::string> _unsafeIntegerFunctions;

    /** init unsafe functions list ' */
    void initUnsafeFunctions() {

		const char* unsafe_int_function_tips = "[CERT INT06-CPP] string token to integer";
		const char* raw_memory_function_tips = "[CERT MEM08-CPP] Raw memory";
		const char* cstyle_fio_function_tips = "[CERT FIO17-CPP] C-style input and output";
        // Unsafe functions, which messages suggest only one alternative and doesn't contain additional information.
        const struct {
            const char* bad;
            const char* good;
			const char* prefix;
        } unsafe_stdmsgs[] = {
            {"strcpy", "strncpy", "Obsolete"},
			{"wcscpy", "wcsncpy", "Obsolete"},
            {"strcat", "strncat", "Obsolete"},
            {"sprintf", "snprintf", "Obsolete"},
			{"vsprintf", "vsnprintf", "Obsolete"},
			{"rewind", "fseek", "[CERT FIO07-CPP] Unsafe"},
			{"fopen", "fopen_s", "[CERT FIO06-CPP] Unsafe create file"},
			{"malloc", "new and delete", raw_memory_function_tips},
			{"calloc", "new and delete", raw_memory_function_tips},
			{"realloc", "new and delete", raw_memory_function_tips},
			{"free", "new and delete", raw_memory_function_tips},
			{"fprintf", "C++ streams", cstyle_fio_function_tips},
			{"fscanf", "C++ streams", cstyle_fio_function_tips},
			{"printf", "C++ streams", cstyle_fio_function_tips},
			{"scanf", "C++ streams", cstyle_fio_function_tips},
			{"system", "POSIX execve or other exec family", "[CERT ENV04-CPP] Unsafe C99 Command interpreter"},
			{"atol", "strtol", unsafe_int_function_tips},
			{"atoi", "strtol", unsafe_int_function_tips},
            {"atoll", "strtoll", unsafe_int_function_tips},
			{"sscanf", "strtoll", unsafe_int_function_tips},
			{"scanf", "strtoll", unsafe_int_function_tips},
			{"fscanf", "strtoll", unsafe_int_function_tips}
        };

		for (std::size_t i = 0; i < (sizeof(unsafe_stdmsgs) / sizeof(*unsafe_stdmsgs)); ++i) {
			_unsafeFunctions[unsafe_stdmsgs[i].bad] = std::string(unsafe_stdmsgs[i].prefix) 
				+ " function '" + std::string(unsafe_stdmsgs[i].bad) 
				+ "' called. It is recommended to use the function '" 
				+ unsafe_stdmsgs[i].good + "' instead.";
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
