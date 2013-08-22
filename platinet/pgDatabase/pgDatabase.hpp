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

#ifndef __PG_DATABASE_INTEGRATION__
#define __PG_DATABASE_INTEGRATION__

#include "../engine.hpp"
#include "../pqcc.hpp"

class pgDatabase {
public:
	pgDatabase (
			std::string	_host = "",					//host-address or host-name of postgresql database server
			std::string _port = "5432",				//port
			std::string	_dbname = "",				//database name
			std::string	_user = "",					//user name
			std::string	_passwd = "",				//password
			unsigned	_cuncurrend_connection = 5	//number of simultaneous connection to 	postgresql database server
			);


	/**							DBMS CONNECTION FUCNTION							**/
	void												host				(std::string _host)					{__pgDriver.host(_host);}
	void												port				(std::string _port)					{__pgDriver.port(_port);}
	void												dbname				(std::string _dbname)				{__pgDriver.dbname(_dbname);}
	void												user				(std::string _user)					{__pgDriver.user(_user);}
	void												passwd				(std::string _passwd)				{__pgDriver.passwd(_passwd);}
	void												connect				()									{__pgDriver.connect();}
	void												connectBlock		()									{__pgDriver.connectBlock();}
	std::unique_ptr<enum pqcc::pgConnection::status[]>	connectionStatus	()							const	{return __pgDriver.connectionStatus();}
	void												disconnect			()									{__pgDriver.disconnect();}
	void												reset				()									{__pgDriver.reset();}
	std::unique_ptr<std::string[]> 						connectionErrorMsg	()							const	{return __pgDriver.connectionErrorMsg();}

	void	loadStructure		();
	void	loadTuple			();

	std::string 								structure(std::string schemaName, std::string tableName)	const	throw (engine::exception&)	{return __database.structure(schemaName, tableName);}
	std::unique_ptr<std::list<std::string>> 	structure(std::string schemaName)							const	throw (engine::exception&)	{return __database.structure(schemaName);}
	std::unique_ptr<std::list<std::string>>		structure()													const	throw ()					{return __database.structure();}

	/**					MEMBRI RELATIVI ALLA STRUTTURA DEL DATABASE							**/
	std::string 								databaseName	() 							const	throw ()	{return __database.databaseName();}
	unsigned									numberOfSchemas	() 							const	throw ()	{return __database.numberOfSchemas();}
	std::unique_ptr<std::list <std::string>>	schemasName		(bool shortFormat = true)	const	throw ()	{return __database.schemasName(shortFormat);}
	bool										findSchema		(std::string schemaName)	const	throw ()	{return __database.findSchema(schemaName);}

	/**					MEMBRI RELATIVI ALLA STRUTTURA DELLO SCHEMA							**/
	unsigned									numberOfTables		(std::string schemaName) 							const	throw (engine::exception&)	{return __database.numberOfTables(schemaName);}
	std::unique_ptr<std::list <std::string>>	tablesName			(std::string schemaName, bool shortFormat = true)	const	throw (engine::exception&)	{return __database.tablesName(schemaName, shortFormat);}

	/**					MEMBRI RELATIVI ALLA STRUTTURA DELLA TABELLA						**/
	bool										resultHandler		(std::string schemaName, std::string tableName)								const	throw (engine::exception&)	{return __database.resultHandler(schemaName, tableName);}
	bool										resultHandler		(std::string tablePath)														const	throw (engine::exception&)	{return __database.resultHandler(tablePath);}
	unsigned									numberOfColumns		(std::string schemaName, std::string tableName)								const	throw (engine::exception&)	{return __database.numberOfColumns(schemaName, tableName);}
	unsigned									numberOfColumns		(std::string tablePath)														const	throw (engine::exception&)	{return __database.numberOfColumns(tablePath);}
	std::unique_ptr<std::list <std::string>> 	columnsName			(std::string schemaName, std::string tableName, bool shortFormat = true)	const	throw (engine::exception&)	{return __database.columnsName(schemaName, tableName, shortFormat);}
	std::unique_ptr<std::list <std::string>>	columnsName			(std::string tablePath, bool shortFormat = true)							const	throw (engine::exception&)	{return __database.columnsName(tablePath, shortFormat);}
	bool										findColumn			(std::string schemaName, std::string tableName, std::string columnName)		const	throw (engine::exception&)	{return __database.findColumn(schemaName, tableName, columnName);}
	bool										findColumn			(std::string columnPath)													const	throw (engine::exception&)	{return __database.findColumn(columnPath);}

