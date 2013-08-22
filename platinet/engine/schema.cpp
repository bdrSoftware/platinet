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

#include "schema.hpp"
#include <algorithm>
#include <iostream>
using namespace engine;

#if !defined __WINDOWS_COMPILING_
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#else

#endif

schema::schema(std::string schemaName, std::string tmpStoragePath) throw () : __schemaName(schemaName), __tmpStoragePath(tmpStoragePath + __schemaName + "/")  {
#if !defined __WINDOWS_COMPILING_
	mkdir(__tmpStoragePath.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else

#endif
};

std::unique_ptr<std::list <std::string>> schema::tablesName(bool shortFormat) const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list <std::string>);
	std::map <std::string, table>::const_iterator it;
	for (it = __tableMap.begin(); it != __tableMap.end(); it++)
		list_ptr -> push_back(it -> first);
	if (!shortFormat && __schemaName != "") {
		std::list <std::string>::iterator it;
		for (it = list_ptr -> begin(); it != list_ptr -> end(); it++)
			(*it) = __schemaName + "." + (*it);
	}
	return list_ptr;
}

void schema::addTable(std::string tableName) throw () {
	if (tableName != "" && __tableMap.count(tableName) == 0)
		__tableMap.insert(std::pair<std::string, table>(tableName, table(tableName, __tmpStoragePath, false)));
}

void schema::addTable(const table& _table) throw (exception&) {
	if (__tableMap.count(_table.tableName()) == 0)
		__tableMap.insert(std::pair<std::string, table>(_table.tableName(), _table));
	else
		throw exception(exception::structural, "Can't add table '" + _table.tableName() + "': there is already a table with this name in schema '" + __schemaName + "'");
}

void schema::dropTable(std::string tableName) throw () {
	std::map <std::string, table>::iterator it = __tableMap.find(tableName);
	if (it != __tableMap.end())
		__tableMap.erase(it);
}

bool schema::findTable (std::string tableName) const throw () {
	std::map <std::string, table>::const_iterator it = __tableMap.find(tableName);
	if (it != __tableMap.end())
		return true;
	else
		return false;
}

std::unique_ptr<std::list<std::string>> schema::columnsName(std::string tableName, bool shortFormat) const throw (exception&) {
	std::map <std::string, table>::const_iterator it = __tableMap.find(tableName);
	if (it != __tableMap.end()) {
		std::unique_ptr<std::list <std::string>> list_ptr;
		if (!shortFormat && __schemaName != "") {
			list_ptr = it -> second.columnsName(false);
			std::list <std::string>::iterator it;
			for (it = list_ptr -> begin(); it != list_ptr -> end(); it++)
				(*it) = __schemaName + "." + (*it);
		}
		else
			list_ptr = it -> second.columnsName();
		return list_ptr;
	}
	else
		throw exception(exception::noTable, "Unable to access table '" + tableName + "': no table with this name in schema '" + __schemaName + "'!");
}

std::unique_ptr<std::list<std::string>> schema::structure() const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list <std::string>);
	for(std::map <std::string, table>::const_iterator it = __tableMap.begin(); it != __tableMap.end(); it++)
		list_ptr->push_back(it->second.structure());
	return list_ptr;
}

std::string schema::insertSql(std::string tableName, unsigned long internalID) const throw (exception&) {

	std::map <std::string, table>::const_iterator tableIterator = find(tableName);

	if (!tableIterator -> second.resultHandler()) {

		std::unique_ptr<std::list <std::string>> columnsName_ptr = tableIterator -> second.columnsName();
		std::unique_ptr<std::list <std::string>> tupleValues_ptr = tableIterator -> second.tupleValues(internalID);

		unsigned lenght = columnsName_ptr -> size(); //coincide con rowValue.size()
		std::string columns = "";
		std::string values = "";
		std::list <std::string>::const_iterator columnsName_iterator = columnsName_ptr -> begin();
		std::list <std::string>::const_iterator tupleValues_iterator = tupleValues_ptr -> begin();

		for (unsigned i = 0; i < lenght; i++, columnsName_iterator++, tupleValues_iterator++) {

			columns += (*columnsName_iterator);

			if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar)
				values += "'";
			values += (*tupleValues_iterator);

			if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar)
				values += "'";

			if (i < lenght - 1) {
				columns += ", ";
				values += ", ";
			}
		}

		std::string sql_command = "insert into ";
		if (__schemaName != "")
			sql_command += __schemaName + ".";
		sql_command += tableIterator -> second.tableName() + " (" + columns + ") values (" + values + ")";
		return sql_command;
	}
	else
		return "";
}

