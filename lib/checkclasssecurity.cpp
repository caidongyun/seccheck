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
// Base class's destructor is not virtual
//---------------------------------------------------------------------------

#include "checkclasssecurity.h"
#include "symboldatabase.h"

using namespace std;

//---------------------------------------------------------------------------


// Register this check class (by creating a static instance of it)
namespace 
{
	CheckClassSecurity instance;
}

/** Check for private static members of class */
/** See: MSC22-CPP. Do not define static private members */
void CheckClassSecurity::checkPrivateStaticMembers() 
{
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();

	// Iterate over all classes
	for (auto classIt = symbolDatabase->typeList.begin();
		classIt != symbolDatabase->typeList.end();
		++classIt) 
	{
		if (classIt->classScope == nullptr)
		{
			continue;
		}
		// Loop in all functions of the class
		for (auto itr = classIt->classScope->functionList.begin();
			itr != classIt->classScope->functionList.end();
			++itr)
		{
			if (itr->isStatic && (itr->access == Private))
			{
				// private and static function
				std::ostringstream errmsg;
				errmsg << "Function: " << itr->name() << " is private static function.\n"
					<< "The definition of private static function maybe questionable.";
				std::string errDesc = errmsg.str();
				reportError(itr->token, Severity::performance, 
					"privateStaticFunction", errDesc);
			}
		}
	}
}

void CheckClassSecurity::checkBaseClass()
{
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();

	// Iterate over all classes
	for (auto classIt = symbolDatabase->typeList.begin();
		classIt != symbolDatabase->typeList.end();
		++classIt) 
	{
		// Iterate over the parent classes
		for (auto parentClassIt = classIt->derivedFrom.begin();
			parentClassIt != classIt->derivedFrom.end();
			++parentClassIt) 
		{
			// Check if there is info about the 'Base' class
			const Type* parentType = parentClassIt->type;
			if (!parentType || !parentType->classScope)
			{
				continue;
			}

			// Loop in all functions of the base class
			for (auto itr = parentType->classScope->functionList.begin();
				itr != parentType->classScope->functionList.end();
				++itr) 
			{
				// Destructor is not virtual? 
				if (itr->isDestructor() && !itr->isVirtual) 
				{
					std::ostringstream errmsg;
					errmsg << "Destructor of base class: " << parentClassIt->name << " is not virtual.\n"
						<< "The destructor may be not called if it is not virtual. ";
					std::string errDesc = errmsg.str();
					reportError(parentClassIt->nameTok, Severity::performance, 
						"baseClassDestructor", errDesc);
				}
			}
		}
	}
}

void CheckClassSecurity::checkDeleteThis()
{
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();

	const std::size_t classes = symbolDatabase->classAndStructScopes.size();
    for (std::size_t i = 0; i < classes; ++i) {
        const Scope * scope = symbolDatabase->classAndStructScopes[i];

		for (auto func = scope->functionList.begin(); func != scope->functionList.end(); ++func)
		{
			checkDeleteThisInFunction(&(*func));
		}
	}
}

void CheckClassSecurity::checkDeleteThisInFunction(const Function *func)
{
	if (func->functionScope == nullptr)
	{
		return;
	}

	const Token *last = func->functionScope->classEnd;
    for (const Token *tok = func->functionScope->classStart; tok && (tok != last); tok = tok->next())
	{
		if (Token::Match(tok, "delete this"))
		{
			std::ostringstream errmsg;
					errmsg << "Avoid use delete this statement.\n"
						<< "use [delete this;] statement may cause error if accessing its members after delete. ";
					std::string errDesc = errmsg.str();
					reportError(tok, Severity::performance, 
						"avoidDeleteThis", errDesc);
		}
	}
}

//---------------------------------------------------------------------------