	/** 				METODI RELATIVI ALLA STRUTTURA DELLE COLONNE 						**/
	engine::column::sqlType		columnType		(std::string schemaName, std::string tableName, std::string columnName)	const	throw (engine::exception&)	{return __database.columnType(schemaName, tableName, columnName);}
	engine::column::sqlType		columnType		(std::string columnPath) 												const	throw (engine::exception&)	{return __database.columnType(columnPath);}
	unsigned 					length			(std::string schemaName, std::string tableName, std::string columnName)	const	throw (engine::exception&)	{return __database.length(schemaName, tableName, columnName);}
	unsigned 					length			(std::string columnPath) 												const	throw (engine::exception&)	{return __database.length(columnPath);}
	std::string					defaultValue	(std::string schemaName, std::string tableName, std::string columnName)	const	throw (engine::exception&)	{return __database.defaultValue(schemaName, tableName, columnName);}
	std::string					defaultValue	(std::string columnPath)												const	throw (engine::exception&)	{return __database.defaultValue(columnPath);}
	bool 						isKey			(std::string schemaName, std::string tableName, std::string columnName)	const	throw (engine::exception&)	{return __database.isKey(schemaName, tableName, columnName);}
	bool 						isKey			(std::string columnPath) 												const	throw (engine::exception&)	{return __database.isKey(columnPath);}

	/**			 		METODI RIGUARDANTI I MEMBRI USATI PER LE QUERY DI SELEZIONE			**/
	bool			 			project				(std::string schemaName, std::string tableName, std::string columnName)									const	throw (engine::exception&)	{return __database.project(schemaName, tableName, columnName);}
	void 						project				(std::string schemaName, std::string tableName, std::string columnName, bool value)								throw (engine::exception&)	{__database.project(schemaName, tableName, columnName, value);}
	bool 						project				(std::string columnPath) 																				const	throw (engine::exception&)	{return __database.project(columnPath);}
	void 						project				(std::string columnPath, bool value)																			throw (engine::exception&)	{__database.project(columnPath, value);}
	void	 					cleanProject		(std::string schemaName, std::string tableName)																	throw (engine::exception&)	{__database.cleanProject(schemaName, tableName);}
	void						cleanProject		(std::string tablePath)																							throw (engine::exception&)	{__database.cleanProject(tablePath);}
	bool 						select				(std::string schemaName, std::string tableName, std::string columnName) 								const	throw (engine::exception&)	{return __database.select(schemaName, tableName, columnName);}
	void						select				(std::string schemaName, std::string tableName, std::string columnName, bool value)								throw (engine::exception&)	{__database.select(schemaName, tableName, columnName, value);}
	bool 						select				(std::string columnPath) 																				const	throw (engine::exception&)	{return __database.select(columnPath);}
	void						select				(std::string columnPath, bool value)																			throw (engine::exception&)	{__database.select(columnPath, value);}
	void 						cleanSelect			(std::string schemaName, std::string tableName)																	throw (engine::exception&)	{__database.cleanSelect(schemaName, tableName);}
	void						cleanSelect			(std::string tablePath)																							throw (engine::exception&)	{__database.cleanSelect(tablePath);}
	std::string 				selectValue			(std::string schemaName, std::string tableName, std::string columnName)									const	throw (engine::exception&)	{return __database.selectValue(schemaName, tableName, columnName);}
	void 						selectValue			(std::string schemaName, std::string tableName, std::string columnName, std::string value)						throw (engine::exception&)	{__database.selectValue(schemaName, tableName, columnName, value);}
	std::string 				selectValue			(std::string columnPath) 																				const	throw (engine::exception&)	{return __database.selectValue(columnPath);}
	void 						selectValue			(std::string columnPath, std::string value)																		throw (engine::exception&)	{__database.selectValue(columnPath, value);}
	void 						cleanSelectValue	(std::string schemaName, std::string tableName)																	throw (engine::exception&)	{__database.cleanSelectValue(schemaName, tableName);}
	void						cleanSelectValue	(std::string tablePath)																							throw (engine::exception&)	{__database.cleanSelectValue(tablePath);}
	engine::column::sqlCompOp 	compareOperator		(std::string schemaName, std::string tableName, std::string columnName) 								const	throw (engine::exception&)	{return __database.compareOperator(schemaName, tableName, columnName);}
	void 						compareOperator		(std::string schemaName, std::string tableName, std::string columnName, engine::column::sqlCompOp op)			throw (engine::exception&)	{__database.compareOperator(schemaName, tableName, columnName, op);}
	engine::column::sqlCompOp 	compareOperator		(std::string columnPath) 																				const	throw (engine::exception&)	{return __database.compareOperator(columnPath);}
	void 						compareOperator		(std::string columnPath, engine::column::sqlCompOp op)															throw (engine::exception&)	{__database.compareOperator(columnPath, op);}
	void 						cleanCompareOperator(std::string schemaName, std::string tableName)																	throw (engine::exception&)	{__database.cleanCompareOperator(schemaName, tableName);}
	void						cleanCompareOperator(std::string tablePath)																							throw (engine::exception&)	{__database.cleanCompareOperator(tablePath);}
	bool 						orderBy				(std::string schemaName, std::string tableName, std::string columnName)									const	throw (engine::exception&)	{return __database.orderBy(schemaName, tableName, columnName);}
	void 						orderBy				(std::string schemaName, std::string tableName, std::string columnName, bool value)								throw (engine::exception&)	{__database.orderBy(schemaName, tableName, columnName, value);}
	bool 						orderBy				(std::string columnPath) 																				const	throw (engine::exception&)	{return __database.orderBy(columnPath);}
	void 						orderBy				(std::string columnPath, bool value)																			throw (engine::exception&)	{__database.orderBy(columnPath, value);}
	void 						cleanOrderBy		(std::string schemaName, std::string tableName)																	throw (engine::exception&)	{__database.cleanOrderBy(schemaName, tableName);}
	void						cleanOrderBy		(std::string tablePath)																							throw (engine::exception&)	{__database.cleanOrderBy(tablePath);}
	engine::column::sqlOrder	orderMode			(std::string schemaName, std::string tableName, std::string columnName) 								const	throw (engine::exception&)	{return __database.orderMode(schemaName, tableName, columnName);}
	void 						orderMode			(std::string schemaName, std::string tableName, std::string columnName, engine::column::sqlOrder order)			throw (engine::exception&)	{__database.orderMode(schemaName, tableName, columnName, order);}
	engine::column::sqlOrder	orderMode			(std::string columnPath) 																				const	throw (engine::exception&)	{return __database.orderMode(columnPath);}
	void 						orderMode			(std::string columnPath, engine::column::sqlOrder order)														throw (engine::exception&)	{__database.orderMode(columnPath, order);}
	void 						cleanOrderMode		(std::string schemaName, std::string tableName)																	throw (engine::exception&)	{__database.cleanOrderMode(schemaName, tableName);}
	void						cleanOrderMode		(std::string tablePath)																							throw (engine::exception&)	{__database.cleanOrderMode(tablePath);}
	void 						cleanAll			(std::string schemaName, std::string tableName)																	throw (engine::exception&)	{__database.cleanAll(schemaName, tableName);}
	void						cleanAll			(std::string tablePath)																							throw (engine::exception&)	{__database.cleanAll(tablePath);}

