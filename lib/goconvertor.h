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

#ifndef GOCONVERTOR_H_1dzfes
#define GOCONVERTOR_H_1dzfes

#include <string>
#include <vector>

class Tokenizer;
class Scope;

class GoConvertor
{
public:
	GoConvertor(const Tokenizer* const ptr);

	void convert();

private:
	std::string convertScope(const Scope& scope);

private:
	const Tokenizer* tokenizer_;
};

#endif // PATHMATCH_H