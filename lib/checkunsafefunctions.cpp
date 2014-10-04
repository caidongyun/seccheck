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
// Unsafe functions
//---------------------------------------------------------------------------

#include "checkunsafefunctions.h"
#include "symboldatabase.h"

//---------------------------------------------------------------------------


// Register this check class (by creating a static instance of it)
namespace {
    CheckUnsafeFunctions instance;
}

namespace {
static bool isNotMemberFunction(const Token* tok)
{
	return (!Token::Match(tok->previous(), ".|::") || Token::simpleMatch(tok->tokAt(-2), "std ::"));
}

/** Check for suspicious variable names. eg: 'password', 'pwd' */
class SuspiciousNames
{
public:
	SuspiciousNames()
	{
		_Containers.insert("password");
		_Containers.insert("pwd");		
		_Containers.insert("passwd");
	}

	bool isBadName(const std::string& para) const
	{
		return _Containers.find(para) != _Containers.end();
	}

private:
	std::set<std::string> _Containers;
};

static bool isSuspiciousName(const std::string& para)
{
	static SuspiciousNames names;
	return names.isBadName(para);
}
}

bool CheckUnsafeFunctions::isWinExecuteFunction(const Token* tok)
{
	// TODO
	return false;
}

void CheckUnsafeFunctions::unsafeFunctions()
{
    const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();

    // Functions defined somewhere?
    for (unsigned int i = 0; i < symbolDatabase->functionScopes.size(); i++) {
        const Scope* scope = symbolDatabase->functionScopes[i];
        _unsafeFunctions.erase(scope->className);
    }

    for (unsigned int i = 0; i < symbolDatabase->functionScopes.size(); i++) {
        const Scope* scope = symbolDatabase->functionScopes[i];
        for (const Token* tok = scope->classStart; tok != scope->classEnd; tok = tok->next()) {
			// Only check cpp file for unsafe functions
            if (_tokenizer->isCPP() 
				&& tok->isName() && tok->varId()==0 && (tok->next() && tok->next()->str() == "(") 
				&& isNotMemberFunction(tok)) {

                auto it = _unsafeFunctions.find(tok->str());
                if (it != _unsafeFunctions.end()) {
                    // If checking an old code base it might be uninteresting to update unsafe functions.
                    reportError(tok, Severity::style, "unsafeFunctions"+it->first, it->second);
                }
            }

			if ((tok->type() == Token::eVariable) && isSuspiciousName(tok->str())) {
				// 
                reportError(tok, Severity::style, 
					"suspiciousVariableName:"+tok->str(), "Suspicious variable name: "
					+ tok->str() + " maybe identify the hard-coded password.\n" 
					+ "Hard coded passwords are like backdoor access to the system, " 
					+ "so it should not be used.");
			}

			// Noncompliant Example (operator>>())
			// CERT INT06-CPP
			if (tok->isOp() && tok->astOperand1() && !Token::Match(tok, ">>")) {

			}
        }
    }
}

//---------------------------------------------------------------------------
