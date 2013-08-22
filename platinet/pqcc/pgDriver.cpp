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
#include "pgDriver.hpp"
using namespace pqcc;

pgDriver::pgDriver(std::string _host, std::string _port, std::string _dbname, std::string _user, std::string _passwd, unsigned _cuncurrend_connection) :
	__cuncurrent_connection(_cuncurrend_connection), __pgConnection_array(0), __queryID(0), __connection_free(__cuncurrent_connection) {
		__pgConnection_array = new pgConnection_m[__cuncurrent_connection];
	for (unsigned i = 0; i < __cuncurrent_connection; i++) {
		__pgConnection_array[i].__id = i;
		__pgConnection_array[i].__pgConnection.host(_host);
		__pgConnection_array[i].__pgConnection.port(_port);
		__pgConnection_array[i].__pgConnection.dbname(_dbname);
		__pgConnection_array[i].__pgConnection.user(_user);
		__pgConnection_array[i].__pgConnection.passwd(_passwd);
	}
}

pgDriver::~pgDriver() {
	delete [] __pgConnection_array;
}

void pgDriver::host (std::string _host) {
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		__pgConnection_array[i].__pgConnection.host(_host);
}

void pgDriver::port (std::string _port) {
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		__pgConnection_array[i].__pgConnection.port(_port);
}

void pgDriver::dbname (std::string _dbname) {
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		__pgConnection_array[i].__pgConnection.dbname(_dbname);
}

void pgDriver::user (std::string _user) {
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		__pgConnection_array[i].__pgConnection.user(_user);
}

void pgDriver::passwd (std::string _passwd) {
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		__pgConnection_array[i].__pgConnection.passwd(_passwd);
}

void pgDriver::connect () {
	for (unsigned i = 0; i < __cuncurrent_connection; i++) {
		std::thread thr(&pgConnection::connect, std::ref(__pgConnection_array[i].__pgConnection));
		thr.detach();
	}
}

void pgDriver::connectBlock () {
	for (unsigned i = 0; i < __cuncurrent_connection; i++) 
		__pgConnection_array[i].__pgConnection.connect();
}


std::unique_ptr<enum pgConnection::status[]> pgDriver::connectionStatus () const {
	std::unique_ptr<enum pgConnection::status[]> ptr(new enum pgConnection::status [__cuncurrent_connection]);
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		ptr[i] = __pgConnection_array[i].__pgConnection.connectionStatus();
	return ptr;
}

void pgDriver::disconnect () {
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		__pgConnection_array[i].__pgConnection.disconnect();
}

void pgDriver::reset () {
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		__pgConnection_array[i].__pgConnection.reset();
}

std::unique_ptr<std::string[]> pgDriver::connectionErrorMsg () const {
	std::unique_ptr<std::string[]> ptr (new std::string [__cuncurrent_connection]);
	for (unsigned i = 0; i < __cuncurrent_connection; i++)
		ptr[i] = __pgConnection_array[i].__pgConnection.errorMsg();
	return ptr;
}

unsigned long pgDriver::execQuery (std::string sqlStatement) {
	++__queryID;
	__queryMap.insert(std::pair<unsigned long, pgQuery>(__queryID, pgQuery(sqlStatement)));
	std::thread thr(&pgDriver::executeQuery, std::ref(*this), __queryID);
	thr.detach();
	return __queryID;
}

unsigned long pgDriver::execQueryBlock (std::string sqlStatement) {
	++__queryID;
	__queryMap.insert(std::pair<unsigned long, pgQuery>(__queryID, pgQuery(sqlStatement)));
	executeQuery(__queryID);
	return __queryID;
}

bool pgDriver::executed (unsigned long queryID) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.executed();
	else
		return false;
}

enum pgQuery::status pgDriver::queryStatus (unsigned long queryID) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.queryStatus();
	else
		return pgQuery::nonFatal;
}

std::string pgDriver::queryStatusMsg (unsigned long queryID) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.statusMsg();
	else
		return "";
}

std::string pgDriver::queryErrorMsg (unsigned long queryID) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.errorMsg();
	else
		return "";
}

int pgDriver::numTuples (unsigned long queryID) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.numTuples();
	else
		return 0;
}

int pgDriver::numColumns (unsigned long queryID) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.numColumns();
	else
		return 0;
}

std::string pgDriver::columnName (unsigned long queryID, int colNumber) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.columnName(colNumber);
	else
		return "";
}

std::string pgDriver::value (unsigned long queryID, int row, int col) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.value(row, col);
	else
		return "";
}

bool pgDriver::isNull (unsigned long queryID, int row, int col) const {
	std::map<unsigned long, pgQuery>::const_iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		return it->second.isNull(row, col);
	else
		return false;
}

void pgDriver::erase (unsigned long queryID) {
	std::map<unsigned long, pgQuery>::iterator it = __queryMap.find(queryID);
	if (it != __queryMap.end())
		__queryMap.erase(it);
}

void pgDriver::executeQuery (unsigned long id) {
	std::map<unsigned long, pgQuery>::iterator it = __queryMap.find(id);
	__connection_free_mtx.lock();
	while (__connection_free == 0)									
		__connection_free_cv.wait(__connection_free_mtx);				
	__connection_free--;
	__connection_free_mtx.unlock();

	unsigned __connection_free_index = 0;
	while (__connection_free_index < __cuncurrent_connection && !__pgConnection_array[__connection_free_index].__busy_mtx.try_lock())	__connection_free_index++;
		it->second.exec(__pgConnection_array[__connection_free_index].__pgConnection);
		__pgConnection_array[__connection_free_index].__busy_mtx.unlock();

	__connection_free_mtx.lock();
	__connection_free++;
	if (__connection_free == 1)	__connection_free_cv.notify_all();
	__connection_free_mtx.unlock();
}
