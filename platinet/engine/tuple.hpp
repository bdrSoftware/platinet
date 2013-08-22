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

#ifndef TUPLE_HEADER
#define TUPLE_HEADER

#include <list>
#include <string>
#include <fstream>
#include <memory>
#include "exception.hpp"
#include "column.hpp"

namespace engine {
class tuple {
public :
	enum state {empty, loaded, inserting, updating, deleting};
	tuple() : __state(empty), __visible(true) {}
	tuple(const std::list <std::string>& rowValue, const std::list <column>& structure, state rowState = inserting) throw (exception&);
	
	void		update		(const std::list <std::string>& newValue, const std::list <column>& structure)	throw ();
	
	void		cancel		()			throw ();
	void		erase		()			throw ();
	enum state	state		()	const	throw ()	{return __state;}
	bool		visible		()	const	throw ()	{return __visible;}
	
	void	value	(std::unique_ptr<std::list <std::string>>& ptr)	const	throw ();
	void	old		(std::unique_ptr<std::list <std::string>>& ptr)	const	throw ();
	
	std::unique_ptr<std::list <std::string>>	value	()	const	throw ();
	std::unique_ptr<std::list <std::string>>	old		()	const	throw ();
		
	std::streamoff	sizeOnFile	()						const	throw ();
	void			write		(std::fstream& stream)	const	throw ();
	void			read		(std::fstream& stream)			throw ();

private :
	std::list <std::string>	__value;
	enum state				__state;
	std::list <std::string>	__old;
	bool					__visible;

	void	_insert	(const std::list <std::string>& from, std::list <std::string>& to, const std::list <column>& structure)	const	throw (exception&);
	void	_update	(const std::list <std::string>& from, std::list <std::string>& to, const std::list <column>& structure)	const	throw ();

	void	write_string	(std::fstream& stream, const std::string& _string)				const	throw ();
	void	read_string		(std::fstream& stream, std::string& _string)					const	throw ();
	void	write_list		(std::fstream& stream, const std::list <std::string>& _list)	const	throw ();
	void	read_list		(std::fstream& stream, std::list <std::string>& _list)			const	throw ();
};
};
#endif
