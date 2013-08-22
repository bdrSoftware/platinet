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

#ifndef _TABLE_
#define _TABLE_

#include "column.hpp"
#include "mStorage.hpp"
#include "fStorage.hpp"
#include <map>
#include <list>

namespace engine {
class table {
public :
	enum storageType {memory, disk};
	table(std::string tableName, std::string tmpStorageBase,bool resultHandler = false, storageType _storage = disk)	throw ();

	std::string	tableName			() 							const	throw ()	{return __tableName;}
	void 		tableName			(std::string tableName)				throw ()	{if (resultHandler() || __tableName == "") __tableName = tableName;}
	bool		resultHandler		()							const	throw ()	{return __resultHandler;}
	unsigned	numberOfColumns		()							const	throw ()	{return __columnList.size();}	//restituisce il numero di colonne che compongono la tabella

	std::unique_ptr<std::list<std::string>>		columnsName			(bool shortFormat = true)	const	throw ();					//restituisce una lista contenente il nome delle colonne della tabella

	/**		MEMBRI RELATIVI ALLA STRUTTURA DELLA TABELLA	**/
	void addColumn (std::string columnName,	//nome colonna
			column::sqlType columnType,		//tipo sql
			unsigned length = 0,			//eventuale lunghezza, solo per tipo varchar
			bool isKey = false,				//true se compone la chiave per la tabella
			std::string defaultValue = ""	//valore di default
		)	throw ();
	bool		findColumn	(std::string columnName)	 	const	throw ();
	void		dropColumn	(std::string columnName)				throw ();
	std::string structure	()								const	throw();

	/** 		METODI CHE PERMETTONO L'ACCESSO E MODIFICA DELLA STRUTTURA DELLE COLONNE **/
	enum column::sqlType	columnType		(std::string columnName)	 					const	throw (exception&)	{return find(columnName) -> columnType();}
	unsigned			 	length			(std::string columnName)						const	throw (exception&)	{return find(columnName) -> length();}	//numero di caratteri necessari a rappresentare un valore del tipo sqlType della colonna
	std::string				defaultValue	(std::string columnName)						const	throw (exception&)	{return find(columnName) -> defaultValue();}
	void					defaultValue	(std::string columnName, std::string newValue)			throw (exception&)	{find(columnName) -> defaultValue(newValue);}
	bool 					isKey			(std::string columnName) 						const	throw (exception&)	{return find(columnName) -> isKey();}
	void			 		isKey			(std::string columnName, bool key)						throw (exception&)	{find(columnName) -> isKey(key);}

	/** 		METODI RIGUARDANTI I MEMBRI USATI PER LE QUERY **/
	bool 	project			(std::string columnName) 				const	throw (exception&)	{return find(columnName) -> project();}
	void 	project			(std::string columnName, bool value)			throw (exception&)	{find(columnName) -> project(value);}
	void	cleanProject	()												throw ();

	bool	select		(std::string columnName) 				const	throw (exception&)	{return find(columnName) -> select();}
	void	select		(std::string columnName, bool value)			throw (exception&) 	{find(columnName) -> select(value);}
	void	cleanSelect	()												throw ();

	std::string 	selectValue			(std::string columnName) 						const	throw (exception&)	{return find(columnName) -> selectValue();}
	void 			selectValue			(std::string columnName, std::string value)				throw (exception&)	{find(columnName) -> selectValue(value);}
	void			cleanSelectValue	()														throw ();

	column::sqlCompOp 	compareOperator			(std::string columnName) 						const	throw (exception&)	{return find(columnName) -> compareOperator();}
	void				compareOperator			(std::string columnName, column::sqlCompOp op)			throw (exception&)	{find(columnName) -> compareOperator(op);}
	void				cleanCompareOperator	()														throw ();

	bool	orderBy			(std::string columnName) 				const	throw (exception&)	{return find(columnName) -> orderBy();}
	void	orderBy			(std::string columnName, bool value)			throw (exception&)	{find(columnName) -> orderBy(value);}
	void	cleanOrderBy	()												throw ();

	column::sqlOrder	orderMode		(std::string columnName) 							const	throw (exception&)	{return find(columnName) -> orderMode();}
	void 				orderMode		(std::string columnName, column::sqlOrder order)			throw (exception&)	{find(columnName) -> orderMode(order);}
	void				cleanOrderMode	()															throw ();

	void 	cleanAll	()		throw ();

	/**		MEMBRI RELATIVI ALL' INSERIMENTO, MODOFICA E CANCELLAZIONE DI TUPLE DALLA TABELLA 	**/
	unsigned long								numberOfTuples		()	const	throw ()					{return __tupleStorage -> numTuples();}	//restituisce il numero di righe (tuple) di una tabella
	std::unique_ptr<std::list<unsigned long>>	tuplesInternalID	()	const	throw ()					{return __tupleStorage -> internalID();}
	void										clear				()			throw ()					{__tupleStorage -> clear();}
	
	unsigned long	insertTuple	(const std::list <std::string>& values, enum tuple::state _status = tuple::inserting)		throw (exception&)	{return __tupleStorage -> insert(values, __columnList, _status);}
	void			updateTuple	(unsigned long internalID, const std::list <std::string>& values)							throw (exception&)	{__tupleStorage -> update(internalID, values, __columnList);}
	void			deleteTuple	(unsigned long internalID)																	throw (exception&)	{__tupleStorage -> cancel(internalID);}

	void				eraseTuple		(unsigned long internalID)			throw (exception&)	{__tupleStorage -> erase(internalID);}
	enum tuple::state	tupleStatus		(unsigned long internalID)	const	throw (exception&)	{return __tupleStorage -> state(internalID);}
	bool				tupleVisible	(unsigned long internalID)	const	throw (exception&)	{return __tupleStorage -> visible(internalID);}
	
	std::unique_ptr<std::list <std::string>>	tupleValues		(unsigned long internalID)	const	throw (exception&)	{return __tupleStorage -> value(internalID);}
	std::unique_ptr<std::list <std::string>>	tupleOldValues	(unsigned long internalID)	const	throw (exception&)	{return __tupleStorage -> old(internalID);}

	void	bulkInsertModel	(std::string fileName)									const	throw (exception&);
	void	bulkInsert		(std::string fileName)											throw (exception&);
	void	bulkUpdateModel	(std::string fileName)									const	throw (exception&);
	void	bulkUpdate		(std::string fileName)											throw (exception&);
	
	void	bulkInsertModel_nothr	(std::string fileName)							const	throw (exception&);
	void	bulkInsert_nothr		(std::string fileName)									throw (exception&);
	void	bulkUpdateModel_nothr	(std::string fileName)							const	throw (exception&);
	void	bulkUpdate_nothr		(std::string fileName)									throw (exception&);

	/**		MEMBRI RELATIVI ALL' OUTPUT DELLA TABELLA	**/
	void	toHtml			(std::string fileName)	const	throw (exception&);
	void	toHtml_nothr	(std::string fileName)	const	throw (exception&);

private :
	std::string 		__tableName;		//nome della tabella
	std::string			__tmpStorageBase;
	bool 				__resultHandler;	//true se la tabella gestisce result di query
	std::list <column> 	__columnList;		//list invece di map, così le colonne sono ordinate secondo l'ordine di creazione nella tabella!

	storage*			__tupleStorage;

	std::list <column>::const_iterator			find		(std::string columnName)	const	throw (exception&);
	std::list <column>::iterator				find		(std::string columnName)			throw (exception&);
	std::unique_ptr<std::list <std::string>> 	tokenize	(std::string _string)		const;
};
};
#endif
