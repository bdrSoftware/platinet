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
 
#ifndef _TIME_
#define _TIME_

#include <string>

namespace engine {
class time {
public :
	time ();
	unsigned	seconds		()					const	{return __seconds;}
	unsigned	minutes		()					const	{return __minutes;}
	unsigned	hour		()					const	{return __hours;}
	unsigned	day		()					const	{return __day;}
	unsigned	month		()					const	{return __month;}
	unsigned	year		()					const	{return __year;}
	std::string 	clock		(char sep = ':')			const;
	std::string	date		(char sep = '/')			const;
	std::string	timeStamp	(char d_sep = '/', char t_sep = ':')	const;

private :
	unsigned	__seconds;
	unsigned	__minutes;
	unsigned	__hours;
	unsigned	__day;
	unsigned	__month;
	unsigned	__year;
};
};
#endif
