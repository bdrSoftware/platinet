/*
 * Copyright 2011, 2012, 2013 salvatore barone <salvator.barone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _COLUMN_
#define _COLUMN_

#include <string>

namespace engine {
class column {
/*
	L'oggetto "column" astrae il concetto di colonna, ma non ne conserva i valori: le informazioni contenute in questo tipo di oggetto sono solo di
	natura strutturale e gestionale.
	*/
public :
	enum sqlType{undefined, boolean, varchar, bigint, integer, smallint, doublePrecision, real, date, time};
	column (std::string columnName = "", 		//nome della colonna
			sqlType columnType = undefined,			//tipo sql
			unsigned length = 0,					//numero caratteri necessario a rappresentare il tipo di dato. impostato automaticamente
													//per tutti i tipi tranne che per varchar
			bool isKey = false,						//true se la colonna compone la chiave per la tabella alla quale appartiene
			std::string defaultValue = ""			//valore di default
		)	throw ();

	/** METODI RIGUARDANTI I MEMBRI STRUTTURALI DELLA COLONNA **/
	std::string		columnName		()		 				const 	throw ()	{return __columnName;}
	void 			columnName		(std::string newName) 			throw ()	{if (__columnName == "") __columnName = newName;}
	enum sqlType 	columnType		() 						const	throw ()	{return __columnType;}
	void 			columnType		(enum sqlType newType)			throw ()	{if (__columnType == undefined) __columnType = newType;}
	unsigned	 	length			() 						const	throw ()	{return __length;}
	std::string 	defaultValue	() 						const	throw ()	{return __defaultValue;}
	void 			defaultValue	(std::string newValue)			throw ()	{__defaultValue = newValue;}
	bool 			isKey			() 						const	throw ()	{return __isKey;}
	void 			isKey			(bool key)						throw ()	{__isKey = key;}

	/** METODI RIGUARDANTI I MEMBRI USATI PER LE QUERY **/
	enum sqlCompOp	{more, moreEqual, equal, disequal, like, notLike, lessEqual, less};
	enum sqlOrder	{asc, desc};
	bool 			project			() 						const 	throw ()	{return __project;}
	void			project			(bool value) 					throw ()	{__project = value;}
	bool 			select			() 						const 	throw ()	{return __select;}
	void 			select			(bool value) 					throw ()	{__select = value;}
	std::string 	selectValue		() 						const 	throw ()	{return __selectValue;}
	void			selectValue		(std::string value) 			throw ()	{__selectValue = value;}
	enum sqlCompOp 	compareOperator	()	 					const	throw ()	{return __compareOperator;}
	void 			compareOperator	(sqlCompOp op) 					throw ()	{__compareOperator = op;}
	bool 			orderBy			() 						const 	throw ()	{return __orderBy;}
	void			orderBy			(bool value)					throw ()	{__orderBy = value;}
	enum sqlOrder 	orderMode		() 						const	throw ()	{return __orderMode;}
	void			orderMode		(enum sqlOrder order) 			throw ()	{__orderMode = order;}

private :
	std::string		__columnName;		//nome della colonna
	sqlType			__columnType;		//tipo sql gestito
	unsigned	 	__length;			//numero di caratteri necessari a rappresentare un valore del tipo sqlType della colonna
	bool 			__isKey;			//true se la colonna compone la chiave di relazione
	std::string 	__defaultValue; 	//valore di default

	//membri usati durante query
	bool 				__project;			//true se la colonna viene usata in proiezione
	bool 				__select;			//true se la colonna viene usata in selezione
	std::string 		__selectValue;		//valore usato per la selezione
	enum sqlCompOp		__compareOperator;	//operatore di confronto usato in selezione
	bool 				__orderBy;			//true se si deve ordinare i risultati in base ai valori della colonna
	enum sqlOrder		__orderMode;		//modo in cui devino essere ordinati i risultati

	unsigned	getLength	()	 const	throw ();	//restituisce il numero di caratteri necessari a rappresentare un valore del tipo sqlType
};
};
#endif

