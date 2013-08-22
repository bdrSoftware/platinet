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

#include "table.hpp"
#include "time.hpp"
#include <fstream>
#include <thread>
#include <iostream>
using namespace engine;

table::table(std::string tableName, std::string tmpStorageBase, bool resultHandler, storageType _storage) throw() : __tableName(tableName), __tmpStorageBase(tmpStorageBase + __tableName), __resultHandler(resultHandler) {
	if (_storage == memory)
		__tupleStorage = new mStorage;
	else
		__tupleStorage = new fStorage(__tmpStorageBase);
}

std::unique_ptr<std::list <std::string>> table::columnsName (bool shortFormat) const throw () {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list<std::string>);
	std::list <column>::const_iterator it;
	for (it = __columnList.begin(); it != __columnList.end(); it++)
		if (!shortFormat)
			list_ptr -> push_back(__tableName + "." + (it -> columnName()));
		else
			list_ptr -> push_back(it -> columnName());
	return list_ptr;
}

std::string table::structure () const throw() {
	std::string _structure = "table '" + __tableName +"':";
	for (std::list<column>::const_iterator it = __columnList.begin(); it != __columnList.end(); it++) {
		_structure += "\n\t" + it -> columnName() + "\t";
		switch (it -> columnType()) {
			case column::boolean:
				_structure += "boolean;";
				break;
			case column::varchar:
				_structure = _structure + "varchar (" + std::to_string(it -> length()) + ")";
				break;
			case column::bigint :
				_structure += "big integer;";
				break;
			case column::integer:
				_structure += "integer;";
				break;
			case column::smallint:
				_structure += "small integer;";
				break;
			case column::doublePrecision:
				_structure += "double precision floating point;";
				break;
			case column::real:
				_structure += "single precision floating point;";
				break;
			case column::date:
				_structure += "date in 'YYYY-MM-DD' format;";
				break;
			case column::time:
				_structure += "time without timezone in 'HH:MM:SS' format;";
				break;
		}
		if (it -> defaultValue() != "")
			_structure += "\t default value is '" + it -> defaultValue() +"';";
		if (it -> isKey())
			_structure += "\t involved in key constraint.";
	}
	return _structure;
}

void table::addColumn(std::string columnName, column::sqlType columnType, unsigned length, bool isKey, std::string defaultValue) throw () {
	if (columnName != "" && !findColumn(columnName) && __tupleStorage -> numTuples() == 0)
		__columnList.push_back(column(columnName, columnType, length, isKey, defaultValue));
}

bool table::findColumn(std::string columnName) const throw () {
	std::list <column>::const_iterator iterator = __columnList.begin();
	bool found = false;
	while (iterator != __columnList.end() && !found)
		if (iterator -> columnName() == columnName)
			found = true;
		else
			iterator++;
	return found;
}

void table::dropColumn(std::string columnName)  throw () {
	std::list <column>:: iterator it = find(columnName);
	if (it != __columnList.end())
		__columnList.erase(it);
}

void table::cleanProject () throw () {
	std::list <column>::iterator it;
	for (it = __columnList.begin(); it != __columnList.end(); it++)
		it -> project(false);
}

void table::cleanSelect () throw () {
	std::list <column>::iterator it;
	for (it = __columnList.begin(); it != __columnList.end(); it++)
		it -> select(false);
}

void table::cleanSelectValue () throw () {
	std::list <column>::iterator it;
	for (it = __columnList.begin(); it != __columnList.end(); it++)
		it -> selectValue("");
}

void table::cleanCompareOperator () throw () {
	std::list <column>::iterator it;
	for (it = __columnList.begin(); it != __columnList.end(); it++)
		it -> compareOperator(column ::equal);
}

void table::cleanOrderBy () throw () {
	std::list <column>::iterator it;
	for (it = __columnList.begin(); it != __columnList.end(); it++)
		it -> orderBy(false);
}

void table::cleanOrderMode () throw () {
	std::list <column>::iterator it;
	for (it = __columnList.begin(); it != __columnList.end(); it++)
		it -> orderMode(column::asc);
}

void table::cleanAll () throw () {
	cleanProject();
	cleanSelect();
	cleanSelectValue();
	cleanCompareOperator();
	cleanOrderBy();
	cleanOrderMode();
}

void table::bulkInsertModel (std::string fileName) const throw (exception&) {
	std::thread thr(&table::bulkInsertModel_nothr, std::ref(*this),fileName);
	thr.detach();
}

