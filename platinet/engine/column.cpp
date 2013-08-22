/*
 * Copyright 2011, 2012, 2013 salvatore barone <salvator.barone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "column.hpp"
using namespace engine;

column::column (std::string columnName, sqlType columnType, unsigned length, bool isKey, std::string defaultValue)	throw () :
	__columnName(columnName), __columnType(columnType), __length(length), __isKey(isKey), __defaultValue(defaultValue),
	__project(false), __select(false), __selectValue(""), __compareOperator(equal), __orderBy(false), __orderMode(asc) {

	if (__columnType != varchar)
	    __length = getLength();

	}

unsigned column::getLength() const throw ()	{
	switch (__columnType) {
		case boolean : 		return 5;
		case integer:
		case bigint : 		return 11;
		case smallint : 	return 6;
		case real : 		return 32;
		case doublePrecision : return 64;
		case date : 		return 10;
		case time : 		return 8;
		default : 		return 0;
	}
}
