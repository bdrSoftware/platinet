/*
 * Copyright 2013 Salvatore Barone <salvator.barone@gmail.com>
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

 #ifndef __PQCC_PGCONNECTION_
 #define __PQCC_PGCONNECTION_

#include "libpq-fe.h"
#include <string>

namespace pqcc {

class pgConnection {
public:
	pgConnection(
			std::string	_host = "",			//host-address or hostname of postgresql server
			std::string _port = "5432",		//port
			std::string	_dbname = "",		//database name
			std::string	_user = "",			//user name
			std::string	_passwd = ""		//password
			) : __host(_host), __port(_port), __dbname(_dbname), __user(_user), __passwd(_passwd), __pgconnection(0) {}

	~pgConnection();

	std::string		host	()	const	{return __host;}
	std::string		port	()	const	{return __port;}
	std::string		dbname	()	const	{return __dbname;}
	std::string		user	()	const	{return __user;}

	void			host	(std::string _host)		{__host = _host;}
	void			port	(std::string _port)		{__port = _port;}
	void			dbname	(std::string _dbname)	{__dbname = _dbname;}
	void			user	(std::string _user)		{__user = _user;}
	void			passwd	(std::string _passwd)	{__passwd = _passwd;}

	enum status {ok, bad};
	void			connect				();
	enum status		connectionStatus	()	const;
	void			disconnect			();
	void			reset				();
	std::string		errorMsg			()	const;
	PGconn*			get_PGconn			()	const	{return __pgconnection;}

private:
	std::string		__host;
	std::string		__port;
	std::string		__dbname;
	std::string		__user;
	std::string		__passwd;
	PGconn* 		__pgconnection;
};
};

 #endif