void table::bulkInsert (std::string fileName) throw (exception&) {
	std::thread thr(&table::bulkInsert_nothr, std::ref(*this),fileName);
	thr.detach();
}

void table::bulkUpdateModel (std::string fileName) const	throw (exception&) {
	std::thread thr(&table::bulkUpdateModel_nothr, std::ref(*this),fileName);
	thr.detach();
}

void table::bulkUpdate (std::string fileName) throw (exception&) {
	std::thread thr(&table::bulkUpdate_nothr, std::ref(*this),fileName);
	thr.detach();
}

void table::bulkInsertModel_nothr	(std::string fileName) const throw (exception&) {
	if (fileName == "")
		fileName = std::string(__tableName + ".txt");
	std::ofstream stream;
	stream.open(fileName.c_str());
	//~ if (!stream.is_open())
		//~ throw exception (exception::fileOpening, "Can't open file '" + fileName +"' while creating  bulk insert model for '" + __tableName + "' table");

	if (stream.is_open()) {
		for (std::list<column>::const_iterator it = __columnList.begin(); it != __columnList.end(); it++)
			stream << it -> columnName().c_str() <<"\t";
		stream << std::endl;
		stream.close();
	}
}

void  table::bulkInsert_nothr (std::string fileName) throw (exception&) {
	if (fileName == "")
		fileName = std::string(__tableName + ".txt");
	std::ifstream stream;
	stream.open(fileName.c_str());
	//~ if (!stream.is_open())
		//~ throw exception (exception::fileOpening, "Can't open file '" + fileName +"' while executing  bulk insert for '" + __tableName + "' table");

	std::string	errorLog = __tableName + "_bulkInsert_log.txt";
	std::ofstream log;
	log.open(errorLog.c_str());

	if (stream.is_open()) {
		std::string _string;
		unsigned rowReaded = 0;
		while (stream.is_open() && !stream.eof()) {
			getline(stream, _string);
			if (_string != "" && rowReaded > 0) {
				std::unique_ptr<std::list<std::string>> string_ptr = tokenize(_string);
				try {__tupleStorage -> insert(*string_ptr, __columnList);}
				catch(exception& excpt) {
					if (excpt.code() == exception::keyNull)
						log <<fileName <<" row " <<std::to_string(rowReaded + 1) <<": "  <<excpt.what() <<std::endl;
					else {
						stream.close();
						log.close();
						throw;
					}
				}
			}
			rowReaded++;
		}
		stream.close();
	}
	else
		log << "Can't open file '" << fileName << "' while executing bulk update for '" << __tableName << "' table\n";
	log.close();
}

void table::bulkUpdateModel_nothr	(std::string fileName) const throw (exception&) {
	if (fileName == "")
		fileName = std::string(__tableName + ".txt");
	std::ofstream stream;
	stream.open(fileName.c_str());
	//~ if (!stream.is_open())
		//~ throw exception (exception::fileOpening, "Can't open file '" + fileName +"' while creating  bulk update model for '" + __tableName + "' table");
	if (stream.is_open()) {
		stream <<"internalID" <<"\t";
		for (std::list<column>::const_iterator it = __columnList.begin(); it != __columnList.end(); it++)
			stream << it -> columnName().c_str() <<"\t";
		stream << std::endl;
	
		std::unique_ptr<std::list<unsigned long>> internalID_list = __tupleStorage -> internalID();
		for (std::list<unsigned long>::const_iterator tID_it = internalID_list -> begin(); tID_it != internalID_list -> end(); tID_it++) {
			std::unique_ptr<std::list<std::string>> tupleValues_list = __tupleStorage -> value(*tID_it);
			stream << *tID_it;
			for (std::list<std::string>::const_iterator it = tupleValues_list -> begin(); it != tupleValues_list -> end(); it++)
				stream <<"\t" << it->c_str() ;
			stream << std::endl;
		}
		stream.close();
	}
}

