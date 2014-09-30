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
// Unsafe functions
//---------------------------------------------------------------------------

#include "checkmiscellaneous.h"
#include "symboldatabase.h"
#include <unordered_set>

using namespace std;

//---------------------------------------------------------------------------


// Register this check class (by creating a static instance of it)
namespace {
    CheckMiscellaneous instance;
}

class FloatDefine
{
public:
	FloatDefine()
	{
		floatDef.insert("float");
		floatDef.insert("double");
		floatDef.insert("long double");
	}

	bool isFloat(const string& varDef) const
	{
		return floatDef.find(varDef) != floatDef.end();
	}

private:
	unordered_set<string> floatDef;
};

/** Is given variable an float variable */
static bool isFloat(const Variable *var)
{
	if (var == 0)
	{
		return false;
	}

	static FloatDefine fd;
	const string& varDef = var->nameToken()->previous()->str();
	return fd.isFloat(varDef);
}

/** Is given variable an time_t variable */
static bool isTimeT(const Variable *var)
{
	if (var == 0)
	{
		return false;
	}

	static FloatDefine fd;
	const string& varDef = var->nameToken()->previous()->str();
	return "time_t" == (varDef);
}

void CheckMiscellaneous::floatEqualsError(const Token *tok)
{
	reportError(tok, Severity::warning,
                "FloatEqualsError",
                "Comparing two float variables is improper.\n"
				"Should avoid compare two float variables directly. "
				"Maybe you can compare with an epsilon value."
				"Please see: http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison ");
}

// https://www.securecoding.cert.org/confluence/display/cplusplus/MSC05-CPP.+Do+not+manipulate+time_t+typed+values+directly
void CheckMiscellaneous::timetOperError(const Token *tok)
{
	reportError(tok, Severity::warning,
                "time_tArithmeticError",
                "There is no safe way to manually perform arithmetic on the time_t type.\n"
				"The time_t values should not be modified directly. "
				"Please see: CERT C++ Secure Coding Standard  49. Miscellaneous (MSC) "
                "MSC05-CPP. Do not manipulate time_t typed values directly. ");
}

static bool isFloatVariable(const Token* tok)
{
    if (tok == 0)
    {
        return false;
    }

	return (tok->type() == Token::eVariable) && isFloat(tok->variable());
}

static bool isTimeTVariable(const Token* tok)
{
    if (tok == 0)
    {
        return false;
    }

	return (tok->type() == Token::eVariable) && isTimeT(tok->variable());
}

static bool isFloatComparison(const Token* tok)
{
	Token* prev = tok->previous();
	Token* next = tok->next();

	if (isFloatVariable(prev))
	{
		if (next->type() == Token::eNumber)
		{
			// Compare variable to constant value
			return true;
		}
		else if (isFloatVariable(next))
		{
			// Both token are floating variables 
			return true;
		}

		return false;
	}

	if (isFloatVariable(next))
	{
		if (prev->type() == Token::eNumber)
		{
			// Compare variable to constant value
			return true;
		}
		else if (isFloatVariable(prev))
		{
			// Both token are floating variables 
			return true;
		}

		return false;
	}
	
	return false;
}

void CheckMiscellaneous::improperArithmetic()
{
	// TODO
	//if (!_settings->isEnabled("portability"))
    //    return;

    const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();

    // Check return values
    const std::size_t functions = symbolDatabase->functionScopes.size();

	// Check assignments
    for (std::size_t i = 0; i < functions; ++i) {
        const Scope * scope = symbolDatabase->functionScopes[i];
        for (const Token *tok = scope->classStart; tok && tok != scope->classEnd; tok = tok->next()) {
			if (tok->type() == Token::eComparisonOp)
			{
				if (tok->str() != "==")
			    {
				    continue;
			    }

			    if (isFloatComparison(tok))
			    {
				    floatEqualsError(tok);
			    }
			}
            else if (tok->type() == Token::eArithmeticalOp)
            {
                if (isTimeTVariable(tok->next()) || isTimeTVariable(tok->previous()))
                {
                    timetOperError(tok);
                }
            }
			
        }
    }
}
//---------------------------------------------------------------------------
