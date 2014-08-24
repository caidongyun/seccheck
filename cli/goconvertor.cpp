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

#include <string>
#include <locale>
#include <vector>

using namespace std;

static string convertVariableName(const string& varName)
{
	string firstCharUpper = varName;
	firstCharUpper[0] = toupper(firstCharUpper[0]);
	return firstCharUpper;
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
		return (varName) + " [] " + var.type()->name();
	}
	else if (var.isPointer())
	{
		return (varName) + " * " + var.type()->name();
	}
	else
	{
		return (varName) + " " + var.type()->name();
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
	funcDef += "){";
	// TODO function return value

	statements.push_back(funcDef);

	// TODO Function content

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

static string convertFunctionScope(const Scope& sc)
{
	return "";
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

GoConvertor::GoConvertor(Tokenizer* ptr) : tokenizer_(ptr)
{
}

// See: SymbolDatabase::printOut
void GoConvertor::convert()
{
	const SymbolDatabase* pDb = tokenizer_->getSymbolDatabase();
	for (auto scope = pDb->scopeList.begin(); scope != pDb->scopeList.end(); ++scope)
	{
		switch (scope->type)
		{
		case Scope::eGlobal:
			break;
		case Scope::eClass:
			convertClassScope(*scope);
			break;
		case Scope::eStruct:
			convertStructScope(*scope);
			break;
		case Scope::eUnion:
			break;
		case Scope::eNamespace:
			convertNamespaceScope(*scope);
			break;
		case Scope::eFunction:
			convertFunctionScope(*scope);
			break;
		case Scope::eIf:
			convertIfScope(*scope);
			break;
		case Scope::eElse:
			convertElseScope(*scope);
			break;
		case Scope::eFor:
			convertForScope(*scope);
			break;
		case Scope::eWhile:
			convertWhileScope(*scope);
			break;
		case Scope::eDo:
			convertDoScope(*scope);
			break;
		case Scope::eSwitch:
			convertSwitchScope(*scope);
			break;
		case Scope::eUnconditional:
			break;
		case Scope::eTry:
			convertTryScope(*scope);
			break;
		case Scope::eCatch:
			break;
		default:
			break;
		}
	}

}

void GoConvertor::convertScope(const Scope* scopePtr)
{

}
