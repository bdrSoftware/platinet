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

#ifndef _REFERENCE_
#define _REFERENCE_

#include "schema.hpp"

namespace engine {
class database {
public :
	explicit database (std::string databaseName) throw ();

	/**		MEMBRI RELATIVI ALLA STRUTTURA DEL DATABASE	**/
	std::string 		databaseName	() 						const	throw ()	{return __databaseName;}
	unsigned			numberOfSchemas	() 						const	throw ()	{return __schemaMap.size();}
	void				clearStructure	()											{__schemaMap.clear();}
	
	std::unique_ptr<std::list <std::string>>	schemasName	(bool shortFormat = true)			const	throw ();
	void										addSchema	(std::string schemaName)					throw ();
	bool										findSchema	(std::string schemaName)			const	throw ();
	void										dropSchema	(std::string schemaName)					throw (exception&);

	/**		MEMBRI RELATIVI ALLA STRUTTURA DELLO SCHEMA			**/
	unsigned									numberOfTables	(std::string schemaName)									const	throw (exception&)	{return find(schemaName) -> second.numberOfTables();}
	std::unique_ptr<std::list <std::string>>	tablesName		(std::string schemaName, bool shortFormat = true) 			const	throw (exception&)	{return find(schemaName) -> second.tablesName(shortFormat);}
	void										addTable		(std::string schemaName, std::string tableName)						throw (exception&)	{find(schemaName) -> second.addTable(tableName);}
	void										addTable		(std::string tablePath)												throw (exception&)	{pathResolver path(tablePath, pathResolver::table); addTable(path.schemaName(), path.tableName());}
	void										addTable		(std::string schemaName, const table& _table)						throw (exception&)	{find(schemaName) -> second.addTable(_table);}
	void										dropTable		(std::string schemaName, std::string tableName)						throw (exception&)	{find(schemaName) -> second.dropTable(tableName);}
	void										dropTable		(std::string tablePath)												throw (exception&)	{pathResolver path(tablePath, pathResolver::table); dropTable(path.schemaName(), path.tableName());}
	bool										findTable		(std::string schemaName, std::string tableName)				const	throw (exception&)	{return find(schemaName) -> second.findTable(tableName);}
	bool										findTable		(std::string tablePath)										const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return findTable(path.schemaName(), path.tableName());}
	
