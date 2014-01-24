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

#include "checkbasedestructor.h"
#include "symboldatabase.h"

using namespace std;

//---------------------------------------------------------------------------


// Register this check class (by creating a static instance of it)
namespace 
{
	CheckBaseDestructor instance;
}

void CheckBaseDestructor::checkBaseClass()
{
	const SymbolDatabase *symbolDatabase = _tokenizer->getSymbolDatabase();

	// Iterate over all classes
	for (std::list<Type>::const_iterator classIt = symbolDatabase->typeList.begin();
		classIt != symbolDatabase->typeList.end();
		++classIt) 
	{
		// Iterate over the parent classes
		for (std::vector<Type::BaseInfo>::const_iterator parentClassIt = classIt->derivedFrom.begin();
			parentClassIt != classIt->derivedFrom.end();
			++parentClassIt) 
		{
			// Check if there is info about the 'Base' class
			const Type* parentType = parentClassIt->type;
			if (!parentType || !parentType->classScope)
			{
				continue;
			}

			// Check if they have a member variable in common
			for (std::list<Function>::const_iterator itr = parentType->classScope->functionList.begin();
				itr != parentType->classScope->functionList.end();
				++itr) 
			{
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

//---------------------------------------------------------------------------
