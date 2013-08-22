/*
 * Copyright 2011, 2012, 2013 salvatore barone <salvator.barone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _EXCEPTION_
#define _EXCEPTION_

#include <string>

namespace engine {
class exception {
public :
	enum eCode {noSchema, structural, noTable, noColumn, noStructure, keyNull, noID, fileCreation, fileOpening, ioError};
	exception(enum eCode _code, std::string _what) : __code(_code), __what(_what) {}

	enum eCode	code() 	const	{return __code;}
	std::string	what()	const	{return __what;}

private:
	enum eCode	__code;
	std::string	__what;
};
};
#endif
