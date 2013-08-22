#include "test_function.hpp"
#include <iostream>

void print(bool _value) {
	if (_value)
		std::cout <<"true";
	else
		std::cout <<"false";
}

void print(const std::list<std::string>& _list) {
	std::cout<<"(";
	for(std::list<std::string>::const_iterator it = _list.begin(); it != _list.end(); it++) {
		if (it != _list.begin() && it != _list.end()) std::cout <<", ";
		std::cout <<"'" <<*it <<"'";
	}
	std::cout<<")";
}

void print(enum engine::tuple::state _state) {
	switch (_state) {
		case engine::tuple::loaded :
			std::cout <<"engine::tuple::loaded" <<std::endl;
			break;

		case engine::tuple::empty :
			std::cout <<"engine::tuple::empty" <<std::endl;
			break;
		case engine::tuple::inserting :
			std::cout <<"engine::tuple::inserting" <<std::endl;
			break;

		case engine::tuple::updating :
			std::cout <<"engine::tuple::updating" <<std::endl;
			break;

		case engine::tuple::deleting :
			std::cout <<"engine::tuple::deleting" <<std::endl;
			break;
		}
}

void print(enum engine::column::sqlType _type) {
	switch (_type) {
			case engine::column::boolean:
				std::cout <<"boolean;" <<std::endl; 
				break;
			case engine::column::varchar:
				std::cout <<"varchar" <<std::endl;
				break;
			case engine::column::bigint :
				std::cout <<"big integer;" <<std::endl;
				break;
			case engine::column::integer:
				std::cout <<"integer;" <<std::endl;
				break;
			case engine::column::smallint:
				std::cout <<"small integer;" <<std::endl;
				break;
			case engine::column::doublePrecision:
				std::cout <<"double precision floating point;" <<std::endl;
				break;
			case engine::column::real:
				std::cout <<"single precision floating point;" <<std::endl;
				break;
			case engine::column::date:
				std::cout <<"date in 'YYYY-MM-DD' format;" <<std::endl;
				break;
			case engine::column::time:
				std::cout <<"time without timezone in 'HH:MM:SS' format;" <<std::endl;
				break;
		}
}

void print(engine::schema::loadCommand_info _load) {
	std::cout <<"__tablePath = " <<_load.__tablePath <<"; " <<std::endl
	<<"__loadCommand = " <<_load.__loadCommand <<std::endl;
}

void print(enum engine::column::sqlCompOp _operator) {
	switch(_operator) {
	case engine::column::more:
		std::cout<<">" <<std::endl;
		break;
	case engine::column::moreEqual:
		std::cout<<">=" <<std::endl;
		break;
	case engine::column::equal:
		std::cout<<"=" <<std::endl;
		break;
	case engine::column::disequal:
		std::cout<<"<>" <<std::endl;
		break;
	case engine::column::like:
		std::cout<<"like" <<std::endl;
		break;
	case engine::column::notLike:
		std::cout<<"not like" <<std::endl;
		break;
	case engine::column::lessEqual:
		std::cout<<"<=" <<std::endl;
		break;
	case engine::column::less:
		std::cout<<"<" <<std::endl;
		break;
	}
}

void print(enum engine::column::sqlOrder _order) {
	switch(_order) {
	case engine::column::asc:
		std::cout<<"asc" <<std::endl;
		break;
	case engine::column::desc:
		std::cout<<"desc" <<std::endl;
		break;
	}
}

std::unique_ptr<std::list <std::string>> tokenize (std::string _string) {
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

void createDatabaseForTest(engine::database& _database) {
	_database.addSchema("public");
	_database.addTable("public.tabella_test_tipi");
	_database.addColumn("public.tabella_test_tipi.colonna_boolean", engine::column::boolean, false, "true");
	_database.addColumn("public.tabella_test_tipi.colonna_varchar", engine::column::varchar, 200, false, "valore default varchar");
	_database.addColumn("public.tabella_test_tipi.colonna_bigint", engine::column::bigint, 0, false, "123456789");
	_database.addColumn("public.tabella_test_tipi.colonna_integer", engine::column::integer, 0, false, "1234");
	_database.addColumn("public.tabella_test_tipi.colonna_smallint", engine::column::smallint, 0, false, "12");
	_database.addColumn("public.tabella_test_tipi.colonna_double", engine::column::doublePrecision, 0, false, "123456.789");
	_database.addColumn("public.tabella_test_tipi.colonna_real", engine::column::real, 0, false, "1234567.89");
	_database.addColumn("public.tabella_test_tipi.colonna_date", engine::column::date, 0, false, "12/12/2013");
	_database.addColumn("public.tabella_test_tipi.colonna_time", engine::column::time, 0, false, "12:34:56");

	_database.addTable("public.articoli");
	_database.addColumn("public.articoli.codice_articolo", engine::column::varchar, 10, true);
	_database.addColumn("public.articoli.nome_articolo", engine::column::varchar, 150);
	_database.addColumn("public.articoli.imponibile", engine::column::real);
	_database.addColumn("public.articoli.aliquota_iva", engine::column::smallint);
	_database.addColumn("public.articoli.scorta_minima", engine::column::integer);
}