std::string schema::updateSql (std::string tableName, unsigned long internalID) const throw (exception&) {
	std::map <std::string, table>::const_iterator tableIterator = find(tableName);
	if (!tableIterator -> second.resultHandler())  {
		std::unique_ptr<std::list <std::string>> valueList_ptr = tableIterator -> second.tupleValues(internalID);
		if (!valueList_ptr -> empty()) {
			std::string sql_command = "update " + tableName + " set ";
			std::unique_ptr<std::list <std::string>> columnsName_ptr = tableIterator -> second.columnsName();
			std::list <std::string>::const_iterator columnsName_iterator = columnsName_ptr -> begin();
			std::list <std::string>::const_iterator valueList_iterator = valueList_ptr -> begin();
			unsigned numberOfColumns = tableIterator -> second.numberOfColumns();
			unsigned numberofKeys = 0;
			unsigned numberOfNormal = 0;

			std::string set;
			std::string where;
			for (unsigned i = 0; i < numberOfColumns; i++, columnsName_iterator++, valueList_iterator++)
				if (tableIterator -> second.isKey(*columnsName_iterator)) {
					if (numberofKeys > 0) where += " and ";
					where += (*columnsName_iterator) + "=";
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) where +="'";
					where += (*valueList_iterator);
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) where +="'";
					numberofKeys++;
				}
				else {
					if (numberOfNormal > 0) set += ", ";
					set += (*columnsName_iterator) + "=";
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) set +="'";
					set += (*valueList_iterator);
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) set +="'";
					numberOfNormal++;
				}

			if (numberofKeys == 0) {
				columnsName_iterator = columnsName_ptr -> begin();
				valueList_ptr = tableIterator -> second.tupleOldValues(internalID);
				valueList_iterator = valueList_ptr -> begin();
				for (unsigned i = 0; i < numberOfColumns; i++, columnsName_iterator++, valueList_iterator++) {
					if (numberofKeys > 0) where += " and ";
					where += (*columnsName_iterator) + "=";
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) where +="'";
					where += (*valueList_iterator);
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) where +="'";
					numberofKeys++;
				}
			}

			sql_command += set + " where " + where;
			return sql_command;
		}
	}
	return "";
}

std::string schema::deleteSql (std::string tableName, unsigned long internalID) const throw (exception&) {
	std::map <std::string, table>::const_iterator tableIterator = __tableMap.find(tableName);
	if (tableIterator != __tableMap.end() && !tableIterator -> second.resultHandler())  {
		std::unique_ptr<std::list<std::string>> valueList_ptr = tableIterator -> second.tupleValues(internalID);
		if (!valueList_ptr -> empty()) {
			std::string sql_command = "delete from " + __schemaName + "." + tableName + " where ";
			std::unique_ptr<std::list <std::string>> columnsName_ptr = tableIterator -> second.columnsName();
			std::list <std::string>::const_iterator columnsName_iterator = columnsName_ptr -> begin();
			std::list <std::string>::const_iterator valueList_iterator = valueList_ptr -> begin();
			unsigned numberOfColumns = tableIterator -> second.numberOfColumns();
			unsigned numberofKeys = 0;

			for (unsigned i = 0; i < numberOfColumns; i++, columnsName_iterator++, valueList_iterator++)
				if (tableIterator -> second.isKey(*columnsName_iterator)) {
					if (numberofKeys > 0) sql_command += " and ";
					sql_command += (*columnsName_iterator) + "=";
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) sql_command +="'";
					sql_command += (*valueList_iterator);
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) sql_command +="'";
					numberofKeys++;
				}

			if (numberofKeys == 0) {
				columnsName_iterator = columnsName_ptr -> begin();
				valueList_iterator = valueList_ptr -> begin();
				for (unsigned i = 0; i < numberOfColumns; i++, columnsName_iterator++, valueList_iterator++) {
					if (numberofKeys > 0) sql_command += " and ";
					sql_command += (*columnsName_iterator) + "=";
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) sql_command +="'";
					sql_command += (*valueList_iterator);
					if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar) sql_command +="'";
					numberofKeys++;
				}
			}
			return sql_command;
		}
	}
	return "";
}

