#include "../engine.hpp"
#include "../pqcc.hpp"
#include "../pgDatabase.hpp"
#include "test_function.hpp"
#include <iostream>
using namespace std;





int main() {

	engine::database _database("test");
	createDatabaseForTest(_database);

	_database.bulkInsert("public.articoli", "/tmp/platinet/test/articoli.txt");
	cin.get();
	_database.toHtml("public.articoli", "/tmp/platinet/test/articoli.html");
	cin.get();


	

//	pgDatabase _pgDatabase("192.168.1.4", "5432", "prova", "platinet", "platinet");
//	_pgDatabase.connectBlock();
//	_pgDatabase.loadStructure();
//	std::unique_ptr<std::list<std::string>> dbStructure = _pgDatabase.structure();
//	for (std::list<std::string>::const_iterator it = dbStructure->begin(); it!=dbStructure->end(); it++)
//		cout <<*it <<endl;
}


/*	TO DO LIST
 * - "in" e "not in" come operatori di confronto per select value
 * - implementare tipi sql character, numeric
 * - migliorare supporto per tipi date e time
 * - aggiungere funzioni per project, select, selectValue, orderBy e orderMode con maschera
 * - unit test per pgConnection
 * - unit test per pgQuery
 * - unit test per pgDriver
 *
 *
 *
 *
 */
