#ifndef __TEST_FUNCTION_HEADER__
#define __TEST_FUNCTION_HEADER__

#include <list>
#include <memory>
#include <string>
#include "../engine.hpp"

void print(bool _value);
void print(const std::list<std::string>& _list);
void print(enum engine::tuple::state _state);
void print(enum engine::column::sqlType _type);
void print(enum engine::column::sqlCompOp _operator);
void print(enum engine::column::sqlOrder _order);
void print(engine::schema::loadCommand_info _load);

std::unique_ptr<std::list <std::string>> tokenize (std::string _string);

void createDatabaseForTest(engine::database& _database);

#endif