	/**		MEMBRI RELATIVI	 ALL' INSERIMENTO, MODOFICA E CANCELLAZIONE DI TUPLE DALLA TABELLA 	**/
	unsigned long								numberOfTuples		(std::string schemaName, std::string tableName)		const	throw (engine::exception&)	{return __database.numberOfTuples(schemaName, tableName);}
	unsigned long								numberOfTuples		(std::string tablePath)								const	throw (engine::exception&)	{return __database.numberOfTuples(tablePath);}
	std::unique_ptr<std::list <unsigned long>>	tuplesInternalID	(std::string schemaName, std::string tableName)		const	throw (engine::exception&)	{return __database.tuplesInternalID(schemaName, tableName);}
	std::unique_ptr<std::list <unsigned long>>	tuplesInternalID	(std::string tablePath) 							const	throw (engine::exception&)	{return __database.tuplesInternalID(tablePath);}
	void										clear				(std::string schemaName, std::string tableName)				throw (engine::exception&)	{__database.clear(schemaName, tableName);}
	void										clear				(std::string tablePath) 									throw (engine::exception&)	{__database.clear(tablePath);}

	unsigned long	insertTuple	(std::string schemaName, std::string tableName,const std::list<std::string>& values)								throw (engine::exception&)	{return __database.insertTuple(schemaName, tableName, values);}
	unsigned long	insertTuple	(std::string tablePath,const std::list<std::string>& values)														throw (engine::exception&)	{return __database.insertTuple(tablePath, values);}
	void			updateTuple	(std::string schemaName, std::string tableName, unsigned long internalID,const std::list<std::string>& values)		throw (engine::exception&)	{__database.updateTuple(schemaName, tableName, internalID, values);}
	void			updateTuple	(std::string tablePath, unsigned long internalID, const std::list<std::string>& values)								throw (engine::exception&)	{__database.updateTuple(tablePath, internalID, values);}
	void			deleteTuple	(std::string schemaName, std::string tableName, unsigned long internalID)											throw (engine::exception&)	{__database.deleteTuple(schemaName, tableName, internalID);}
	void			deleteTuple	(std::string tablePath, unsigned long internalID)																	throw (engine::exception&)	{__database.deleteTuple(tablePath, tablePath, internalID);}
	void			eraseTuple	(std::string schemaName, std::string tableName, unsigned long internalID)											throw (engine::exception&)	{__database.eraseTuple(schemaName, tableName, internalID);}
	void			eraseTuple	(std::string tablePath, unsigned long internalID)																	throw (engine::exception&)	{__database.eraseTuple(tablePath, internalID);}

