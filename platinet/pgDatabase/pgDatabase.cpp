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

#include "pgDatabase.hpp"
#include <iostream>

const std::string pgDatabase::__allColumnQuery = "select col.table_schema,col.table_name,col.column_name,col.data_type,col.character_maximum_length,col.column_default from information_schema.columns col where col.table_schema <> 'pg_catalog'and col.table_schema <> 'information_schema'";
const std::string pgDatabase::__keyColumnQuery = "select col.table_schema, col.table_name, col.column_name from information_schema.columns col join information_schema.constraint_column_usage ccl on col.table_schema=ccl.table_schema and col.table_name=ccl.table_name and col.column_name=ccl.column_name join information_schema.table_constraints ts on ccl.constraint_name=ts.constraint_name where ts.constraint_type = 'PRIMARY KEY' and col.table_schema <> 'pg_catalog'and col.table_schema <> 'information_schema'";
const std::string pgDatabase::__referentialConstraint = "select tc.table_schema, tc.table_name, kcu.column_name, ccu.table_schema, ccu.table_name, ccu.column_name from information_schema.table_constraints tc left join information_schema.key_column_usage kcu ON tc.constraint_catalog = kcu.constraint_catalog AND tc.constraint_schema = kcu.constraint_schema AND tc.constraint_name = kcu.constraint_name left join information_schema.referential_constraints rc ON tc.constraint_catalog = rc.constraint_catalog AND tc.constraint_schema = rc.constraint_schema AND tc.constraint_name = rc.constraint_name left join information_schema.constraint_column_usage ccu ON rc.unique_constraint_catalog = ccu.constraint_catalog AND rc.unique_constraint_schema = ccu.constraint_schema AND rc.unique_constraint_name = ccu.constraint_name where tc.constraint_type = 'FOREIGN KEY'";

pgDatabase::pgDatabase(std::string _host, std::string _port, std::string _dbname, std::string _user, std::string _passwd, unsigned _cuncurrend_connection)	
	: __pgDriver(_host, _port, _dbname, _user, _passwd, _cuncurrend_connection), __database(_dbname) {}

void pgDatabase::loadStructure () {
	unsigned long structureQueryID = __pgDriver.execQueryBlock(__allColumnQuery);
	if (__pgDriver.queryStatus(structureQueryID) == pqcc::pgQuery::tuplesOK) {
		__database.clearStructure();
		for (int i = 0; i < __pgDriver.numTuples(structureQueryID); i++) {
			std::string schemaName		=	__pgDriver.value(structureQueryID, i, 0);
			std::string tableName 		=	__pgDriver.value(structureQueryID, i, 1);
			std::string columnName		=	__pgDriver.value(structureQueryID, i, 2);
			if (!__database.findSchema(schemaName))
				__database.addSchema(schemaName);
			if (!__database.findTable(schemaName, tableName))
				__database.addTable(schemaName, tableName);
			if (!__database.findColumn(schemaName, tableName, columnName))	{
				engine::column::sqlType type = __sqlType(std::string(__pgDriver.value(structureQueryID, i, 3)));
				unsigned length	=	__columnLength(std::string(__pgDriver.value(structureQueryID, i, 4)), type);
				std::string columnDefault	=	__columnDefault(std::string(__pgDriver.value(structureQueryID, i, 5)), type);
				__database.addColumn(schemaName, tableName, columnName, type, length, false, columnDefault);
			}
		}
		__pgDriver.erase(structureQueryID);

		unsigned keyQuery = __pgDriver.execQueryBlock(__keyColumnQuery);
		if (__pgDriver.queryStatus(keyQuery) == pqcc::pgQuery::tuplesOK)
			for (int i = 0; i < __pgDriver.numTuples(keyQuery); i++) {
				std::string schemaName		=	__pgDriver.value(keyQuery, i, 0);
				std::string tableName 		=	__pgDriver.value(keyQuery, i, 1);
				std::string columnName		=	__pgDriver.value(keyQuery, i, 2);
				if (__database.findColumn(schemaName, tableName, columnName))
					__database.isKey(schemaName, tableName, columnName, true);
			}
		__pgDriver.erase(keyQuery);

		unsigned referenceQuery = __pgDriver.execQueryBlock(__referentialConstraint);
		if (__pgDriver.queryStatus(referenceQuery) == pqcc::pgQuery::tuplesOK)
			for (int i = 0; i < __pgDriver.numTuples(referenceQuery); i++) {
				std::string schemaName			=		__pgDriver.value(referenceQuery, i, 0);
				std::string tableName 			=		__pgDriver.value(referenceQuery, i, 1);
				std::string columnName			=		__pgDriver.value(referenceQuery, i, 2);
				std::string ref_schemaName		=		__pgDriver.value(referenceQuery, i, 3);
				std::string ref_tableName 		=		__pgDriver.value(referenceQuery, i, 4);
				std::string ref_columnName		=		__pgDriver.value(referenceQuery, i, 5);
				__database.addReference(schemaName, tableName, columnName, ref_schemaName, ref_tableName, ref_columnName);
			}
		__pgDriver.erase(referenceQuery);
	}
}

