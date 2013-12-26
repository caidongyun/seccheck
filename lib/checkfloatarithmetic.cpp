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

#include "checkfloatarithmetic.h"
#include "symboldatabase.h"

//---------------------------------------------------------------------------


// Register this check class (by creating a static instance of it)
namespace {
    CheckFloatArithmetic instance;
}

/** Is given variable an float variable */
static bool isFloat(const Variable *var)
{
	if (var == 0)
	{
		return false;
	}

	const std::string FLOAT_DEFINE = "float|double|long double";
	return FLOAT_DEFINE.find(var->nameToken()->previous()->str()) != std::string::npos;
}

void CheckFloatArithmetic::floatEqualsError(const Token *tok)
{
	reportError(tok, Severity::warning,
                "FloatEqualsError",
                "Compare two float variables is improper.\n"
				"Should avoid compare two float variables directly. "
				"Maybe you can compare with an epsilon value."
				"Please see: http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison ");
}

static bool isFloatVariable(const Token* tok)
{
	return (tok->type() == Token::eVariable) && isFloat(tok->variable());
}

static bool isFloatComparison(const Token* tok)
{
	Token* prev = tok->previous();
	Token* next = tok->next();
	if ((prev == 0) || (next == 0)) {
		return false;
	}

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

void CheckFloatArithmetic::improperArithmetic()
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
			if (tok->type() != Token::eComparisonOp)
			{
				continue;
			}

			if (isFloatComparison(tok))
			{
				floatEqualsError(tok);
			}
        }
    }
}
//---------------------------------------------------------------------------