	std::unique_ptr<std::list <std::string>>	tupleValues			(std::string schemaName, std::string tableName, unsigned long internalID)	const	throw (engine::exception&)	{return __database.tupleValues(schemaName, tableName, internalID);}
	std::unique_ptr<std::list <std::string>>	tupleValues			(std::string tablePath, unsigned long internalID)	 						const	throw (engine::exception&)	{return __database.tupleValues(tablePath, internalID);}
	std::unique_ptr<std::list <std::string>>	tupleOldValues		(std::string schemaName, std::string tableName, unsigned long internalID)	const	throw (engine::exception&)	{return __database.tupleOldValues(schemaName, tableName, internalID);}
	std::unique_ptr<std::list <std::string>>	tupleOldValues		(std::string tablePath, unsigned long internalID)	 						const	throw (engine::exception&)	{return __database.tupleOldValues(tablePath, internalID);}

	void	bulkInsertModel	(std::string schemaName, std::string tableName, std::string fileName)	const	throw (engine::exception&)	{__database.bulkInsertModel(schemaName, tableName, fileName);}
	void	bulkInsertModel	(std::string tablePath, std::string fileName)							const	throw (engine::exception&)	{__database.bulkInsertModel(tablePath, fileName);}
	void	bulkInsert		(std::string schemaName, std::string tableName, std::string fileName)			throw (engine::exception&)	{__database.bulkInsert(schemaName, tableName, fileName);}
	void	bulkInsert		(std::string tablePath, std::string fileName)									throw (engine::exception&)	{__database.bulkInsert(tablePath, fileName);}
	void	bulkUpdateModel	(std::string schemaName, std::string tableName, std::string fileName)	const	throw (engine::exception&)	{__database.bulkUpdateModel(schemaName, tableName, fileName);}
	void	bulkUpdateModel	(std::string tablePath, std::string fileName)							const	throw (engine::exception&)	{__database.bulkUpdateModel(tablePath, fileName);}
	void	bulkUpdate		(std::string schemaName, std::string tableName, std::string fileName)			throw (engine::exception&)	{__database.bulkUpdate(schemaName, tableName, fileName);}
	void	bulkUpdate		(std::string tablePath, std::string fileName)									throw (engine::exception&)	{__database.bulkUpdate(tablePath, fileName);}

	/**					MEMBRI RELATIVI ALLE OPERAZIONI SQL SU DBMS REMOTO				**/
	std::unique_ptr<std::list <unsigned long>> commit	()												throw ();
	std::unique_ptr<std::list <unsigned long>> commit	(std::string schemaName, std::string tableName)	throw (engine::exception&);
	std::unique_ptr<std::list <unsigned long>> commit	(std::string tablePath)							throw (engine::exception&);

	/**						MEMBRI DI UTILITÀ GENERALE					**/
	void	toHtml	(std::string schemaName, std::string tableName, std::string fileName)	const	throw (engine::exception&)	{__database.toHtml(schemaName, tableName, fileName);}
	void	toHtml	(std::string tablePath, std::string fileName)	 						const	throw (engine::exception&)	{__database.toHtml(tablePath, fileName);}

private:
	pqcc::pgDriver		__pgDriver;
	engine::database	__database;

	static const std::string	__allColumnQuery;
	static const std::string	__keyColumnQuery;
	static const std::string	__referentialConstraint;

	engine::column::sqlType		__sqlType(std::string type)												const;
	std::string					__columnDefault(std::string colDefault, engine::column::sqlType type)	const;
	unsigned					__columnLength(std::string length, engine::column::sqlType type)		const;

	void	loadTupleThr	(engine::schema::loadCommand_info 	_loadInfo);
};
#endif
