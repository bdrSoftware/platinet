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

#ifndef MSTORAGE
#define MSTORAGE

#include "storage.hpp"
#include <map>

namespace engine {
class mStorage : public storage {
public :
	mStorage () throw () : storage() {}

	virtual void										internalID	(std::unique_ptr<std::list<unsigned long>>& ptr)	const	throw ();
	virtual std::unique_ptr<std::list<unsigned long>>	internalID	()													const	throw ();
	virtual unsigned long								numTuples	()													const	throw ()	{return __tupleMap.size();}
	virtual void										clear		()															throw ();

	virtual unsigned long	insert	(const std::list<std::string>& values, const std::list<column>& structure, enum tuple::state _status = tuple::inserting)	throw (exception&);
	
	virtual void	update	(unsigned long ID, const std::list<std::string>& values, const std::list<column>& structure)	throw (exception&)	{getTuple(ID).update(values, structure);}
	virtual void 	cancel	(unsigned long ID)											 									throw (exception&)	{getTuple(ID).cancel();}

	virtual void							erase		(unsigned long ID) 			throw (exception&);
	virtual enum tuple::state				state		(unsigned long ID)	const	throw (exception&)	{return getTuple(ID).state();}
	virtual bool							visible		(unsigned long ID)	const	throw (exception&)	{return getTuple(ID).visible();}
	
	virtual void 	value	(unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr)	const	throw (exception&)	{getTuple(ID).value(ptr);}
	virtual void	old		(unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr)	const	throw (exception&)	{getTuple(ID).old(ptr);}

	virtual std::unique_ptr<std::list <std::string>> 	value	(unsigned long ID)	const	throw (exception&)	{return getTuple(ID).value();}
	virtual std::unique_ptr<std::list <std::string>>	old		(unsigned long ID)	const	throw (exception&)	{return getTuple(ID).old();}

private :
	std::map <unsigned long, tuple>	__tupleMap;
	const tuple&	getTuple	(unsigned long ID)	const	throw (exception&);
	tuple&			getTuple	(unsigned long ID)			throw (exception&);
};
};
#endif
