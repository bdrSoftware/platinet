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

#ifndef _SCHEMA_
#define _SCHEMA_

#include "table.hpp"
#include <map>

namespace engine {
class schema {
public :
	schema(std::string schemaName, std::string tmpStorageBase) throw ();

	/**		MEMBRI RELATIVI ALLA STRUTTURA DELLO SCHEMA	**/
	std::string	 								schemaName		()			 						const 	throw ()	{return __schemaName;}
	void										schemaName		(std::string newName) 						throw ()	{__schemaName = newName;}
	unsigned									numberOfTables	() 									const	throw ()	{return __tableMap.size();}
	std::unique_ptr<std::list <std::string>>	tablesName		(bool shortFormat = true) 			const	throw ();

	void	addTable	(std::string tableName)			throw ();
	void	addTable	(const table& _table)			throw (exception&);
	void	dropTable	(std::string tableName)			throw ();
	bool	findTable	(std::string tableName)	const	throw ();

	/**		MEMBRI RELATIVI ALLA STRUTTURA DELLA TABELLA	**/
	bool										resultHandler		(std::string tableName)										const	throw (exception&)	{return find(tableName) -> second.resultHandler();}
	unsigned									numberOfColumns		(std::string tableName)										const	throw (exception&)	{return find(tableName) -> second.numberOfColumns();}
	std::unique_ptr<std::list<std::string>>		columnsName			(std::string tableName, bool shortFormat = true)			const	throw (exception&);

	void addColumn(std::string tableName,
			std::string columnName,
			column::sqlType columnType,
			unsigned length = 0,
			bool isKey = false,
			std::string defaultValue = ""
	) throw (exception&)	{find(tableName) -> second.addColumn(columnName, columnType, length, isKey, defaultValue);}

	void addColumn(	std::string columnPath,
			column::sqlType columnType,
			unsigned length = 0,
			bool isKey = false,
			std::string defaultValue = ""
	) throw (exception&)	{pathResolver resolvedPath(columnPath); addColumn(resolvedPath.tableName(), resolvedPath.columnName(), columnType, length, isKey, defaultValue);}

	std::string 								structure(std::string tableName)	const	throw (exception&)	{return find(tableName) -> second.structure();}
	std::unique_ptr<std::list<std::string>>		structure()							const	throw ();

	bool	findColumn	(std::string tableName, std::string columnName)		const	throw (exception&)	{return find(tableName) -> second.findColumn(columnName);}
	bool	findColumn	(std::string columnPath)							const	throw (exception&)	{pathResolver resolvedPath(columnPath); return findColumn(resolvedPath.tableName(), resolvedPath.columnName());}
	void	dropColumn	(std::string tableName, std::string columnName)				throw (exception&)	{find(tableName) -> second.dropColumn(columnName);}
	void	dropColumn	(std::string columnPath)									throw (exception&)	{pathResolver resolvedPath(columnPath); dropColumn(resolvedPath.tableName(), resolvedPath.columnName());}

	/** 		METODI CHE PERMETTONO L'ACCESSO E MODIFICA DELLA STRUTTURA DELLE COLONNE **/
	column::sqlType		columnType		(std::string tableName, std::string columnName)							const	throw (exception&)	{return find(tableName) -> second.columnType(columnName);}
	column::sqlType		columnType		(std::string columnPath) 												const	throw (exception&)	{pathResolver resolvedPath(columnPath); return columnType(resolvedPath.tableName(), resolvedPath.columnName());}
	unsigned 			length			(std::string tableName, std::string columnName)							const	throw (exception&)	{return find(tableName) -> second.length(columnName);}
	unsigned 			length			(std::string columnPath) 												const	throw (exception&)	{pathResolver resolvedPath(columnPath); return length(resolvedPath.tableName(), resolvedPath.columnName());}
	std::string			defaultValue	(std::string tableName, std::string columnName)							const	throw (exception&)	{return find(tableName) -> second.defaultValue(columnName);}
	void				defaultValue	(std::string tableName, std::string columnName, std::string newValue)			throw (exception&)	{find(tableName) -> second.defaultValue(columnName, newValue);}
	std::string			defaultValue	(std::string columnPath)												const	throw (exception&)	{pathResolver resolvedPath(columnPath); return defaultValue(resolvedPath.tableName(), resolvedPath.columnName());}
	void				defaultValue	(std::string columnPath, std::string newValue)									throw (exception&)	{pathResolver resolvedPath(columnPath); return defaultValue(resolvedPath.tableName(), resolvedPath.columnName(), newValue);}
	bool 				isKey			(std::string tableName, std::string columnName)							const	throw (exception&)	{return find(tableName) -> second.isKey(columnName);}
	void 				isKey			(std::string tableName, std::string columnName, bool key)						throw (exception&)	{find(tableName) -> second.isKey(columnName, key);}
	bool 				isKey			(std::string columnPath) 												const	throw (exception&)	{pathResolver resolvedPath(columnPath); return isKey(resolvedPath.tableName(), resolvedPath.columnName());}
	void 				isKey			(std::string columnPath, bool key)												throw (exception&)	{pathResolver resolvedPath(columnPath); isKey(resolvedPath.tableName(), resolvedPath.columnName(), key);}

