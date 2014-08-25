/*
* Cppcheck - A tool for static C/C++ code analysis
* Copyright (C) 2007-2014 Daniel Marjamäki and Cppcheck team.
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

#include "goconvertor.h"
#include "tokenize.h"
#include "symboldatabase.h"

#include <iostream>
#include <locale>
#include <vector>

using namespace std;

static string convertVariableName(const string& varName)
{
	string firstCharUpper = varName;
	firstCharUpper[0] = toupper(firstCharUpper[0]);
	return firstCharUpper;
}

static string findVariableType(const Variable& var)
{
	if (var.type() != nullptr)
	{
		return var.type()->name();
	}

	if (var.typeStartToken() != nullptr)
	{
		return var.typeStartToken()->str();
	}

	// TODO
	return "NA";
}

static string convertVariableDefine(const Variable& var)
{
	string varName = var.name();

	if (var.isPublic())
	{
		varName = convertVariableName(varName);
	}

	if (var.isArray())
	{
		return (varName) + " [] " + findVariableType(var);
	}
	else if (var.isPointer())
	{
		return (varName) + " * " + findVariableType(var);
	}
	else
	{
		return (varName) + " " + findVariableType(var);
	}	
}

static string array2string(const vector<string>& arr)
{
	string statement = "";
	for (auto itr = arr.begin(); itr != arr.end(); itr++)
	{
		statement += *itr + "\r\n";
	}
	return statement;
}

static string findReturnValue(const Function& func)
{
	if (func.retType != nullptr)
	{
		return func.retType->name();
	}

	if (func.retDef != nullptr)
	{
		return func.retDef->str();
	}

	return "";
}

static string convertFunctionContent(const Scope& sc)
{
	string line = "";
	for (const Token* ftok2 = sc.classStart; ftok2 != sc.classEnd; ftok2 = ftok2->next())
	{
		// TODO parse content
		line += ftok2->str() + " ";
	}
	return line;
}

static string convertNormalFunctionDefine(const Function& func)
{
	vector<string> statements;

	string funcDef = "";
	if (func.nestedIn->type == Scope::ScopeType::eClass || func.nestedIn->type == Scope::ScopeType::eStruct)
	{
		funcDef = ("func (parent * " + func.nestedIn->className + " ) " + func.name() + "(");
	}
	else
	{
		funcDef = ("func " + func.name() + "(");
	}

	for (auto itr = func.argumentList.begin(); itr != func.argumentList.end(); ++itr)
	{
		funcDef += convertVariableDefine(*itr) + ",";
	}

	funcDef = funcDef.substr(0, funcDef.size() - 1); // Remove last "," 
	funcDef += ") " + findReturnValue(func) + " { "; // function return value

	statements.push_back(funcDef);

	// Function content
	string content = convertFunctionContent(*func.functionScope);
	if (content != "")
	{
		statements.push_back(content);
	}

	statements.push_back("}");

	return array2string(statements);
}

static string convertFunctionDefine(const Function& func)
{
	switch (func.type)
	{
	case Function::eConstructor:
		break;
	case Function::eCopyConstructor:
		break;
	case Function::eMoveConstructor:
		break;
	case Function::eOperatorEqual:
		break;
	case Function::eDestructor:
		break;
	case Function::eFunction:
		return convertNormalFunctionDefine(func);
	}
	return "";
}

static string convertGlobalScope(const Scope& sc)
{
	return "";
}

static string convertStructScope(const Scope& sc)
{
	string classDef = "struct " + sc.className + "{\r\n";

	// Added variable members
	for (auto var = sc.varlist.begin(); var != sc.varlist.end(); ++var)
	{
		classDef += convertVariableDefine(*var) + "\r\n";
	}

	classDef += "}\r\n";

	// Added functions definition
	for (auto func = sc.functionList.begin(); func != sc.functionList.end(); ++func)
	{
		classDef += convertFunctionDefine(*func) + "\r\n";
	}

	return classDef;
}

static string convertClassScope(const Scope& sc)
{
	return convertStructScope(sc);
}

static string convertNamespaceScope(const Scope& sc)
{
	return "";
}

static string convertIfScope(const Scope& sc)
{
	return "";
}

static string convertElseScope(const Scope& sc)
{
	return "";
}

static string convertForScope(const Scope& sc)
{
	return "";
}

static string convertWhileScope(const Scope& sc)
{
	return "";
}

static string convertDoScope(const Scope& sc)
{
	return "";
}

static string convertSwitchScope(const Scope& sc)
{
	return "";
}

static string convertTryScope(const Scope& sc)
{
	return "";
}

static string convertCatchScope(const Scope& sc)
{
	return "";
}

GoConvertor::GoConvertor(const Tokenizer* const ptr) : tokenizer_(ptr)
{
}

// See: SymbolDatabase::printOut
void GoConvertor::convert()
{
	vector<string> contents;
	const SymbolDatabase* pDb = tokenizer_->getSymbolDatabase();
	for (auto scope = pDb->scopeList.begin(); scope != pDb->scopeList.end(); ++scope)
	{
		string content = convertScope(*scope);

		if (content.size() > 0)
		{
			contents.push_back(content);
		}
	}

	cout << array2string(contents) << endl;
}

string GoConvertor::convertScope(const Scope& scope)
{
	switch (scope.type)
	{
	case Scope::eGlobal:
		break;
	case Scope::eClass:
		return convertClassScope(scope);
	case Scope::eStruct:
		return convertStructScope(scope);
	case Scope::eUnion:
		break;
	case Scope::eNamespace:
		return convertNamespaceScope(scope);
	case Scope::eFunction:
		{
			if (scope.function == nullptr)
			{
				return "";
			}
			return convertNormalFunctionDefine(*scope.function);
		}
	case Scope::eIf:
		return convertIfScope(scope);
	case Scope::eElse:
		return convertElseScope(scope);
	case Scope::eFor:
		return convertForScope(scope);
	case Scope::eWhile:
		return convertWhileScope(scope);
	case Scope::eDo:
		return convertDoScope(scope);
	case Scope::eSwitch:
		return convertSwitchScope(scope);
	case Scope::eUnconditional:
		break;
	case Scope::eTry:
		return convertTryScope(scope);
	case Scope::eCatch:
		break;
	default:
		break;
	}

	return "";
}
