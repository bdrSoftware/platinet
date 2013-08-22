/*
 * Copyright 2011, 2012, 2013 Salvatore Barone <salvator.barone@gmail.com>
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

#include "tuple.hpp"
using namespace engine;

tuple::tuple(const std::list <std::string>& rowValue,  const std::list <column>& structure, enum state rowState) throw (exception&) : __state(rowState), __visible(true) {
	_insert(rowValue, __value, structure);
}

void tuple::update (const std::list <std::string>& newValue, const std::list <column>& structure) throw () {
	switch (__state) {
	case loaded :
		__old = __value;
		__state = updating;
		_update(newValue, __value, structure);
		break;

	case empty :
	case inserting :
		_insert(newValue, __value, structure);
		break;

	case updating :
		_update(newValue, __value, structure);
		break;

	case deleting :
		break;
	}
}

void tuple::cancel () throw () {
	__state = deleting;
	__visible = false;
}

void tuple::erase () throw () {
	__value.clear();
	__old.clear();
}

void tuple::value (std::unique_ptr<std::list <std::string>>& ptr) const throw () {
	ptr = std::move(std::unique_ptr<std::list <std::string>>(new std::list <std::string>));
	for (std::list <std::string>::const_iterator it = __value.begin(); it != __value.end(); it++)
		ptr -> push_back(*it);
}

void tuple::old (std::unique_ptr<std::list <std::string>>& ptr) const throw () {
	ptr = std::move(std::unique_ptr<std::list <std::string>>(new std::list <std::string>));
	for (std::list <std::string>::const_iterator it = __old.begin(); it != __old.end(); it++)
		ptr -> push_back(*it);
}

std::unique_ptr<std::list <std::string>> tuple::value () const throw () {
	std::unique_ptr<std::list <std::string>> ptr (new std::list <std::string>);
	for (std::list <std::string>::const_iterator it = __value.begin(); it != __value.end(); it++)
		ptr -> push_back(*it);
	return ptr;
}

std::unique_ptr<std::list <std::string>> tuple::old () const throw () {
	std::unique_ptr<std::list <std::string>> ptr (new std::list <std::string>);
	for (std::list <std::string>::const_iterator it = __old.begin(); it != __old.end(); it++)
		ptr -> push_back(*it);
	return ptr;
}

std::streamoff tuple::sizeOnFile () const throw () {
	unsigned long size = sizeof(__state) + sizeof(bool) + 2 * sizeof(unsigned);
	for (std::list <std::string>::const_iterator it = __value.begin(); it != __value.end(); it++)
		size += sizeof(unsigned) + (*it).size();
	for (std::list <std::string>::const_iterator it = __old.begin(); it != __old.end(); it++)
		size += sizeof(unsigned) + (*it).size();
	return (std::streamoff) size;
}

void tuple::write (std::fstream& stream) const throw () {
	stream.write(reinterpret_cast<const char*> (&__state), sizeof(enum state));
	stream.write(reinterpret_cast<const char*> (&__visible), sizeof(bool));
	write_list(stream, __value);
	write_list(stream, __old);
}

void tuple::read (std::fstream& stream) throw () {
	stream.read(reinterpret_cast<char*> (&__state), sizeof(enum state));
	stream.read(reinterpret_cast<char*> (&__visible), sizeof(bool));
	read_list(stream, __value);
	read_list(stream, __old);
}

void tuple::_insert (const std::list <std::string>& from, std::list <std::string>& to, const std::list <column>& structure) const throw (exception&) {
	std::list <std::string>::const_iterator from_it = from.begin();
	std::list <column>::const_iterator col_it = structure.begin();
	to.clear();
	while (col_it != structure.end()) {
		if (col_it -> isKey() && ((from_it != from.end() && *from_it == "") || from_it == from.end()))	throw exception(exception::keyNull, "column's key must not be empty or an empty string");

		if (from_it != from.end()) {
			std::string tmp = *from_it;
			if (col_it -> columnType() == column::varchar)
				tmp = tmp.substr(0, col_it -> length());
			to.push_back(tmp);
		}
		else
			to.push_back("");

		if (from_it != from.end())
			from_it++;
		col_it++;
	}
}

void tuple::_update (const std::list <std::string>& from, std::list <std::string>& to, const std::list <column>& structure) const throw () {
	std::list <std::string>::const_iterator from_it = from.begin();
	std::list <std::string>::iterator to_it = to.begin();
	std::list <column>::const_iterator col_it = structure.begin();
	while (col_it != structure.end()) {
		if (!col_it -> isKey()) {
			if (from_it != from.end()) {
				std::string tmp = *from_it;
				if (col_it -> columnType() == column::varchar)
					tmp = tmp.substr(0, col_it -> length());
				*to_it = tmp;
			}
			else
				*to_it = "";
		}
		if (from_it != from.end())
			from_it++;
		to_it++;
		col_it++;
	}
}

void tuple::write_string (std::fstream& stream, const std::string& _string) const throw () {
	unsigned length;
	length = _string.size();
	stream.write(reinterpret_cast <const char*> (&length), sizeof (unsigned));
	stream.write(_string.c_str(), length);
}

void tuple::read_string (std::fstream& stream, std::string& _string) const throw () {
	unsigned length;
	char* tmp;
	stream.read(reinterpret_cast <char*> (&length), sizeof (unsigned));
	tmp = new char [length + 1];
	stream.read(tmp, length);
	_string = std::string (tmp, length);
	delete [] tmp;
}

void tuple::write_list (std::fstream& stream, const std::list <std::string>& _list) const throw () {
	unsigned elementNumber = _list.size();
	stream.write(reinterpret_cast<const char*> (&elementNumber), sizeof(unsigned));
	for (std::list <std::string>::const_iterator it = _list.begin(); it != _list.end(); it++)
		write_string(stream, *it);
}

void tuple::read_list (std::fstream& stream, std::list <std::string>& _list) const throw () {
	unsigned elementNumber;
	stream.read(reinterpret_cast <char*> (&elementNumber), sizeof(unsigned));
	std::string _string;
	for (unsigned i = 0; i < elementNumber; i++) {
		read_string(stream, _string);
		_list.push_back(_string);
	}
}