std::string schema::selectSql (std::string tableName) const throw (exception&) {

	std::map <std::string, table>::const_iterator tableIterator = find(tableName);

	if (tableIterator != __tableMap.end() && !tableIterator -> second.resultHandler()) {

		std::unique_ptr<std::list <std::string>> columnsName_ptr = tableIterator -> second.columnsName();
		std::list <std::string>::const_iterator columnsName_iterator = columnsName_ptr -> begin();
		unsigned lenght = columnsName_ptr -> size();

		std::string project = "";
		bool project_andNeeded = false;
		std::string select = "";
		bool select_andNeeded = false;
		std::string orderBy = "";
		bool orderBy_andNeeded = false;

		for (unsigned i = 0; i < lenght; i++, columnsName_iterator++) {

			if (tableIterator->second.project((*columnsName_iterator))) {
				if (project_andNeeded)
					project += ", ";
				project += (*columnsName_iterator);
				project_andNeeded = true;
			}

			if (tableIterator -> second.select((*columnsName_iterator))) {
				if (select_andNeeded)
					select += " and ";
				select += (*columnsName_iterator) + " = ";

				if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar)
					select += "'";

				select += tableIterator -> second.selectValue((*columnsName_iterator));

				if (tableIterator -> second.columnType((*columnsName_iterator)) == column::varchar)
					select += "'";

				select_andNeeded = true;
			}

			if (tableIterator -> second.orderBy(*columnsName_iterator)) {
				if (orderBy_andNeeded)
					orderBy += ", ";
				orderBy += (*columnsName_iterator);
				if (tableIterator->second.orderMode(*columnsName_iterator) == column::desc)
					orderBy += " desc";
				else
					orderBy += " asc";
				orderBy_andNeeded = true;
			}
		}

		std::string sqlCommand = "select ";

		if (project == "") {
			std::unique_ptr<std::list<std::string>> __columnList = columnsName(tableName, false);
			for (std::list<std::string>::const_iterator list_it = __columnList->begin(); list_it != __columnList->end(); list_it++) {
				if (list_it != __columnList->begin()) project += ", ";
				project += *list_it;
			}
		}

		sqlCommand += project + " from ";

		if (__schemaName != "")
			sqlCommand += __schemaName + ".";
		sqlCommand += tableName;

		if (select != "")
			sqlCommand += " where " + select;

		if (orderBy != "")
			sqlCommand += " order by " + orderBy;

		return sqlCommand;
	}
	else
		return "";
}

std::unique_ptr<std::list<schema::loadCommand_info>> schema::loadCommand() const throw () {
	std::unique_ptr<std::list<loadCommand_info>> queryList_ptr(new std::list<loadCommand_info>);
	for (std::map <std::string, table>::const_iterator it = __tableMap.begin(); it != __tableMap.end(); it++) {
		if (!it -> second.resultHandler())
			queryList_ptr -> push_back(loadCommand(it->first));
	}
	return queryList_ptr;
}