	/**		MEMBRI RELATIVI ALLA STRUTTURA DELLA TABELLA	**/
	bool										resultHandler	(std::string schemaName, std::string tableName)										const	throw (exception&)	{return find(schemaName) -> second.resultHandler(tableName);}
	bool										resultHandler	(std::string tablePath)																const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return resultHandler(path.schemaName(), path.tableName());}
	unsigned									numberOfColumns	(std::string schemaName, std::string tableName)										const	throw (exception&)	{return find(schemaName) -> second.numberOfColumns(tableName);}
	unsigned									numberOfColumns	(std::string tablePath)																const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return numberOfColumns(path.schemaName(), path.tableName());}
	std::unique_ptr<std::list <std::string>> 	columnsName		(std::string schemaName, std::string tableName, bool shortFormat = true)			const	throw (exception&)	{return find(schemaName) -> second.columnsName(tableName, shortFormat);}
	std::unique_ptr<std::list <std::string>>	columnsName		(std::string tablePath, bool shortFormat = true)									const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return columnsName(path.schemaName(), path.tableName(), shortFormat);}

	void	addColumn	(std::string schemaName,
				std::string tableName,
				std::string columnName,
				column::sqlType columnType,
				unsigned length = 0,
				bool isKey = false,
				std::string defaultValue = ""
		)	throw (exception&)	{find(schemaName)-> second.addColumn(tableName, columnName, columnType, length, isKey, defaultValue);}

	void	addColumn	(std::string columnPath,
				column::sqlType columnType,
				unsigned length = 0,
				bool isKey = false,
				std::string defaultValue = ""
		)	throw (exception&)	{pathResolver path(columnPath); addColumn(path.schemaName(), path.tableName(), path.columnName(), columnType, length, isKey, defaultValue);}

	void	addReference(std::string schemaName, std::string tableName, std::string columnName, std::string ref_schemaName, std::string ref_tableName, std::string ref_columnName)	throw ()	{__referenceList.push_back(reference_info(schemaName, tableName, columnName, ref_schemaName, ref_tableName, ref_columnName));}

	bool	findColumn	(std::string schemaName, std::string tableName, std::string columnName)	const	throw (exception&)	{return find(schemaName) -> second.findColumn(tableName, columnName);}
	void	dropColumn	(std::string schemaName, std::string tableName, std::string columnName)			throw (exception&)	{return find(schemaName) -> second.dropColumn(tableName, columnName);}
	bool	findColumn	(std::string columnPath)												const	throw (exception&)	{pathResolver path(columnPath); return findColumn(path.schemaName(), path.tableName(), path.columnName());}
	void	dropColumn	(std::string columnPath)														throw (exception&)	{pathResolver path(columnPath); dropColumn(path.schemaName(), path.tableName(), path.columnName());}

	std::string 								structure(std::string schemaName, std::string tableName)	const	throw (exception&)	{return find(schemaName) -> second.structure(tableName);}
	std::unique_ptr<std::list<std::string>> 	structure(std::string schemaName)							const	throw (exception&)	{return find(schemaName) -> second.structure();}
	std::unique_ptr<std::list<std::string>>		structure()													const	throw ();

	/** 		METODI CHE PERMETTONO L'ACCESSO E MODIFICA DELLA STRUTTURA DELLE COLONNE **/
	enum column::sqlType	columnType		(std::string schemaName, std::string tableName, std::string columnName) 							const	throw (exception&)	{return find(schemaName) -> second.columnType(tableName, columnName);}
	enum column::sqlType	columnType		(std::string columnPath) 																			const	throw (exception&)	{pathResolver path(columnPath); return columnType(path.schemaName(), path.tableName(), path.columnName());}
	unsigned 				length			(std::string schemaName, std::string tableName, std::string columnName) 							const	throw (exception&)	{return find(schemaName) -> second.length(tableName, columnName);}
	unsigned 				length			(std::string columnPath) 																			const	throw (exception&)	{pathResolver path(columnPath); return length(path.schemaName(), path.tableName(), path.columnName());}
	std::string				defaultValue	(std::string schemaName, std::string tableName, std::string columnName)								const	throw (exception&)	{return find(schemaName) -> second.defaultValue(tableName, columnName);}
	void					defaultValue	(std::string schemaName, std::string tableName, std::string columnName, std::string newValue)				throw (exception&)	{find(schemaName) -> second.defaultValue(tableName, columnName, newValue);}
	std::string				defaultValue	(std::string columnPath)																			const	throw (exception&)	{pathResolver path(columnPath); return defaultValue(path.schemaName(), path.tableName(), path.columnName());}
	void					defaultValue	(std::string columnPath, std::string newValue)																throw (exception&)	{pathResolver path(columnPath); defaultValue(path.schemaName(), path.tableName(), path.columnName(), newValue);}
	bool 					isKey			(std::string schemaName, std::string tableName, std::string columnName) 							const	throw (exception&)	{return find(schemaName) -> second.isKey(tableName, columnName);}
	void 					isKey			(std::string schemaName, std::string tableName, std::string columnName, bool key)							throw (exception&)	{find(schemaName) -> second.isKey(tableName, columnName, key);}
	bool 					isKey			(std::string columnPath) 																			const	throw (exception&)	{pathResolver path(columnPath); return isKey(path.schemaName(), path.tableName(), path.columnName());}
	void 					isKey			(std::string columnPath, bool key)																			throw (exception&)	{pathResolver path(columnPath); isKey(path.schemaName(), path.tableName(), path.columnName(), key);}

			/** METODI RIGUARDANTI I MEMBRI USATI PER LE QUERY **/
	bool 	project			(std::string schemaName, std::string tableName, std::string columnName)					const	throw (exception&)	{return find(schemaName) -> second.project(tableName, columnName);}
	void 	project			(std::string schemaName, std::string tableName, std::string columnName, bool value)				throw (exception&)	{find(schemaName) -> second.project(tableName, columnName, value);}
	bool 	project			(std::string columnPath) 																const	throw (exception&)	{pathResolver path(columnPath); return project(path.schemaName(), path.tableName(), path.columnName());}
	void 	project			(std::string columnPath, bool value)															throw (exception&)	{pathResolver path(columnPath); project(path.schemaName(), path.tableName(), path.columnName(), value);}
	void 	cleanProject	(std::string schemaName, std::string tableName)													throw (exception&)	{return find(schemaName) -> second.cleanProject(tableName);}
	void	cleanProject	(std::string tablePath)																			throw (exception&)	{pathResolver path(tablePath, pathResolver::table); cleanProject(path.schemaName(), path.tableName());}

	bool 	select		(std::string schemaName, std::string tableName, std::string columnName)	 			const	throw (exception&)	{return find(schemaName) -> second.select(tableName, columnName);}
	void	select		(std::string schemaName, std::string tableName, std::string columnName, bool value)			throw (exception&)	{find(schemaName) -> second.select(tableName, columnName, value);}
	bool 	select		(std::string columnPath) 															const	throw (exception&)	{pathResolver path(columnPath); return select(path.schemaName(), path.tableName(), path.columnName());}
	void	select		(std::string columnPath, bool value)														throw (exception&)	{pathResolver path(columnPath); select(path.schemaName(), path.tableName(), path.columnName(), value);}
	void 	cleanSelect	(std::string schemaName, std::string tableName)												throw (exception&)	{find(schemaName) -> second.cleanSelect(tableName);}
	void	cleanSelect	(std::string tablePath)																		throw (exception&)	{pathResolver path(tablePath, pathResolver::table); cleanSelect(path.schemaName(), path.tableName());}

	std::string 	selectValue			(std::string schemaName, std::string tableName, std::string columnName)					 	const	throw (exception&)	{return find(schemaName) -> second.selectValue(tableName, columnName);}
	void 			selectValue			(std::string schemaName, std::string tableName, std::string columnName, std::string value)			throw (exception&)	{find(schemaName) -> second.selectValue(tableName, columnName, value);}
	std::string 	selectValue			(std::string columnPath) 																	const	throw (exception&)	{pathResolver path(columnPath); return selectValue(path.schemaName(), path.tableName(), path.columnName());}
	void 			selectValue			(std::string columnPath, std::string value)															throw (exception&)	{pathResolver path(columnPath); selectValue(path.schemaName(), path.tableName(), path.columnName(), value);}
	void 			cleanSelectValue	(std::string schemaName, std::string tableName)														throw (exception&)	{find(schemaName) -> second.cleanSelectValue(tableName);}
	void			cleanSelectValue	(std::string tablePath)																				throw (exception&)	{pathResolver path(tablePath, pathResolver::table); cleanSelectValue(path.schemaName(), path.tableName());}

	enum column::sqlCompOp 	compareOperator			(std::string schemaName, std::string tableName, std::string columnName) 							const	throw (exception&)	{return find(schemaName) -> second.compareOperator(tableName, columnName);}
	void 					compareOperator			(std::string schemaName, std::string tableName, std::string columnName, enum column::sqlCompOp op)			throw (exception&)	{find(schemaName) -> second.compareOperator(tableName, columnName, op);}
	enum column::sqlCompOp 	compareOperator			(std::string columnPath) 																			const	throw (exception&)	{pathResolver path(columnPath); return compareOperator(path.schemaName(), path.tableName(), path.columnName());}
	void 					compareOperator			(std::string columnPath, enum column::sqlCompOp op)															throw (exception&)	{pathResolver path(columnPath); compareOperator(path.schemaName(), path.tableName(), path.columnName(), op);}
	void 					cleanCompareOperator	(std::string schemaName, std::string tableName)																throw (exception&)	{find(schemaName) -> second.cleanCompareOperator(tableName);}
	void					cleanCompareOperator	(std::string tablePath)																						throw (exception&)	{pathResolver path(tablePath, pathResolver::table); cleanCompareOperator(path.schemaName(), path.tableName());}

	bool 	orderBy			(std::string schemaName, std::string tableName, std::string columnName)	 			const	throw (exception&)	{return find(schemaName) -> second.orderBy(tableName, columnName);}
	void 	orderBy			(std::string schemaName, std::string tableName, std::string columnName, bool value)			throw (exception&)	{find(schemaName) -> second.orderBy(tableName, columnName, value);}
	bool 	orderBy			(std::string columnPath) 															const	throw (exception&)	{pathResolver path(columnPath); return orderBy(path.schemaName(), path.tableName(), path.columnName());}
	void 	orderBy			(std::string columnPath, bool value)														throw (exception&)	{pathResolver path(columnPath); orderBy(path.schemaName(), path.tableName(), path.columnName(), value);}
	void 	cleanOrderBy	(std::string schemaName, std::string tableName)												throw (exception&)	{find(schemaName) -> second.cleanOrderBy(tableName);}
	void	cleanOrderBy	(std::string tablePath)																		throw (exception&)	{pathResolver path(tablePath, pathResolver::table); cleanOrderBy(path.schemaName(), path.tableName());}

	enum column::sqlOrder	orderMode		(std::string schemaName, std::string tableName, std::string columnName) 								const	throw (exception&)	{return find(schemaName) -> second.orderMode(tableName, columnName);}
	void 					orderMode		(std::string schemaName, std::string tableName, std::string columnName, enum column::sqlOrder order)			throw (exception&)	{find(schemaName) -> second.orderMode(tableName, columnName, order);}
	enum column::sqlOrder	orderMode		(std::string columnPath) 																				const	throw (exception&)	{pathResolver path(columnPath); return orderMode(path.schemaName(), path.tableName(), path.columnName());}
	void 					orderMode		(std::string columnPath, enum column::sqlOrder order)															throw (exception&)	{pathResolver path(columnPath); orderMode(path.schemaName(), path.tableName(), path.columnName(), order);}
	void 					cleanOrderMode	(std::string schemaName, std::string tableName)																	throw (exception&)	{find(schemaName) -> second.cleanOrderMode(tableName);}
	void					cleanOrderMode	(std::string tablePath)																							throw (exception&)	{pathResolver path(tablePath, pathResolver::table); cleanOrderMode(path.schemaName(), path.tableName());}

	void 	cleanAll	(std::string schemaName, std::string tableName)		throw (exception&)	{find(schemaName) -> second.cleanAll(tableName);}
	void	cleanAll	(std::string tablePath)								throw (exception&)	{pathResolver path(tablePath, pathResolver::table); cleanAll(path.schemaName(), path.tableName());}


	/**		MEMBRI RELATIVI	 ALL' INSERIMENTO, MODOFICA E CANCELLAZIONE DI TUPLE DALLA TABELLA 	**/
	unsigned long								numberOfTuples		(std::string schemaName, std::string tableName)		const	throw (exception&)	{return find(schemaName) -> second.numberOfTuples(tableName);}
	unsigned long								numberOfTuples		(std::string tablePath)								const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return numberOfTuples(path.schemaName(), path.tableName());}
	std::unique_ptr<std::list <unsigned long>>	tuplesInternalID	(std::string schemaName, std::string tableName)		const	throw (exception&)	{return find(schemaName) -> second.tuplesInternalID(tableName);}
	std::unique_ptr<std::list <unsigned long>>	tuplesInternalID	(std::string tablePath) 							const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return tuplesInternalID(path.schemaName(), path.tableName());}
	void										clear				(std::string schemaName, std::string tableName)				throw (exception&)	{find(schemaName) -> second.clear(tableName);}
	void										clear				(std::string tablePath) 									throw (exception&)	{pathResolver path(tablePath, pathResolver::table); clear(path.schemaName(), path.tableName());}

	unsigned long	insertTuple	(std::string schemaName, std::string tableName,const std::list<std::string>& values, enum tuple::state _status = tuple::inserting)	throw (exception&)	{return find(schemaName) -> second.insertTuple(tableName, values, _status);}
	unsigned long	insertTuple	(std::string tablePath,const std::list<std::string>& values, enum tuple::state _status = tuple::inserting)							throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return insertTuple(path.schemaName(), path.tableName(), values, _status);}
	void			updateTuple	(std::string schemaName, std::string tableName, unsigned long internalID,const std::list<std::string>& values)						throw (exception&)	{find(schemaName) -> second.updateTuple(tableName, internalID, values);}
	void			updateTuple	(std::string tablePath, unsigned long internalID, const std::list<std::string>& values)												throw (exception&)	{pathResolver path(tablePath, pathResolver::table); updateTuple(path.schemaName(), path.tableName(), internalID, values);}
	void			deleteTuple	(std::string schemaName, std::string tableName, unsigned long internalID)															throw (exception&)	{find(schemaName) -> second.deleteTuple(tableName, internalID);}
	void			deleteTuple	(std::string tablePath, unsigned long internalID)																					throw (exception&)	{pathResolver path(tablePath, pathResolver::table); deleteTuple(path.schemaName(), path.tableName(), internalID);}
	void			eraseTuple	(std::string schemaName, std::string tableName, unsigned long internalID)															throw (exception&)	{find(schemaName) -> second.eraseTuple(tableName, internalID);}
	void			eraseTuple	(std::string tablePath, unsigned long internalID)																					throw (exception&)	{pathResolver path(tablePath, pathResolver::table); eraseTuple(path.schemaName(), path.tableName(), internalID);}

	std::unique_ptr<std::list <std::string>>	tupleValues			(std::string schemaName, std::string tableName, unsigned long internalID)	const	throw (exception&)	{return find(schemaName) -> second.tupleValues(tableName, internalID);}
	std::unique_ptr<std::list <std::string>>	tupleValues			(std::string tablePath, unsigned long internalID)	 						const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return tupleValues(path.schemaName(), path.tableName(), internalID);}
	std::unique_ptr<std::list <std::string>>	tupleOldValues		(std::string schemaName, std::string tableName, unsigned long internalID)	const	throw (exception&)	{return find(schemaName) -> second.tupleOldValues(tableName, internalID);}
	std::unique_ptr<std::list <std::string>>	tupleOldValues		(std::string tablePath, unsigned long internalID)	 						const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return tupleOldValues(path.schemaName(), path.tableName(), internalID);}

	void	bulkInsertModel	(std::string schemaName, std::string tableName, std::string fileName)	const	throw (exception&)	{find(schemaName) -> second.bulkInsertModel(tableName, fileName);}
	void	bulkInsertModel	(std::string tablePath, std::string fileName)							const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); bulkInsertModel(path.schemaName(), path.tableName(), fileName);}
	void	bulkInsert		(std::string schemaName, std::string tableName, std::string fileName)			throw (exception&)	{find(schemaName) -> second.bulkInsert(tableName, fileName);}
	void	bulkInsert		(std::string tablePath, std::string fileName)									throw (exception&)	{pathResolver path(tablePath, pathResolver::table); bulkInsert(path.schemaName(), path.tableName(), fileName);}
	void	bulkUpdateModel	(std::string schemaName, std::string tableName, std::string fileName)	const	throw (exception&)	{find(schemaName) -> second.bulkUpdateModel(tableName, fileName);}
	void	bulkUpdateModel	(std::string tablePath, std::string fileName)							const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); bulkUpdateModel(path.schemaName(), path.tableName(), fileName);}
	void	bulkUpdate		(std::string schemaName, std::string tableName, std::string fileName)			throw (exception&)	{find(schemaName) -> second.bulkUpdate(tableName, fileName);}
	void	bulkUpdate		(std::string tablePath, std::string fileName)									throw (exception&)	{pathResolver path(tablePath, pathResolver::table); bulkUpdate(path.schemaName(), path.tableName(), fileName);}

	/**					MEMBRI RELATIVI ALLE OPERAZIONI SQL SU DBMS REMOTO				**/
	std::unique_ptr<std::list <schema::loadCommand_info>>	loadCommand	()												const	throw ();
	schema::loadCommand_info								loadCommand	(std::string schemaName, std::string tableName) const	throw (exception&)	{return find(schemaName) -> second.loadCommand(tableName);}
	schema::loadCommand_info								loadCommand	(std::string tablePath)							const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return loadCommand(path.schemaName(), path.tableName());}
	std::unique_ptr<std::list <std::string>>				commit		()												const	throw ();
	std::unique_ptr<std::list <std::string>>				commit		(std::string schemaName, std::string tableName)	const	throw (exception&)	{return find(schemaName) -> second.commit(tableName);}
	std::unique_ptr<std::list <std::string>>				commit		(std::string tablePath)							const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); return commit(path.schemaName(), path.tableName());}

	/**						MEMBRI DI UTILITÀ GENERALE					**/
	void	toHtml	(std::string schemaName, std::string tableName, std::string fileName)	const	throw (exception&)	{find(schemaName) -> second.toHtml(tableName, fileName);}
	void	toHtml	(std::string tablePath, std::string fileName)	 						const	throw (exception&)	{pathResolver path(tablePath, pathResolver::table); toHtml(path.schemaName(), path.tableName(), fileName);}

	class pathResolver {
	public :
		enum pathType {table, column};
		pathResolver (std::string path, pathType type = column);

		std::string schemaName() 	const 	{return resolvedSchema;}
		std::string tableName() 	const 	{return resolvedTable;}
		std::string columnName() 	const 	{return resolvedColumn;}

	private :
		std::string resolvedSchema;
		std::string resolvedTable;
		std::string resolvedColumn;
	};

private :
	std::string						__databaseName;
	static const std::string		__tmpBasePath;
	std::string						__tmpStoragePath;
	std::map <std::string, schema>	__schemaMap;

	std::map <std::string, schema>::const_iterator	find	(std::string schemaName)	const	throw (exception&);
	std::map <std::string, schema>::iterator		find	(std::string schemaName)			throw (exception&);

	class reference_info {
	public:
		reference_info(std::string schemaName, std::string tableName, std::string columnName, std::string ref_schemaName, std::string ref_tableName, std::string ref_columnName) :
			__schemaName(schemaName), __tableName(tableName), __columnName(columnName), __ref_schemaName(ref_schemaName), __ref_tableName(ref_tableName), __ref_columnName(ref_columnName) {}

		std::string path		()	const	throw ()	{return __schemaName + "." + __tableName + "." + __columnName;}
		std::string ref_path	() 	const 	throw ()	{return __ref_schemaName + "." + __ref_tableName + "." + __ref_columnName;}

	private:
		std::string __schemaName;
		std::string __tableName;
		std::string __columnName;
		std::string __ref_schemaName;
		std::string __ref_tableName;
		std::string __ref_columnName;
	};
	std::list<reference_info>	__referenceList;
};
};
#endif
