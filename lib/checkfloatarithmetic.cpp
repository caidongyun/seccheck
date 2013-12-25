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
    return (var && Token::Match(var->nameToken()->previous(), "float|double|long double %var% !!["));
}

void CheckFloatArithmetic::floatEqualsError(const Token *tok)
{
	reportError(tok, Severity::warning,
                "FloatEqualsError",
                "Compare two float variables is improper.\n");
}

inline bool isFloatComparison(const Token* tok)
{
	Token* prev = tok->previous();
	Token* next = tok->next();
	if ((prev == 0) || (next == 0)) {
		return false;
	}


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

			if (tok->previous()->type() == Token::eVariable) {
			}
            if (Token::Match(tok, "[;{}] %var% == %var% [;+]")) {

                const Variable *var1(tok->next()->variable());
                const Variable *var2(tok->tokAt(3)->variable());

                if (isFloat(var1) && isFloat(var2))
                    floatEqualsError(tok->next());

				/*
                else if (isint(var1) && isaddr(var2) && !tok->tokAt(3)->isPointerCompare()) {
                    // assigning address => warning
                    // some trivial addition => warning
                    if (Token::Match(tok->tokAt(4), "+ %any% !!;"))
                        continue;

                    assignmentAddressToIntegerError(tok->next());
                }
				*/
            }
        }
    }
}
//---------------------------------------------------------------------------
