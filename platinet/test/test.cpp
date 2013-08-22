#include "../engine.hpp"
#include "../pqcc.hpp"
#include "../pgDatabase.hpp"
#include "test_function.hpp"
#include <iostream>
using namespace std;





int main() {

	engine::database _database("test");
	createDatabaseForTest(_database);

//	_database.project("public.articoli.codice_articolo", true);
//	_database.select("public.articoli.codice_articolo", true);
//	_database.selectValue("public.articoli.codice_articolo", "prova");
//	_database.compareOperator("public.articoli.codice_articolo", engine::column::more);
//	_database.orderBy("public.articoli.codice_articolo", true);
//	_database.orderMode("public.articoli.codice_articolo", engine::column::desc);
//	_database.orderBy("public.articoli.scorta_minima", true);
//	_database.orderMode("public.articoli.scorta_minima", engine::column::asc);

	engine::schema::loadCommand_info _info = _database.loadCommand("public.articoli");

	cout <<_info.__tablePath <<endl
	<<_info.__loadCommand <<endl;

	std::list<std::string> 	_tuple1;
		_tuple1.push_back("pnll01");
		_tuple1.push_back("pennello rettangolare 10x5 cm.");
		_tuple1.push_back("12.20");
		_tuple1.push_back("21");
		_tuple1.push_back("30");

	std::list<std::string> 	_tuple2;
		_tuple2.push_back("pnll02");
		_tuple2.push_back("pennello rettangolare 15x7 cm.");
		_tuple2.push_back("15.00");
		_tuple2.push_back("21");
		_tuple2.push_back("30");

	std::list<std::string> 	_tuple3;
		_tuple3.push_back("pnll03");
		_tuple3.push_back("pennello ovale 2x7 cm.");
		_tuple3.push_back("5.00");
		_tuple3.push_back("21");
		_tuple3.push_back("30");

	std::list<std::string> 	_tuple4;
		_tuple4.push_back("pnll04");
		_tuple4.push_back("pennello ovale 5x10 cm.");
		_tuple4.push_back("10.00");
		_tuple4.push_back("21");
		_tuple4.push_back("30");
try {
	unsigned long tuple1_id = _database.insertTuple("public.articoli", _tuple1, engine::tuple::loaded);
	unsigned long tuple2_id = _database.insertTuple("public.articoli", _tuple2, engine::tuple::inserting);
	unsigned long tuple3_id = _database.insertTuple("public.articoli", _tuple3, engine::tuple::loaded);
	unsigned long tuple4_id = _database.insertTuple("public.articoli", _tuple4, engine::tuple::inserting);

	_tuple3.clear();
		_tuple3.push_back("update");
		_tuple3.push_back("pennello ovale 2x7 cm.");
		_tuple3.push_back("5.00");
		_tuple3.push_back("21");
		_tuple3.push_back("30");

	_database.updateTuple("public.articoli", tuple3_id, _tuple3);
	_database.deleteTuple("public.articoli", tuple4_id);

	unique_ptr<list<string>> queryList = _database.commit("public.articoli");
	for (list<string>::const_iterator it = queryList->begin(); it!=queryList->end(); it++)
		cout <<*it <<endl;
}
catch (engine::exception& excpt) {cout <<excpt.what() <<endl;}
	

//	pgDatabase _pgDatabase("192.168.1.4", "5432", "prova", "platinet", "platinet");
//	_pgDatabase.connectBlock();
//	_pgDatabase.loadStructure();
//	std::unique_ptr<std::list<std::string>> dbStructure = _pgDatabase.structure();
//	for (std::list<std::string>::const_iterator it = dbStructure->begin(); it!=dbStructure->end(); it++)
//		cout <<*it <<endl;
}


