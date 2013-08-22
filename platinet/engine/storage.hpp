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
#ifndef STORAGE
#define STORAGE
#include "exception.hpp"
#include "column.hpp"
#include "tuple.hpp"
#include <memory>

namespace engine {
class storage {
public :
	storage() throw ()	: __lastKey(0) {};

	virtual void										internalID	(std::unique_ptr<std::list<unsigned long>>& ptr)	const	throw () = 0;
	virtual std::unique_ptr<std::list<unsigned long>>	internalID	()													const	throw () = 0;
	virtual unsigned long								numTuples	()													const	throw () = 0;
	virtual void										clear		()															throw () = 0;

	virtual unsigned long	insert	(const std::list<std::string>& values, const std::list<column>& structure, enum tuple::state _status = tuple::inserting)	throw (exception&) = 0;
	
	virtual void	update	(unsigned long ID, const std::list<std::string>& values, const std::list<column>& structure)	throw (exception&)	= 0;
	virtual void 	cancel	(unsigned long ID)											 									throw (exception&)	= 0;

	virtual void				erase		(unsigned long ID) 			throw (exception&)	= 0;
	virtual enum tuple::state	state		(unsigned long ID)	const	throw (exception&)	= 0;
	virtual bool				visible		(unsigned long ID)	const	throw (exception&)	= 0;
	
	virtual void 	value	(unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr)	const	throw (exception&)	= 0;
	virtual void	old		(unsigned long ID, std::unique_ptr<std::list <std::string>>& ptr)	const	throw (exception&)	= 0;

	virtual std::unique_ptr<std::list <std::string>> 	value	(unsigned long ID)	const	throw (exception&) = 0;
	virtual std::unique_ptr<std::list <std::string>>	old		(unsigned long ID)	const	throw (exception&) = 0;

protected :
	unsigned long 		__lastKey;
};
};
#endif

