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

#include "fStorage.hpp"
#include <cstdlib>
using namespace engine;

fStorage::fStorage (std::string fileName) throw (exception&) : storage(), __fileName(fileName + ".db"), __trashID(0) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::out);
	if(!file.is_open())
		throw exception(exception::fileCreation, "(file storage) Error: '" + __fileName + "' can not be created!");
	file.close();
}

void fStorage::internalID (std::unique_ptr<std::list<unsigned long>>& ptr) const throw () {
	ptr = std::move(std::unique_ptr<std::list<unsigned long>>(new std::list<unsigned long>));
	for (std::map <unsigned long, segment>::const_iterator it = __tupleMap.begin(); it != __tupleMap.end(); it++)
		ptr -> push_back(it -> first);
}

std::unique_ptr<std::list<unsigned long>> fStorage::internalID () const throw () {
	std::unique_ptr<std::list<unsigned long>> ptr(new std::list<unsigned long>);
	for (std::map <unsigned long, segment>::const_iterator it = __tupleMap.begin(); it != __tupleMap.end(); it++)
		ptr -> push_back(it -> first);
	return ptr;
}


void fStorage::clear () throw () {
	__tupleMap.clear();
	__trash.clear();
	__lastKey = 0;
	__trashID = 0;
}

unsigned long fStorage::insert (const std::list<std::string>& values, const std::list<column>& structure, enum tuple::state _status) throw (exception&) {
	tuple _tuple(values, structure, _status);
	unsigned long free_space = recycle(_tuple.sizeOnFile());
	if (free_space == 0)
		append(_tuple, __tupleMap[__lastKey+1]);
	else {
		__tupleMap[__lastKey + 1] = popTrash(free_space, _tuple.sizeOnFile());
		write(_tuple, __tupleMap[__lastKey + 1]);
	}
	return ++__lastKey;
}

void fStorage::update (unsigned long ID, const std::list<std::string>& values, const std::list<column>& structure) throw (exception&) {
	std::unique_ptr<tuple> tuple_ptr = getTuple(ID);
	tuple_ptr -> update(values, structure);
	pushTrash(__tupleMap[ID]);
	unsigned long free_space = recycle(tuple_ptr -> sizeOnFile());
	if (free_space == 0)
		append(*tuple_ptr, __tupleMap[ID]);
	else {
		__tupleMap[ID] = popTrash(free_space, tuple_ptr->sizeOnFile());
		write(*tuple_ptr, __tupleMap[ID]);
	}
}

void fStorage::cancel (unsigned long ID) throw (exception&) {
	std::unique_ptr<tuple> tuple_ptr = getTuple(ID);
	tuple_ptr -> cancel();
	write(*tuple_ptr, __tupleMap[ID]);
}

void fStorage::erase (unsigned long ID) throw (exception&) {
	std::map<unsigned long, segment>::const_iterator tuple_it = __tupleMap.find(ID);
	if (tuple_it != __tupleMap.end()) {
		pushTrash(tuple_it ->second);
		__tupleMap.erase(tuple_it);
	}
	else
		throw exception(exception::noID, "(file storage) Error: no match was found for the specified tuple id. (it was " + std::to_string(ID) + ")");
}

enum tuple::state fStorage::state (unsigned long ID) const	throw (exception&)	{
	std::unique_ptr<tuple> tuple_ptr = getTuple(ID);
	return tuple_ptr -> state();
}

bool fStorage::visible (unsigned long ID) const	throw (exception&)	{
	std::unique_ptr<tuple> tuple_ptr = getTuple(ID);
	return tuple_ptr -> visible();
}

void fStorage::value (unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr) const throw (exception&) {
	std::unique_ptr<tuple> tuple_ptr = getTuple(ID);
	tuple_ptr -> value(ptr);
}

void fStorage::old (unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr) const throw (exception&) {
	std::unique_ptr<tuple> tuple_ptr = getTuple(ID);
	tuple_ptr -> old(ptr);
}

std::unique_ptr<std::list <std::string>> fStorage::value (unsigned long ID) const throw (exception&) {
	std::unique_ptr<tuple> tuple_ptr =	getTuple(ID);
	return tuple_ptr -> value();
}

std::unique_ptr<std::list <std::string>> fStorage::old (unsigned long ID) const throw (exception&) {
	std::unique_ptr<tuple> tuple_ptr =	getTuple(ID);
	return tuple_ptr -> old();
}