			/** METODI RIGUARDANTI I MEMBRI USATI PER LE QUERY **/
	bool 	project			(std::string tableName, std::string columnName) 				const	throw (exception&)	{return find(tableName) -> second.project(columnName);}
	void 	project			(std::string tableName, std::string columnName, bool value)				throw (exception&)	{find(tableName) -> second.project(columnName, value);}
	bool 	project			(std::string columnPath) 										const	throw (exception&)	{pathResolver resolvedPath(columnPath); return project(resolvedPath.tableName(), resolvedPath.columnName());}
	void 	project			(std::string columnPath, bool value)									throw (exception&)	{pathResolver resolvedPath(columnPath); project(resolvedPath.tableName(), resolvedPath.columnName(), value);}
	void	cleanProject	(std::string tableName)													throw (exception&)	{find(tableName) -> second.cleanProject();}

	bool 	select		(std::string tableName, std::string columnName) 				const	throw (exception&)	{return find(tableName) -> second.select(columnName);}
	void	select		(std::string tableName, std::string columnName, bool value)				throw (exception&)	{find(tableName) -> second.select(columnName, value);}
	bool 	select		(std::string columnPath) 										const	throw (exception&)	{pathResolver resolvedPath(columnPath); return select(resolvedPath.tableName(), resolvedPath.columnName());}
	void	select		(std::string columnPath, bool value)									throw (exception&)	{pathResolver resolvedPath(columnPath); select(resolvedPath.tableName(), resolvedPath.columnName(), value);}
	void	cleanSelect	(std::string tableName)													throw (exception&)	{find(tableName) -> second.cleanSelect();}

	std::string 	selectValue			(std::string tableName, std::string columnName) 					const	throw (exception&)	{return find(tableName) -> second.selectValue(columnName);}
	void 			selectValue			(std::string tableName, std::string columnName, std::string value)			throw (exception&)	{find(tableName) -> second.selectValue(columnName, value);}
	std::string 	selectValue			(std::string columnPath) 											const	throw (exception&)	{pathResolver resolvedPath(columnPath); return selectValue(resolvedPath.tableName(), resolvedPath.columnName());}
	void 			selectValue			(std::string columnPath, std::string value)									throw (exception&)	{pathResolver resolvedPath(columnPath); selectValue(resolvedPath.tableName(), resolvedPath.columnName(), value);}
	void			cleanSelectValue	(std::string tableName)														throw (exception&)	{find(tableName) -> second.cleanSelectValue();}

	enum column::sqlCompOp 	compareOperator			(std::string tableName, std::string columnName) 						const	throw (exception&)	{return find(tableName) -> second.compareOperator(columnName);}
	void 					compareOperator			(std::string tableName, std::string columnName, column::sqlCompOp op)			throw (exception&)	{find(tableName) -> second.compareOperator(columnName, op);}
	enum column::sqlCompOp	compareOperator			(std::string columnPath) 												const	throw (exception&)	{pathResolver resolvedPath(columnPath); return compareOperator(resolvedPath.tableName(), resolvedPath.columnName());}
	void 					compareOperator			(std::string columnPath, column::sqlCompOp op)									throw (exception&)	{pathResolver resolvedPath(columnPath); compareOperator(resolvedPath.tableName(), resolvedPath.columnName(), op);}
	void					cleanCompareOperator	(std::string tableName)															throw (exception&)	{find(tableName) -> second.cleanCompareOperator();}

	bool 	orderBy			(std::string tableName, std::string columnName) 			const	throw (exception&)	{return find(tableName) -> second.orderBy(columnName);}
	void 	orderBy			(std::string tableName, std::string columnName, bool value)			throw (exception&)	{find(tableName) -> second.orderBy(columnName, value);}
	bool 	orderBy			(std::string columnPath) 									const	throw (exception&)	{pathResolver resolvedPath(columnPath); return orderBy(resolvedPath.tableName(), resolvedPath.columnName());}
	void 	orderBy			(std::string columnPath, bool value)								throw (exception&)	{pathResolver resolvedPath(columnPath); orderBy(resolvedPath.tableName(), resolvedPath.columnName(), value);}
	void	cleanOrderBy	(std::string tableName)												throw (exception&)	{find(tableName) -> second.cleanOrderBy();}

	enum column::sqlOrder	orderMode		(std::string tableName, std::string columnName) 						const	throw (exception&)  {return find(tableName) -> second.orderMode(columnName);}
	void 					orderMode		(std::string tableName, std::string columnName, column::sqlOrder order)			throw (exception&)	{find(tableName) -> second.orderMode(columnName, order);}
	enum column::sqlOrder	orderMode		(std::string columnPath) 												const	throw (exception&)	{pathResolver resolvedPath(columnPath); return orderMode(resolvedPath.tableName(), resolvedPath.columnName());}
	void 					orderMode		(std::string columnPath, column::sqlOrder order)								throw (exception&)	{pathResolver resolvedPath(columnPath); orderMode(resolvedPath.tableName(), resolvedPath.columnName(), order);}
	void					cleanOrderMode	(std::string tableName)															throw (exception&)	{find(tableName) -> second.cleanOrderMode();}

