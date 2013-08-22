/*
 * Copyright 2011, 2012, 2013 Salvatore Barone <salvator.barone@gmail.com>
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

#include "pgConnection.hpp"
using namespace pqcc;


pgConnection::~pgConnection() {
	disconnect();
}

void pgConnection::connect() {
	std :: string connection_string = "host='" + __host + "' port='" + __port + "' dbname='" + __dbname + "' user='" +  __user + "' password='" + __passwd + "'";
	__pgconnection = PQconnectdb(connection_string.c_str());
}

enum pgConnection::status pgConnection::connectionStatus() const {
	if (__pgconnection != 0)
		switch (PQstatus(__pgconnection)) {
			case CONNECTION_OK 	:	return	ok;
			case CONNECTION_BAD :	return	bad;
			default				:	return	bad;
		}
	else
		return bad;
}

void pgConnection::disconnect() {
	if (__pgconnection != 0)
		PQfinish(__pgconnection);
}

void pgConnection::reset() {
	if (__pgconnection != 0)
		PQreset(__pgconnection);
}

std::string	pgConnection::errorMsg() const {
	std::string error = std::string(PQerrorMessage(__pgconnection));
	return error;
}
