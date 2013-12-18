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
    std::map<std::string, std::string> _unsafeStandardFunctions;
    std::map<std::string, std::string> _unsafePosixFunctions;
    std::map<std::string, std::string> _unsafeC99Functions;

    /** init unsafe functions list ' */
    void initUnsafeFunctions() {
        // Unsafe posix functions, which messages suggest only one alternative and doesn't contain additional information.
        const struct {
            const char* bad;
            const char* good;
        } posix_stdmsgs[] = {
            {"bsd_signal", "sigaction"},
            {"gethostbyaddr", "getnameinfo"},
            {"gethostbyname", "getaddrinfo"},
            {"bcmp", "memcmp"},
            {"bzero", "memset"},
            {"ecvt", "sprintf"},
            {"fcvt", "sprintf"},
            {"gcvt", "sprintf"},
            {"getwd", "getcwd"},
            {"index", "strchr"}, // See #2334 (using the Qt Model/View function 'index')
            {"rindex", "strrchr"},
            {"pthread_attr_getstackaddr", "pthread_attr_getstack"},
            {"pthread_attr_setstackaddr", "pthread_attr_setstack"},
            {"vfork", "fork"},
            {"wcswcs", "wcsstr"},
            {"rand_r", "rand"},
            {"utime", "utimensat"},
            {"asctime_r", "strftime"},
            {"ctime_r", "strftime"}
        };

        for (std::size_t i = 0; i < (sizeof(posix_stdmsgs) / sizeof(*posix_stdmsgs)); ++i) {
            _unsafePosixFunctions[posix_stdmsgs[i].bad] = "Unsafe function '" + std::string(posix_stdmsgs[i].bad) + "' called. It is recommended to use the function '" + posix_stdmsgs[i].good + "' instead.";
        }

        _unsafePosixFunctions["usleep"] = "Unsafe function 'usleep' called. It is recommended to use the 'nanosleep' or 'setitimer' function instead.\n"
                                            "The unsafe function 'usleep' is called. POSIX.1-2001 declares usleep() function unsafe and POSIX.1-2008 removes it. It is recommended that new applications use the 'nanosleep' or 'setitimer' function.";

        _unsafePosixFunctions["bcopy"] = "Unsafe function 'bcopy' called. It is recommended to use the 'memmove' or 'memcpy' function instead.";

        _unsafePosixFunctions["ftime"] = "Unsafe function 'ftime' called. It is recommended to use time(), gettimeofday() or clock_gettime() instead.";

        _unsafePosixFunctions["getcontext"] = "Unsafe function 'getcontext' called. Due to portability issues, applications are recommended to be rewritten to use POSIX threads.";
        _unsafePosixFunctions["makecontext"] = "Unsafe function 'makecontext' called. Due to portability issues, applications are recommended to be rewritten to use POSIX threads.";
        _unsafePosixFunctions["swapcontext"] = "Unsafe function 'swapcontext' called. Due to portability issues, applications are recommended to be rewritten to use POSIX threads.";

        _unsafePosixFunctions["scalbln"] = "Unsafe function 'scalb' called. It is recommended to use 'scalbln', 'scalblnf' or 'scalblnl' instead.";

        _unsafePosixFunctions["ualarm"] = "Unsafe function 'ualarm' called. It is recommended to use 'timer_create', 'timer_delete', 'timer_getoverrun', 'timer_gettime' or 'timer_settime' instead.";

        _unsafePosixFunctions["tmpnam"] = "Unsafe function 'tmpnam' called. It is recommended to use 'tmpfile', 'mkstemp' or 'mkdtemp' instead.";

        _unsafePosixFunctions["tmpnam_r"] = "Unsafe function 'tmpnam_r' called. It is recommended to use 'tmpfile', 'mkstemp' or 'mkdtemp' instead.";

        _unsafeStandardFunctions["gets"] = "Unsafe function 'gets' called. It is recommended to use the function 'fgets' instead.\n"
                                             "The unsafe function 'gets' is called. With 'gets' you'll get a buffer overrun if the input data exceeds the size of the buffer. It is recommended to use the function 'fgets' instead.";
        _unsafeC99Functions["alloca"] = "Unsafe function 'alloca' called. In C99 and later it is recommended to use a variable length array instead.\n"
                                          "The unsafe function 'alloca' is called. In C99 and later it is recommended to use a variable length array or a dynamically allocated array instead. The function 'alloca' is dangerous for many reasons (http://stackoverflow.com/questions/1018853/why-is-alloca-not-considered-good-practice and http://linux.die.net/man/3/alloca).";
        _unsafeC99Functions["asctime"] = "Unsafe function 'asctime' called. It is recommended to use the function 'strftime' instead.";
        // ctime is unsafe - it's not threadsafe. but there is no good replacement.
        //_unsafeC99Functions["ctime"] = "Unsafe function 'ctime' called. It is recommended to use the function 'strftime' instead.";
    }

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings) const {
        CheckUnsafeFunctions c(0, settings, errorLogger);

        std::map<std::string,std::string>::const_iterator it(_unsafePosixFunctions.begin()), itend(_unsafePosixFunctions.end());
        for (; it!=itend; ++it) {
            c.reportError(0, Severity::style, "unsafeFunctions"+it->first, it->second);
        }
    }

    static std::string myName() {
        return "Unsafe functions";
    }

    std::string classInfo() const {
        std::string info = "Warn if any of these unsafe functions are used:\n";
        std::map<std::string,std::string>::const_iterator it(_unsafePosixFunctions.begin()), itend(_unsafePosixFunctions.end());
        for (; it!=itend; ++it) {
            info += "* " + it->first + "\n";
        }
        return info;
    }
};
/// @}
//---------------------------------------------------------------------------
#endif // checkunsafefunctionsH
