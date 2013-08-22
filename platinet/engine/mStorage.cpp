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

#include "mStorage.hpp"
using namespace engine;

void mStorage::internalID (std::unique_ptr<std::list<unsigned long>>& ptr) const throw () {
	ptr = std::move(std::unique_ptr<std::list<unsigned long>>(new std::list<unsigned long>));
	for (std::map <unsigned long, tuple>::const_iterator it = __tupleMap.begin(); it != __tupleMap.end(); it++)
		ptr -> push_back(it -> first);
}

std::unique_ptr<std::list<unsigned long>> mStorage::internalID () const throw () {
	std::unique_ptr<std::list<unsigned long>> ptr(new std::list<unsigned long>);
	for (std::map <unsigned long, tuple>::const_iterator it = __tupleMap.begin(); it != __tupleMap.end(); it++)
		ptr -> push_back(it -> first);
	return ptr;
}

void mStorage::clear () throw () {
	__tupleMap.clear();
	__lastKey = 0;
}

//ultimo test completo: 23 luglio 2013, 18:46
//esito test: negativo (tutto funzionante)
unsigned long	mStorage::insert (const std::list<std::string>& values, const std::list<column>& structure, enum tuple::state _status) throw (exception&) {
	__lastKey++;
	__tupleMap.insert(std::pair<unsigned long,tuple>(__lastKey, tuple(values, structure, _status)));
	return __lastKey;
}

//ultimo test completo: 23 luglio 2013, 18:46
//esito test: negativo (tutto funzionante)
void mStorage::erase (unsigned long ID) throw (exception&) {
	std::map <unsigned long, tuple>::iterator it = __tupleMap.find(ID);
	if (it != __tupleMap.end())
		__tupleMap.erase(it);
	else
 		throw exception(exception::noID, "Error: no match was found for the specified tuple id. (it was " + std::to_string(ID) + ")");
}

//ultimo test completo: 23 luglio 2013, 18:46
//esito test: negativo (tutto funzionante)
const tuple& mStorage::getTuple (unsigned long ID) const throw (exception&){
	std::map <unsigned long, tuple>::const_iterator it = __tupleMap.find(ID);
	if (it != __tupleMap.end())
		return it -> second;
	else
 		throw exception(exception::noID, "Error: no match was found for the specified tuple id. (it was " + std::to_string(ID) + ")");
}

//ultimo test completo: 23 luglio 2013, 18:46
//esito test: negativo (tutto funzionante)
tuple& mStorage::getTuple (unsigned long ID) throw (exception&) {
	std::map <unsigned long, tuple>::iterator it = __tupleMap.find(ID);
	if (it != __tupleMap.end())
		return it -> second;
	else
		throw exception(exception::noID, "Error: no match was found for the specified tuple id. (it was " + std::to_string(ID) + ")");
}
