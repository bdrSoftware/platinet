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

#ifndef FSTORAGE
#define FSTORAGE

#include "storage.hpp"
#include <map>

namespace engine {
class fStorage : public storage {
public :
	fStorage (std::string fileName)	throw (exception&);

	virtual void										internalID	(std::unique_ptr<std::list<unsigned long>>& ptr)	const	throw ();
	virtual std::unique_ptr<std::list<unsigned long>>	internalID	()													const	throw ();
	virtual unsigned long								numTuples	()													const	throw ()	{return __tupleMap.size();}
	virtual void										clear		()															throw ();

	virtual unsigned long	insert	(const std::list<std::string>& values, const std::list<column>& structure, enum tuple::state _status = tuple::inserting)	throw (exception&);
	
	virtual void	update	(unsigned long ID, const std::list<std::string>& values, const std::list<column>& structure)	throw (exception&);
	virtual void 	cancel	(unsigned long ID)																				throw (exception&);

	virtual void							erase		(unsigned long ID) 			throw (exception&);
	virtual enum tuple::state				state		(unsigned long ID)	const	throw (exception&);
	virtual bool							visible		(unsigned long ID)	const	throw (exception&);
	
	virtual void 	value	(unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr)	const	throw (exception&);
	virtual void	old		(unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr)	const	throw (exception&);

	virtual std::unique_ptr<std::list <std::string>> 	value	(unsigned long ID)	const	throw (exception&);
	virtual std::unique_ptr<std::list <std::string>>	old		(unsigned long ID)	const	throw (exception&);

private :
	struct segment {
		segment() : begin(0), end(0) {}
		std::streampos begin;
		std::streampos end;
		std::streamoff size() const {return end - begin + (std::streamoff)1;}
	};
	std::map <unsigned long, segment>	__tupleMap;
	static const std::string			__path;
	std::string							__fileName;

	void write	(const tuple& _tuple, const segment _segment)	const	throw (exception&);
	void append	(const tuple& _tuple, segment& _segment)				throw (exception&);
	void read 	(tuple& _tuple, const segment _segment)		const	throw (exception&);

	std::map<unsigned long, segment>	__trash;
	unsigned long						__trashID;
	void 			pushTrash	(segment _segment);
	unsigned long	recycle		(std::streamoff byte)						const;
	segment			popTrash	(unsigned long id, std::streamoff byte);

	void					getTuple	(unsigned long ID, std::unique_ptr<tuple>& ptr)	const	throw (exception&);
	std::unique_ptr<tuple>	getTuple	(unsigned long ID)								const	throw (exception&);
};
};
#endif