void fStorage::write(const tuple& _tuple, const segment _segment) const throw (exception&) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::in|std::ios::out);
	if(!file.is_open())
		throw exception(exception::fileOpening, "(file storage) Error: '" + __fileName + "' can not be opened!");
	file.seekp(_segment.begin);
	_tuple.write(file);
	if (file.tellg() != _segment.end + (std::streampos)1)
		throw exception(exception::ioError, "(file storage) I/O error during write: '" + __fileName + "' may be corrupt or there may be a bug in the program!");
	file.close();
}

void fStorage::append(const tuple& _tuple, segment& _segment) throw (exception&) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::in|std::ios::out|std::ios::ate);
	if(!file.is_open())
		throw exception(exception::fileOpening, "(file storage) Error: '" + __fileName + "' can not be opened!");
	_segment.begin = file.tellp();
	_tuple.write(file);
	_segment.end = file.tellp() - (std::streamoff)1;
	if (_tuple.sizeOnFile() != _segment.size())
		throw exception(exception::ioError, "(file storage) I/O error during append: '" + __fileName + "' may be corrupt or there may be a bug in the program!");
	file.close();
}

void fStorage::read (tuple& _tuple, const segment _segment) const throw (exception&) {
	std::fstream file;
	file.open(__fileName.c_str(), std::ios::binary|std::ios::in|std::ios::out);
	if(!file.is_open())
		throw exception(exception::fileOpening, "(file storage) Error: '" + __fileName + "' can not be opened!");
	file.seekg(_segment.begin);
	_tuple.read(file);
	if ((_tuple.sizeOnFile() != _segment.size()) | (file.tellg() != _segment.end + (std::streamoff)1))
		throw exception(exception::ioError, "(file storage) I/O error during read: '" + __fileName + "' may be corrupt or there may be a bug in the program!");
	file.close();
}

void fStorage::pushTrash (segment _segment) {
	bool neighbor = false;
	std::map <unsigned long, segment>::iterator it = __trash.begin();
	if (!__trash.empty())
		while (it != __trash.end() && !neighbor) {	//se il segmento che si aggiunge al cestino è adiacente ad uno già esistente, allora il segmento nel cestino viene
													//opportunamente esteso
			if (it->second.end + (std::streamoff)1 == _segment.begin) {
				it->second.end = _segment.end;
				neighbor = true;
			}
			if (it->second.begin - (std::streamoff)1 == _segment.end) {
				it->second.begin = _segment.begin;
				neighbor = true;
			}
			it++;
		}
	if (!neighbor) {							//altrimenti viene aggiunto un nuovo segmento nel cestino
		__trash.insert(std::pair<unsigned long,segment>(__trashID + 1, _segment));
		++__trashID;
	}
}

unsigned long fStorage::recycle(std::streamoff byte) const {
	unsigned long index = 0;
	if (!__trash.empty()) {
		std::map <unsigned long, segment>::const_iterator it = __trash.begin();
		while (index == 0 && it != __trash.end()) {
			if (it -> second.size() >= byte)
				index = it -> first;
			else
				it++;
		}
	}
	return index;
}

fStorage::segment	fStorage::popTrash (unsigned long id, std::streamoff byte) {
	std::map<unsigned long, segment>::iterator it = __trash.find(id);
	segment _segment;
	_segment.begin = it->second.begin;
	_segment.end = _segment.begin + byte - (std::streamoff)1;
	it->second.begin = _segment.end + (std::streamoff)1;
	if (it->second.size() == 0)
		__trash.erase(it);
	return _segment;
}

void fStorage::getTuple (unsigned long ID, std::unique_ptr<tuple>& ptr ) const throw (exception&) {
	std::map<unsigned long, segment>::const_iterator tuple_it = __tupleMap.find(ID);
	if (tuple_it != __tupleMap.end()) {
		ptr = std::move(std::unique_ptr<tuple>(new tuple));
		read(*ptr, tuple_it -> second);
	}
	else
		throw exception(exception::noID, "(file storage) Error: no match was found for the specified tuple id. (it was " + std::to_string(ID) + ")");
}

std::unique_ptr<tuple> fStorage::getTuple (unsigned long ID) const throw (exception&) {
	std::map<unsigned long, segment>::const_iterator tuple_it = __tupleMap.find(ID);
	if (tuple_it != __tupleMap.end()) {
		std::unique_ptr<tuple> ptr(new tuple);
		read(*ptr, tuple_it -> second);
		return ptr;
	}
	else
		throw exception(exception::noID, "(file storage) Error: no match was found for the specified tuple id. (it was " + std::to_string(ID) + ")");
}