schema::loadCommand_info schema::loadCommand (std::string tableName) const throw (exception&) {
	std::map <std::string, table>::const_iterator it = find(tableName);
	if (it -> second.resultHandler())
		throw exception(exception::structural, "Unable create load information for table '" + tableName + "': does not make sense to generate a load query for tables that stores the result of a query!");
	loadCommand_info	__info;
	__info.__tablePath = __schemaName + "." + it->second.tableName();
	__info.__loadCommand = selectSql(it->second.tableName());
	return __info;
}

std::unique_ptr<std::list<std::string>> schema::commit () const throw () {
	std::unique_ptr<std::list<std::string>> queryList_ptr(new std::list<std::string>);
	for (std::map <std::string, table>::const_iterator it = __tableMap.begin(); it != __tableMap.end(); it++) {
		if (!it -> second.resultHandler()) {
			std::unique_ptr<std::list <unsigned long>> internalIDList_ptr(it -> second.tuplesInternalID());
			for (std::list <unsigned long>::const_iterator tuple_it = internalIDList_ptr -> begin(); tuple_it != internalIDList_ptr -> end(); tuple_it++) {
				switch(it -> second.tupleStatus(*tuple_it)) {
					case tuple::inserting :
						queryList_ptr -> push_back(insertSql(it -> second.tableName(), *tuple_it));
						break;

					case tuple::updating :
						queryList_ptr -> push_back(updateSql(it -> second.tableName(), *tuple_it));
						break;

					case tuple::deleting :
						queryList_ptr -> push_back(deleteSql(it -> second.tableName(), *tuple_it));
						break;

					default : break;
				}
			}
		}
	}
	return queryList_ptr;
}

std::unique_ptr<std::list<std::string>> schema::commit (std::string tableName) const throw (exception&) {
	std::map <std::string, table>::const_iterator table_it = find(tableName);
	std::unique_ptr<std::list<std::string>> queryList_ptr(new std::list<std::string>);
	if (!table_it -> second.resultHandler()) {
		std::unique_ptr<std::list <unsigned long>> internalIDList_ptr = table_it -> second.tuplesInternalID();
		for (std::list <unsigned long>::const_iterator tuple_it = internalIDList_ptr -> begin(); tuple_it != internalIDList_ptr -> end(); tuple_it++) {
			switch(table_it -> second.tupleStatus(*tuple_it)) {
				case tuple::inserting :
					queryList_ptr -> push_back(insertSql(tableName, *tuple_it));
					break;

				case tuple::updating :
					queryList_ptr -> push_back(updateSql(tableName, *tuple_it));
					break;

				case tuple::deleting :
					queryList_ptr -> push_back(deleteSql(tableName, *tuple_it));
					break;

				default : break;
			}
		}
	}
	return queryList_ptr;
}

schema::pathResolver::pathResolver (std::string path) : resolvedTable(""), resolvedColumn("") {
	unsigned pointOccurrences = (unsigned) std::count(path.begin(), path.end(), '.');
	if (pointOccurrences == 1) {
		unsigned pos = path.find_first_of('.');
		resolvedTable = path.substr(0, pos);
		resolvedColumn = path.substr(pos + 1, path.size());
	}
}

std::map <std::string, table>::const_iterator schema::find (std::string tableName) const throw (exception&) {
	std::map <std::string, table>::const_iterator it = __tableMap.find(tableName);
	if (it != __tableMap.end())
		return it;
	else
		throw exception(exception::noTable, "Unable to access table '" + tableName + "': no table with this name in schema '" + __schemaName + "'!");
}

std::map <std::string, table>::iterator schema::find (std::string tableName) throw (exception&) {
	std::map <std::string, table>::iterator it = __tableMap.find(tableName);
	if (it != __tableMap.end())
		return it;
	else
		throw exception(exception::noTable, "Unable to access table '" + tableName + "': no table with this name in schema '" + __schemaName + "'!");

}
