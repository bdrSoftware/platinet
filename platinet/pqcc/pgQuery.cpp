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
#include "pgQuery.hpp"
using namespace pqcc;

pgQuery::~pgQuery() {
	if (__pgresult != 0)
		PQclear(__pgresult);
}
	
void pgQuery::exec(const pgConnection& _conn) {
	if (!__executed) {
		__pgresult = PQexec(_conn.get_PGconn(), __sqlCommand.c_str());
		__executed = true;
	}
}

enum pgQuery::status pgQuery::queryStatus () const {
	if (__pgresult != 0)
		switch(PQresultStatus(__pgresult)) {
			case PGRES_EMPTY_QUERY 		:	return empty;
			case PGRES_COMMAND_OK 		:	return commandOK;
			case PGRES_TUPLES_OK		:	return tuplesOK;
			case PGRES_NONFATAL_ERROR	:	return nonFatal;
			case PGRES_FATAL_ERROR		:	return fatal;
			default						:	return nonFatal;
		}
	return fatal;
}

std::string pgQuery::statusMsg () const {
	std::string error = std::string(PQresStatus(PQresultStatus(__pgresult)));
	return error;
}

std::string pgQuery::errorMsg	() const {
	std::string error = std::string(PQresultErrorMessage(__pgresult));
	return error;
}

int pgQuery::numTuples () const {
	return PQntuples(__pgresult);
}

int pgQuery::numColumns () const {
	return PQnfields(__pgresult);
}

std::string pgQuery::columnName (int colNumber) const {
	std::string colName = "";
	if (colNumber < PQnfields(__pgresult))
		colName = std::string(PQfname(__pgresult, colNumber));
	return colName;
}

std::string pgQuery::value (int row, int col) const {
	std::string val = std::string(PQgetvalue(__pgresult, row, col));
	return val;
}

bool pgQuery::isNull (int row, int col) const {
	if (PQgetisnull(__pgresult, row, col) == 1)
		return true;
	else
		return false;
}