void pgDatabase::loadTuple () {
	std::unique_ptr<std::list <engine::schema::loadCommand_info>>	__loadCommand =	__database.loadCommand ();
	std::list <std::thread> __threadList;
	for (std::list <engine::schema::loadCommand_info>::iterator it = __loadCommand->begin(); it != __loadCommand->end(); it++)
		__threadList.push_back(std::thread(&pgDatabase::loadTupleThr, std::ref(*this), *it));
	for (std::list <std::thread>::iterator thr_it = __threadList.begin(); thr_it != __threadList.end(); thr_it++)
		if (thr_it -> joinable())
			thr_it -> join();
}

std::unique_ptr<std::list <unsigned long>> pgDatabase::commit () throw () {
	std::unique_ptr<std::list <std::string>>	__sqlList = __database.commit();
	std::unique_ptr<std::list <unsigned long>>	__queryIDList(new std::list <unsigned long>);
	for (std::list <std::string>::const_iterator it = __sqlList->begin(); it != __sqlList->end(); it++)
		__queryIDList->push_back(__pgDriver.execQuery(*it));
	return __queryIDList;
}

std::unique_ptr<std::list <unsigned long>> pgDatabase::commit (std::string schemaName, std::string tableName) throw (engine::exception&) {
	std::unique_ptr<std::list <std::string>>	__sqlList = __database.commit(schemaName, tableName);
	std::unique_ptr<std::list <unsigned long>>	__queryIDList(new std::list <unsigned long>);
	for (std::list <std::string>::const_iterator it = __sqlList->begin(); it != __sqlList->end(); it++)
		__queryIDList->push_back(__pgDriver.execQuery(*it));
	return __queryIDList;
}

std::unique_ptr<std::list <unsigned long>> pgDatabase::commit (std::string tablePath) throw (engine::exception&) {
	std::unique_ptr<std::list <std::string>>	__sqlList = __database.commit(tablePath);
	std::unique_ptr<std::list <unsigned long>>	__queryIDList(new std::list <unsigned long>);
	for (std::list <std::string>::const_iterator it = __sqlList->begin(); it != __sqlList->end(); it++)
		__queryIDList->push_back(__pgDriver.execQuery(*it));
	return __queryIDList;
}

engine::column::sqlType pgDatabase::__sqlType(std::string type) const {
	if (type == "character varying")
		return engine::column::varchar;
	if	(type == "integer")
		return engine::column::integer;
	if	(type == "real")
		return engine::column::real;
	if	(type == "date")
		return engine::column::date;
	if	(type == "double precision")
		return engine::column::doublePrecision;
	if	(type == "boolean")
		return engine::column::boolean;
	if	(type == "smallint")
		return engine::column::smallint;
	if	(type == "bigint")
		return engine::column::bigint;
	if	(type == "time without time zone")
		return engine::column::time;
}

std::string pgDatabase::__columnDefault(std::string	colDefault, engine::column::sqlType type)	const {
	if (colDefault != "" && (type == engine::column::varchar || type == engine::column::time || type == engine::column::date)) {
		unsigned first = colDefault.find_first_of('\'');
		unsigned last = colDefault.find_last_of('\'');
		return colDefault.substr(first + 1, last - first - 1);
	}
	else
		return colDefault;
}

unsigned pgDatabase::__columnLength(std::string length, engine::column::sqlType type) const {
	if (type == engine::column::varchar)
		return std::stoul(length);
	else
		return 0;
}
void pgDatabase::loadTupleThr (engine::schema::loadCommand_info 	_loadInfo) {
	unsigned queryID = __pgDriver.execQueryBlock(_loadInfo.__loadCommand);
	if (__pgDriver.queryStatus(queryID) == pqcc::pgQuery::tuplesOK) {
		__database.clear(_loadInfo.__tablePath);
		for (int i = 0; i < __pgDriver.numTuples(queryID); i++) {
			std::list<std::string> values;
			for(int j = 0; j < __pgDriver.numColumns(queryID); j++)
				values.push_back(__pgDriver.value(queryID, i, j));
			__database.insertTuple(_loadInfo.__tablePath, values, engine::tuple::loaded);
		}
	}
}