	void	cleanAll	(std::string tableName)	throw (exception&)		{find(tableName) -> second.cleanAll();}

	/**		MEMBRI RELATIVI ALL' INSERIMENTO, MODOFICA E CANCELLAZIONE DI TUPLE DALLA TABELLA 	**/
	unsigned long								numberOfTuples		(std::string tableName)		const	throw (exception&)	{return find(tableName) -> second.resultHandler();}
	std::unique_ptr<std::list<unsigned long>>	tuplesInternalID	(std::string tableName)		const	throw (exception&)	{return find(tableName) -> second.tuplesInternalID();}
	void										clear				(std::string tableName)				throw (exception&)	{find(tableName) -> second.clear();}
	
	unsigned long		insertTuple		(std::string tableName, const std::list<std::string>& values, enum tuple::state _status = tuple::inserting)			throw (exception&)  {return find(tableName) -> second.insertTuple(values, _status);}
	void				updateTuple		(std::string tableName, unsigned long internalID, const std::list<std::string>& values)								throw (exception&)	{find(tableName) -> second.updateTuple(internalID, values);}
	void				deleteTuple		(std::string tableName, unsigned long internalID)																	throw (exception&)  {find(tableName) -> second.deleteTuple(internalID);}
	void				eraseTuple		(std::string tableName, unsigned long internalID)																	throw (exception&)	{find(tableName) -> second.eraseTuple(internalID);}
	enum tuple::state	tupleStatus		(std::string tableName, unsigned long internalID)															const	throw (exception&)  {return find(tableName) -> second.tupleStatus(internalID);}
	bool				tupleVisible	(std::string tableName, unsigned long internalID) 															const	throw (exception&)	{return find(tableName) -> second.tupleVisible(internalID);}
	
	std::unique_ptr<std::list <std::string>>	tupleValues		(std::string tableName, unsigned long internalID)	const	throw (exception&)  {return find(tableName) -> second.tupleValues(internalID);}
	std::unique_ptr<std::list <std::string>>	tupleOldValues	(std::string tableName, unsigned long internalID)	const	throw (exception&)  {return find(tableName) -> second.tupleOldValues(internalID);}

	void	bulkInsertModel	(std::string tableName, std::string fileName)	const	throw (exception&)	{find(tableName) -> second.bulkInsertModel(fileName);}
	void	bulkInsert		(std::string tableName, std::string fileName)			throw (exception&)	{find(tableName) -> second.bulkInsert(fileName);}
	void	bulkUpdateModel	(std::string tableName, std::string fileName)	const	throw (exception&)	{find(tableName) -> second.bulkUpdateModel(fileName);}
	void	bulkUpdate		(std::string tableName, std::string fileName)			throw (exception&)	{find(tableName) -> second.bulkUpdate(fileName);}

	/**						MEMBRI DI UTILITÀ GENERALE					**/
	void	toHtml			(std::string tableName, std::string fileName)	const	throw (exception&) 	{find(tableName) -> second.toHtml(fileName);}
	
	/**					MEMBRI RELATIVI ALLE OPERAZIONI SQL SU DBMS REMOTO				**/
	std::string	insertSql	(std::string tableName, unsigned long internalID)	const	throw (exception&);
	std::string	updateSql	(std::string tableName, unsigned long internalID)	const	throw (exception&);
	std::string	deleteSql	(std::string tableName, unsigned long internalID)	const	throw (exception&);
	std::string selectSql	(std::string tableName)								const	throw (exception&);

	struct loadCommand_info{
		std::string		__tablePath;
		std::string		__loadCommand;
	};
	std::unique_ptr<std::list<loadCommand_info>> 	loadCommand	()							const	throw ();
	loadCommand_info							 	loadCommand	(std::string tableName)		const	throw (exception&);
	std::unique_ptr<std::list<std::string>>			commit		()							const	throw ();
	std::unique_ptr<std::list<std::string>>			commit		(std::string tableName)		const	throw (exception&);

	//path resolver è un membro pensato per risolvere coerentemente il percorso di una colonna di una tabella dello schema
	class pathResolver {
	public :
		explicit pathResolver (std::string path);
		std::string tableName() 	const 	{return resolvedTable;}
		std::string columnName() 	const 	{return resolvedColumn;}

	private :
		std::string resolvedTable;
		std::string resolvedColumn;
	};

private :
	std::string 					__schemaName;
	std::string						__tmpStoragePath;
	std::map <std::string, table> 	__tableMap;

	std::map <std::string, table>::const_iterator	find	(std::string tableName)	const	throw (exception&);
	std::map <std::string, table>::iterator			find	(std::string tableName)			throw (exception&);
};
};
#endif
