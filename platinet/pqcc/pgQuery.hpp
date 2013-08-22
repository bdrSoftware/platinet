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

 #ifndef __PQCC_PGQUERY_
 #define __PQCC_PGQUERY_

#include "libpq-fe.h"
#include <string>
#include "pgConnection.hpp"

namespace pqcc {
class pgQuery {
public:
	explicit pgQuery(std::string sqlCommand) : __sqlCommand(sqlCommand), __executed(false), __pgresult(0) {}
	
	~pgQuery();
	
	void exec		(const pgConnection& _conn);

	bool		executed	()		const	{return __executed;}
	enum status {empty, commandOK, tuplesOK, nonFatal, fatal};
	enum status queryStatus	()		const;
	std::string statusMsg	()		const;
	std::string errorMsg	()		const;

	int				numTuples	()						const;
	int 			numColumns	()						const;
	std::string		columnName	(int colNumber)			const;
	std::string		value		(int row, int col)		const;
	bool			isNull		(int row, int col)		const;

private:
	std::string __sqlCommand;
	bool		__executed;
	PGresult*	__pgresult;
};
};
#endif
