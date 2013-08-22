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
#ifndef __PQCC_PGDRIVER__
#define __PQCC_PGDRIVER__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <memory>
#include "pgConnection.hpp"
#include "pgQuery.hpp"

namespace pqcc {
class pgDriver {
public:
	pgDriver(
			std::string	_host = "",					//host-address or host-name of postgresql database server
			std::string _port = "5432",				//port
			std::string	_dbname = "",				//database name
			std::string	_user = "",					//user name
			std::string	_passwd = "",				//password
			unsigned	_cuncurrend_connection = 5	//number of simultaneous connection to 	postgresql database server
			);

	~pgDriver();

	/**							DBMS CONNECTION FUCNTION							**/
	void											host				(std::string _host);
	void											port				(std::string _port);
	void											dbname				(std::string _dbname);
	void											user				(std::string _user);
	void											passwd				(std::string _passwd);
	void											connect				();										//connect to remote server in non-blocking mode
	void											connectBlock		();										//connect to remote server in non-blocking mode
	std::unique_ptr<enum pgConnection::status[]>	connectionStatus	()							const;		//may be "ok" or  "bad"
	void											disconnect			();										//disconnect to remote server
	void											reset				();										//reset connection
	std::unique_ptr<std::string[]> 					connectionErrorMsg	()							const;		//get error message

	/**								QUERY EXECUTION FUNCION								**/
	unsigned long 			execQuery		(std::string sqlStatement);							//begins or queues the execution of a query. returns the unique ID number of the query
	unsigned long 			execQueryBlock	(std::string sqlStatement);							//begins or queues the execution of a query blocking the callig thread. returns the unique ID number of the query
	bool					executed		(unsigned long queryID)						const;	//true if the query has already been executed
	enum pgQuery::status 	queryStatus		(unsigned long queryID)						const;	//may be pgQuery::empty, pgQuery::commandOK, pgQuery::tuplesOK, pgQuery::nonFatal or pgQuery::fatal
	std::string 			queryStatusMsg	(unsigned long queryID)						const;	//returns a string containing the status of the query execution
	std::string 			queryErrorMsg	(unsigned long queryID)						const;	//returns a string containing the error status of the query execution
	int						numTuples		(unsigned long queryID)						const;	//returns the number of rows affected by the command or the number of rows in the table that contains the result of the query execution
	int 					numColumns		(unsigned long queryID)						const;	//restituisce il numero di colonne contenute nella tabella contenente il risultato di esecuzione della query
	std::string				columnName		(unsigned long queryID, int colNumber)		const;	//returns the name of the column associated with the index specified. indicx starts from 0
	std::string				value			(unsigned long queryID, int row, int col)	const;	//returns the cell value at (row, col). index starts from 0
	bool					isNull			(unsigned long queryID, int row, int col)	const;	//true if cell value at (row, col) is null
	void					erase			(unsigned long queryID);							//delete a query freeing the space occupied by its result

private:
	unsigned __cuncurrent_connection;

	struct pgConnection_m {
		unsigned		__id;
		pgConnection	__pgConnection;
		std::mutex		__busy_mtx;
	};
	pgConnection_m* __pgConnection_array;

	unsigned long						__queryID;
	std::map<unsigned long, pgQuery>	__queryMap;

	std::mutex						__connection_free_mtx;
	std::condition_variable_any		__connection_free_cv;
	unsigned						__connection_free;
	void 							executeQuery			(unsigned long id);
};
};
#endif