void table::bulkUpdate_nothr (std::string fileName) throw (exception&) {
	if (fileName == "")
		fileName = std::string(__tableName + ".txt");
	std::ifstream stream;
	stream.open(fileName.c_str());
	//~ if (!stream.is_open())
		//~ throw exception (exception::fileOpening, "Can't open file '" + fileName + "' while executing bulk update for '" + __tableName + "' table");

	std::string errorLog = __tableName + "_bulkUpdate_log.txt";
	std::ofstream log;
	log.open(errorLog.c_str());

	if (stream.is_open()) {
		std::string _string;
		unsigned rowReaded = 0;
		while (stream.is_open() && !stream.eof()) {
			getline(stream, _string);
			if (_string != "" && rowReaded > 0) {
				std::unique_ptr<std::list<std::string>> string_ptr = tokenize(_string);
				unsigned long tuple_key = std::stoul(*(string_ptr -> begin()));
				string_ptr -> pop_front();
				try {
					__tupleStorage -> update(tuple_key, *string_ptr, __columnList);
				}
				catch(exception& excpt) {
					if (excpt.code() == exception::noID)
						log <<fileName <<" row " <<std::to_string(rowReaded + 1) <<": "  <<excpt.what() <<std::endl;
					else {
						stream.close();
						log.close();
						throw;
					}
				}
			}
			rowReaded++;
		}
		stream.close();
	}
	else
		log << "Can't open file '" << fileName << "' while executing bulk update for '" << __tableName << "' table\n";
	log.close();
}

void table::toHtml_nothr(std::string fileName) const throw (exception&) {
	if (fileName == "")
		fileName = __tableName + ".txt";
	std::ofstream stream;
	stream.open(fileName.c_str());
	//~ if (!stream.is_open())
		//~ throw exception (exception::fileOpening, "Can't open file '" + fileName +"' while exporting table '"+ __tableName + "' to html");
	if (stream.is_open()) {
		stream <<"<html>" <<std::endl
		<<"<head> <h3> Nome tabella: <b>" <<__tableName.c_str() << " </h3> </b></head>" <<std::endl
		<<"<body>" <<std::endl
		<<"<table border=\"1\">" <<std::endl;
	
		stream <<"<tr>" <<std::endl;
		std::list <column>::const_iterator columnIt;
		for ( columnIt = __columnList.begin(); columnIt != __columnList.end(); columnIt++)
			stream <<"<td><b>" << columnIt -> columnName().c_str() <<"</b></td>" <<std::endl;
		stream <<"</tr>" <<std::endl;
	
		std::unique_ptr<std::list <unsigned long>> internal = __tupleStorage -> internalID();
		for (std::list <unsigned long>::const_iterator it = internal -> begin(); it != internal -> end(); it++) {
			std::unique_ptr<std::list< std::string>> values_ptr = __tupleStorage -> value(*it);
			stream <<"<tr>" <<std::endl;
			for (std::list <std::string>::const_iterator list_it = values_ptr -> begin(); list_it != values_ptr -> end(); list_it++)
				stream <<"<td>" <<*list_it <<"</td>" <<std::endl;
	 		stream <<"</tr>" <<std::endl <<std::endl;
	 	}
	
		time _time;
	
		stream<<"</table>" <<std::endl
		<<"<p>Generato il: " <<_time.timeStamp().c_str()
		<<"</h5>" <<std::endl
		<<"</body>" <<std::endl
		<<"</html>";
		stream.close();
	}
}

void table::toHtml(std::string fileName) const throw (exception&) {
	std::thread thr(&table::toHtml_nothr, std::ref(*this),fileName);
	thr.detach();
}

std::list <column>::const_iterator table::find(std::string columnName) const throw (exception&) {
	std::list <column>::const_iterator result = __columnList.end();
	std::list <column>::const_iterator iterator = __columnList.begin();
	while (iterator != result)
		if (iterator -> columnName() == columnName)
			result = iterator;
		else
			iterator++;
	if (iterator != __columnList.end())
		return result;
	else
		throw exception(exception::noColumn, "No column '" + columnName + "' found in table " + __tableName);
}

std::list <column>::iterator table::find(std::string columnName) throw (exception&) {
	std::list <column>::iterator result = __columnList.end();
	std::list <column>::iterator iterator = __columnList.begin();
	while (iterator != result)
		if (iterator -> columnName() == columnName)
			result = iterator;
		else
			iterator++;
	if (iterator != __columnList.end())
		return result;
	else
		throw exception(exception::noColumn, "No column '" + columnName + "' found in table " + __tableName);
}

std::unique_ptr<std::list <std::string>> table::tokenize (std::string _string) const {
	std::unique_ptr<std::list <std::string>> list_ptr(new std::list <std::string>);
	std::string token;
	std::size_t tabPos = 0;
	do {
		tabPos = _string.find_first_of('\t', 0);
		switch (tabPos) {
		case std::string::npos:
			token = _string;
			break;
		case 0 :
			token = "";
			_string.erase(0, 1);
			break;
		default:
			token = _string.substr(0, tabPos);
			_string.erase(0, tabPos + 1);
		}
		list_ptr-> push_back(token);
	} while ( _string != token);
	return list_ptr;
}
