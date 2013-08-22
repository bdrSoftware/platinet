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

#include "database.hpp"
#include "time.hpp"
#include <algorithm>
#include <fstream>
using namespace engine;

#if !defined __WINDOWS_COMPILING_
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#else

#endif

const std::string database::__tmpBasePath = "platinet";

database::database (std::string databaseName) throw () : __databaseName(databaseName) {
#if !defined __WINDOWS_COMPILING_
	__tmpStoragePath = "/tmp/" + __tmpBasePath + "/";
	mkdir(__tmpStoragePath.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	__tmpStoragePath += std::to_string(getpid()) + "/";
	mkdir(__tmpStoragePath.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	__tmpStoragePath += __databaseName + "/";
	mkdir(__tmpStoragePath.c_str(),  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
#endif
}

std::unique_ptr<std::list<std::string>> database::schemasName(bool shortFormat) const throw () {
	std::unique_ptr<std::list<std::string>> _list(new std::list <std::string>);
	std::map <std::string, schema>::const_iterator it;
	for (it = __schemaMap.begin(); it != __schemaMap.end(); it++)
		if (!shortFormat && __databaseName != "")
			_list -> push_back( __databaseName + "." + (it -> first));
		else
			_list -> push_back(it -> first);
	return _list;
}

void database::addSchema (std::string schemaName) throw () {
	if (schemaName != "" && __schemaMap.count(schemaName) == 0)
		__schemaMap.insert(std::pair<std::string, schema>(schemaName, schema(schemaName, __tmpStoragePath)));
}

bool database::findSchema (std::string schemaName) const throw () {
	if (__schemaMap.count(schemaName) == 1)
		return true;
	else
		return false;
}

void database::dropSchema (std::string schemaName) throw (exception&) {
	std::map <std::string, schema>::iterator it = __schemaMap.find(schemaName);
	if (it != __schemaMap.end())
		__schemaMap.erase(it);
	else
		throw exception(exception::noSchema, "Unable to access schema '" + schemaName + "': no schema with this name in database '" + __databaseName + "'!");
}

//~ std::unique_ptr<std::list<std::string>> database::tablesName (std::string schemaName, bool shortFormat) const throw (exception&) {
	//~ std::map <std::string, schema>::const_iterator it = __schemaMap.find(schemaName);
	//~ if (it != __schemaMap.end()) {
		//~ std::unique_ptr<std::list<std::string>> _list(new std::list <std::string>);
		//~ if (!shortFormat && __databaseName != "") {
			//~ _list = it -> second.tablesName(false);
			//~ std::list <std::string>::iterator it;
			//~ for (it = _list -> begin(); it != _list -> end(); it++)
				//~ (*it) = __databaseName + "." + (*it);
		//~ }
		//~ else
			//~ _list = it -> second.tablesName();
		//~ return _list;
	//~ }
	//~ else
		//~ throw exception(exception::noSchema, "Unable to access schema '" + schemaName + "': no schema with this name in database '" + __databaseName + "'!");
//~ }

//~ std::unique_ptr<std::list<std::string>> database::columnsName (std::string schemaName, std::string tableName, bool shortFormat) const throw (exception&) {
	//~ std::map <std::string, schema>::const_iterator it = __schemaMap.find(schemaName);
	//~ if (it != __schemaMap.end()) {
		//~ std::unique_ptr<std::list<std::string>> _list (new std::list <std::string>);
		//~ if (!shortFormat && __databaseName != "") {
			//~ _list = it -> second.columnsName(tableName, false);
//~ 
			//~ std::list <std::string>::iterator it;
			//~ for (it = _list -> begin(); it != _list -> end(); it++)
				//~ (*it) = __databaseName + "." + (*it);
		//~ }
		//~ else
			//~ _list = it -> second.columnsName(tableName);
//~ 
		//~ return _list;
	//~ }
	//~ else
		//~ throw exception(exception::noSchema, "Unable to access schema '" + schemaName + "': no schema with this name in database '" + __databaseName + "'!");
//~ }

std::unique_ptr<std::list<std::string>> database::structure() const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list <std::string>);
	for(std::map <std::string, schema>::const_iterator it = __schemaMap.begin(); it != __schemaMap.end(); it++) {
		std::unique_ptr<std::list <std::string>> tmp = it->second.structure();
		if (!tmp->empty())
			list_ptr->insert(list_ptr->end(), tmp->begin(), tmp->end());
	}
	return list_ptr;
}

std::unique_ptr<std::list <schema::loadCommand_info>> database::loadCommand () const throw () {
	std::unique_ptr<std::list <schema::loadCommand_info>> list_ptr(new std::list <schema::loadCommand_info>);
	for(std::map <std::string, schema>::const_iterator it = __schemaMap.begin(); it != __schemaMap.end(); it++) {
		std::unique_ptr<std::list <schema::loadCommand_info>> tmp = it->second.loadCommand();
		if (!tmp->empty())
			list_ptr->insert(list_ptr->end(), tmp->begin(), tmp->end());
	}
	return list_ptr;
}

std::unique_ptr<std::list<std::string>> database::commit() const throw () {
	std::unique_ptr<std::list<std::string>> queryList_ptr(new std::list <std::string>);
	for (std::map <std::string, schema>::const_iterator it = __schemaMap.begin(); it != __schemaMap.end(); it++) {
		std::unique_ptr<std::list<std::string>> partial_ptr = it -> second.commit();
		queryList_ptr -> merge(*partial_ptr);
	}
	return queryList_ptr;
}

database::pathResolver::pathResolver (std::string path, pathType type) : resolvedSchema(""), resolvedTable(""), resolvedColumn("") {
	if (type == column) {
		unsigned pointOccurrences = (unsigned) std::count(path.begin(), path.end(), '.');
		if (pointOccurrences == 2) {
			unsigned first_pos = path.find_first_of('.');
			unsigned last_pos = path.find_last_of('.');
			resolvedSchema = path.substr(0, first_pos);
			resolvedTable = path.substr(first_pos + 1, last_pos - first_pos - 1);
			resolvedColumn = path.substr(last_pos + 1, path.size());
		}
		else
			if (pointOccurrences == 1) {
				unsigned pos = path.find_first_of('.');
				resolvedSchema = "";
				resolvedTable = path.substr(0, pos);
				resolvedColumn = path.substr(pos + 1, path.size());
			}
	}
	else {
		unsigned pointOccurrences = (unsigned) std::count(path.begin(), path.end(), '.');
		if (pointOccurrences == 1) {
			unsigned pos = path.find_first_of('.');
			resolvedSchema = path.substr(0, pos);
			resolvedTable = path.substr(pos + 1, path.size());
		}
		else
			resolvedTable = path;

	}
}

std::map <std::string, schema>::const_iterator database::find (std::string schemaName) const throw (exception&) {
	std::map <std::string, schema>::const_iterator it = __schemaMap.find(schemaName);
	if (it != __schemaMap.end())
		return it;
	else
		throw exception(exception::noSchema, "Unable to access schema '" + schemaName + "': no schema with this name in database '" + __databaseName + "'!");
}

std::map <std::string, schema>::iterator database::find (std::string schemaName) throw (exception&) {
	std::map <std::string, schema>::iterator it = __schemaMap.find(schemaName);
	if (it != __schemaMap.end())
		return it;
	else
		throw exception(exception::noSchema, "Unable to access schema '" + schemaName + "': no schema with this name in database '" + __databaseName + "'!");
}

