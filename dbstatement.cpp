#include "dbstatement.hpp"
#include "Attribute.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "Database.hpp"
#include <string>
#include <sstream>
#include "Value.hpp"
#include "Row.hpp"
#include "Helpers.hpp"
#include "Filters.hpp"
namespace ECE141 {
	int aNumk = 1;
//-------------Createstatement------------------//
	bool inRange(int a, int b, int i) {
		return i >= a && i <= b;
	}

	bool isvalidname(std::string name) {
		if (name[0] == '_')return false;
		for (int i = 0; i < name.size(); ++i) {
			if (!inRange(0, 9, name[i] - '0') && !inRange(0, 23, name[i] - 'a')
				&& !inRange(0, 23, name[i] - 'A') && name[i] != '_') {
				return false;
			}
		}
		return true;
	}

	CreateStatement::CreateStatement() : Statement(Keywords::create_kw) {}
	CreateStatement::CreateStatement(CreateStatement& aCopy) : Statement(Keywords::create_kw) {}
	CreateStatement::~CreateStatement() {}
	StatusResult  CreateStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().keyword != Keywords::database_kw) {
				return StatusResult(Errors::unknownCommand);
			}
			else if (aTokenizer.size() > 3)return StatusResult(Errors::illegalIdentifier);
		}
		if (aTokenizer.next() && aTokenizer.more()) {
			if (isvalidname(aTokenizer.current().data)) {
				std::string tempath = StorageInfo::getDefaultStoragePath();
				std::string aName = aTokenizer.current().data;
				const std::filesystem::path aPath = tempath + '/' + aName + ".txt";
				const std::filesystem::path aIndexPath = tempath + '/' + aName + "_index" + ".txt";
				if (std::filesystem::exists(aPath)) {
					return StatusResult(Errors::databaseExists);
				}
				std::ofstream dbf1(aPath);
				std::ofstream dbf2(aIndexPath);
				Database aDatabase(aName, CreateNewStorage());
				aDatabase.createDatabase(aName);
				std::cout << "created database " << aName << " (ok)" "\n";
				aTokenizer.next();
				return ECE141::StatusResult();
			}
			else {
				aTokenizer.next();
				return StatusResult(Errors::illegalIdentifier);
			}
		}
		return StatusResult(Errors::unknownCommand);
	}

	StatusResult  CreateStatement::run(std::ostream& aStream) const {
		return StatusResult(Errors::noError);
	}

//-------------Dropstatement------------------//
	DropStatement::DropStatement() : Statement(Keywords::drop_kw) {}
	DropStatement::DropStatement(DropStatement& aCopy) : Statement(Keywords::drop_kw) {}
	DropStatement::~DropStatement() {}
	StatusResult  DropStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().data == "database") {
				if (aTokenizer.size() > 3)return StatusResult(Errors::unknownDatabase);
				if (aTokenizer.next() && aTokenizer.more()) {
					std::string aName = aTokenizer.current().data;
					std::string tempath = StorageInfo::getDefaultStoragePath();
					const std::filesystem::path aPath = tempath + '/' + aName + ".txt";
					if (std::filesystem::exists(aPath)) {
						Database aDatabase(aName, DropExistingStorage());
						aDatabase.dropDatabase(aName);
						std::cout << "dropped database " << aName << " (ok)" << "\n";
						aTokenizer.next();
						return StatusResult(Errors::noError);
					}
					else {
						aTokenizer.next();
						return StatusResult(Errors::unknownDatabase);
					}
				}
			}
		}
		return StatusResult(Errors::unknownCommand);
	}

	StatusResult  DropStatement::run(std::ostream& aStream) const { return StatusResult(Errors::noError); }

//-------------Showstatement------------------//
	ShowStatement::ShowStatement() : Statement(Keywords::show_kw) {}
	ShowStatement::ShowStatement(ShowStatement& aCopy) : Statement(Keywords::show_kw) {}
	ShowStatement::~ShowStatement() {}
	StatusResult  ShowStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().data != "databases") {
				return StatusResult(Errors::unknownCommand);
			}
			else {
				aTokenizer.next();
				return this->run(std::cout);
			}

		}
		return StatusResult(Errors::unknownCommand);
	}

	StatusResult  ShowStatement::run(std::ostream& aStream) const {
		std::string tempath = StorageInfo::getDefaultStoragePath();
		std::filesystem::path aPath = tempath;
		for (auto i : std::filesystem::directory_iterator(aPath)) {
			std::string dbpath = i.path().string();
			size_t position1 = dbpath.find_last_of('/');
			size_t position2 = dbpath.find_last_of('.');
			std::string dbname(dbpath.substr(position1 + 1, position2 - position1 - 1));
			aStream << dbname << "\n";
		}
		return StatusResult(Errors::noError);
	}

//-------------Usestatement------------------//
	UseStatement::UseStatement(SQLProcessor* aProcess) : Statement(Keywords::use_kw), Process(aProcess) {}
	UseStatement::UseStatement(UseStatement& aCopy) : Statement(Keywords::use_kw), Process(aCopy.Process) {}
	UseStatement::~UseStatement() {}
	StatusResult  UseStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more())if (!(aTokenizer.current().data == "database"))return StatusResult(Errors::unknownCommand);
		if (aTokenizer.next() && aTokenizer.more()) {
			std::string tempath = StorageInfo::getDefaultStoragePath();
			std::string aName = aTokenizer.current().data;
			const std::filesystem::path aPath = tempath + '/' + aName + ".txt";
			if (std::filesystem::exists(aPath)) {
				if (Process->inUseDB) {
					//if DB is in use, save changes and close it
					if (Process->inUseDB->getName() != aName) {
						delete Process->inUseDB;
						Process->inUseDB = new Database(aName, OpenExistingStorage());
					}
				}
				else {
					Process->inUseDB = new Database(aName, OpenExistingStorage());
				}
				if (aTokenizer.next() && aTokenizer.more()) {
					while (aTokenizer.next());
					return StatusResult(Errors::unknownCommand);
				}
				std::cout << "using " << aName << "(ok)" << "\n";
				return StatusResult(Errors::noError);
			}
			else {
				while (aTokenizer.next());
				return StatusResult(Errors::unknownDatabase);
			}
		}
		while (aTokenizer.next());
		return StatusResult(Errors::unknownCommand);
	}

	StatusResult  UseStatement::run(std::ostream& aStream) const {
		return StatusResult(Errors::noError);
	}

//-------------Describestatement------------------//
	DescribeStatement::DescribeStatement() : Statement(Keywords::describe_kw) {}
	DescribeStatement::DescribeStatement(DescribeStatement& aCopy) : Statement(Keywords::describe_kw) {}
	DescribeStatement::~DescribeStatement() {}
	StatusResult  DescribeStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().keyword != Keywords::database_kw)
				return StatusResult(Errors::unknownCommand);
			else if (aTokenizer.size() > 3)return StatusResult(Errors::unknownDatabase);
		}
		if (aTokenizer.next() && aTokenizer.more()) {
			std::string tempath = StorageInfo::getDefaultStoragePath();
			std::string aName = aTokenizer.current().data;
			const std::filesystem::path aPath = tempath + '/'+aName + ".txt";
			if (std::filesystem::exists(aPath)) {
				aTokenizer.next();
				return run(std::cout, aName);
			}
			else {
				aTokenizer.next();
				return StatusResult(Errors::unknownDatabase);
			}
		}
		return StatusResult();
	}

	StatusResult  DescribeStatement::run(std::ostream& aStream, std::string aName) const {
		Database aDatabase(aName, OpenExistingStorage{});
		aDatabase.describeDatabase(aStream, aName);
		return StatusResult(Errors::noError);
	}

	//-------------CreateTablestatement------------------//
	static std::map<Keywords, DataType> key2data = {
		std::make_pair(Keywords::float_kw, DataType::float_type),
		std::make_pair(Keywords::integer_kw, DataType::int_type),
		std::make_pair(Keywords::varchar_kw, DataType::varchar_type),
		std::make_pair(Keywords::datetime_kw, DataType::datetime_type),
		std::make_pair(Keywords::timestamp_kw, DataType::timestamp_type),
		std::make_pair(Keywords::boolean_kw, DataType::bool_type)
	};

	StatusResult parseVar(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().data == "(" && aTokenizer.next() && aTokenizer.more()) {
				if (aTokenizer.current().type == TokenType::number) {
					int num = std::stoi(aTokenizer.current().data);
					if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().data == ")") {
						aTokenizer.next();
						return StatusResult(Errors::noError, num);
					}
				}
			}
		}
		return StatusResult(Errors::unknownCommand);
	}

	StatusResult parseAttribute(Tokenizer& aTokenizer, Attribute& aAttribute) {
		aAttribute.setName(aTokenizer.current().data);
		aTokenizer.next();
		StatusResult aResult;
		aAttribute.setType(key2data[aTokenizer.current().keyword]);
		if (aTokenizer.current().keyword == Keywords::varchar_kw) {
			aResult = parseVar(aTokenizer);
			if (aResult.code == Errors::unknownCommand)return aResult;
			aAttribute.setSize(aResult.value);
		}
		else {
			aTokenizer.next();
		}
		while (aTokenizer.current().type == TokenType::keyword) {
			switch (aTokenizer.current().keyword)
			{
			case Keywords::auto_increment_kw:
				aAttribute.setAutoIncrement(true);
				break;
			case Keywords::not_kw:
				aTokenizer.next();
				if (aTokenizer.current().keyword == Keywords::null_kw)aAttribute.setNullable(false);
				else return StatusResult(unknownCommand);
				break;
			case Keywords::primary_kw:
				aTokenizer.next();
				if (aTokenizer.current().keyword == Keywords::key_kw)aAttribute.setPrimaryKey(true);
				else return StatusResult(unknownCommand);
				break;
			case Keywords::default_kw:
				aTokenizer.next();
				if (!aTokenizer.more())return StatusResult(unknownCommand);
				if (aTokenizer.current().type == TokenType::number) {
					if (aAttribute.getType() == DataType::float_type) {
						std::string fNum = aTokenizer.current().data;
						aAttribute.setDefaultValue(fNum);
					}
					else if (aAttribute.getType() == DataType::int_type) {
						std::string num = aTokenizer.current().data;
						aAttribute.setDefaultValue(num);
					}
				}
				else if (aTokenizer.current().type == TokenType::identifier) {
					std::string idd = aTokenizer.current().data;
					transform(idd.begin(), idd.end(), idd.begin(), ::toupper);
					if (idd == "FALSE") {
						aAttribute.setDefaultValue("FALSE");
					}
					else if (idd == "TRUE") {
						aAttribute.setDefaultValue("TRUE");
					}
					else aAttribute.setDefaultValue("NULL");
				}
				break;
			default:
				break;
			}
			aTokenizer.next();
		}
		return StatusResult();
	}

	CreateTableStatement::CreateTableStatement() : Statement(Keywords::create_kw) {}
	CreateTableStatement::CreateTableStatement(SQLProcessor* aProcess) : Statement(Keywords::create_kw), process(aProcess) {}
	CreateTableStatement::CreateTableStatement(CreateTableStatement& aCopy) : Statement(Keywords::create_kw), process(aCopy.process) {}
	CreateTableStatement::~CreateTableStatement() {}
	StatusResult  CreateTableStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().keyword != Keywords::table_kw) {
				return StatusResult(Errors::unknownCommand);
			}
		}
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().type != TokenType::identifier)return StatusResult(Errors::unknownCommand);
			Schema aSchema(aTokenizer.current().data);
			if (aTokenizer.next() && aTokenizer.more()) {
				Token curToken = aTokenizer.current();
				Token nextToken;
				if (curToken.type != TokenType::punctuation || curToken.data != "(")return StatusResult(Errors::unknownCommand);
				while (aTokenizer.next() && aTokenizer.more()) {
					curToken = aTokenizer.current();
					if (!aTokenizer.hasnext())return StatusResult(Errors::unknownCommand);
					nextToken = aTokenizer.peek(1);
					if (curToken.type != TokenType::identifier || key2data.find(nextToken.keyword) == key2data.end()) {
						return StatusResult(Errors::unknownCommand);
					}
					Attribute aAttribute;
					StatusResult aResult;
					aResult = parseAttribute(aTokenizer, aAttribute);
					if (aResult.code == Errors::unknownCommand)return aResult;
					aSchema.addAttribute(aAttribute);
					curToken = aTokenizer.current();
					if (curToken.type != TokenType::punctuation)return StatusResult(Errors::unknownCommand);
					if (curToken.data == ")")break;
				}
				if (curToken.data == ",")return StatusResult(Errors::unknownCommand);
				if (aTokenizer.next() && aTokenizer.more()) {
					while (aTokenizer.next());
					return StatusResult(Errors::unknownCommand);
				}
				return process->createTable(aSchema);
			}
			else return StatusResult();
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult  CreateTableStatement::run(std::ostream& aStream, std::string aName) const { return StatusResult(); }

//-------------ShowTablestatement------------------//
	ShowTableStatement::ShowTableStatement() :Statement(Keywords::show_kw) {}
	ShowTableStatement::ShowTableStatement(SQLProcessor* aProcess) : Statement(Keywords::show_kw), process(aProcess) {}
	ShowTableStatement::ShowTableStatement(ShowTableStatement& aCopy) : Statement(Keywords::show_kw), process(aCopy.process) {}
	ShowTableStatement::~ShowTableStatement() {}
	StatusResult ShowTableStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().keyword != Keywords::tables_kw) {
				return StatusResult(Errors::unknownCommand);
			}
			if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
			aTokenizer.next();
			return process->showTables();
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult ShowTableStatement::run(std::ostream& aStream, std::string aName) const { return StatusResult(); }

//-------------DescribeTablestatement------------------//
	DescribeTableStatement::DescribeTableStatement() :Statement(Keywords::describe_kw) {}
	DescribeTableStatement::DescribeTableStatement(DescribeTableStatement& aCopy) : Statement(Keywords::describe_kw), process(aCopy.process) {}
	DescribeTableStatement::DescribeTableStatement(SQLProcessor* aProcess) : Statement(Keywords::describe_kw), process(aProcess) {}
	DescribeTableStatement::~DescribeTableStatement() {}
	StatusResult DescribeTableStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.next() && aTokenizer.more()) {
			std::string name(aTokenizer.current().data);
			if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
			aTokenizer.next();
			return process->describeTable(name);
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult DescribeTableStatement::run(std::ostream& aStream, std::string aName) const { return StatusResult(); }

//-------------DropTablestatement------------------//
	DropTableStatement::DropTableStatement() :Statement(Keywords::drop_kw) {}
	DropTableStatement::DropTableStatement(SQLProcessor* aProcess) : Statement(Keywords::drop_kw), process(aProcess) {}
	DropTableStatement::DropTableStatement(DropTableStatement& aCopy) : Statement(Keywords::drop_kw), process(aCopy.process) {}
	DropTableStatement::~DropTableStatement() {}
	StatusResult DropTableStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().keyword != Keywords::table_kw) {
				return StatusResult(Errors::unknownCommand);
			}
		}
		if (aTokenizer.next() && aTokenizer.more()) {
			std::string name(aTokenizer.current().data);
			if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
			aTokenizer.next();
			return process->dropTable(name);
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult DropTableStatement::run(std::ostream& aStream, std::string aName) const { return StatusResult(); }

//-------------Insertstatement------------------//
	InsertStatement::InsertStatement() :Statement(Keywords::insert_kw) {}
	InsertStatement::InsertStatement(SQLProcessor* aProcess) : Statement(Keywords::insert_kw), process(aProcess) {}
	InsertStatement::InsertStatement(InsertStatement& aCopy) : Statement(Keywords::insert_kw), process(aCopy.process) {}
	InsertStatement::~InsertStatement() {}

	StatusResult InsertStatement::findTable(Database* inUseDB, std::string tablename) {
		int bn;
		inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
		inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
		while (!inUseDB->getStorage().anotherstream.eof()) {
			std::string tables, table, bN;
			std::getline(inUseDB->getStorage().anotherstream, tables);
			if (tables == "")break;
			std::stringstream aStringstream;
			aStringstream << tables;
			aStringstream >> table;
			aStringstream >> bN;
			bn = std::stoi(bN);
			if (tablename == table)return StatusResult(Errors::noError, bn);
		}
		inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
		inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
		return StatusResult(Errors::unknownTable);
	}

	StatusResult InsertStatement::parseField(Tokenizer& aTokenizer, std::vector<std::string>& list) {
		if (aTokenizer.current().data == "(") {
			if (aTokenizer.next() && aTokenizer.more()) {
				while (aTokenizer.current().data != ")") {
					if (aTokenizer.current().type == TokenType::identifier) {
						list.push_back(aTokenizer.current().data);
						if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().data == ",") {
							aTokenizer.next();
							continue;
						}
					}
					else {
						while (aTokenizer.next());
						return StatusResult(Errors::unknownCommand);
					}
				}
			}
		}
		return StatusResult();
	}

	StatusResult InsertStatement::parseValue(Tokenizer& aTokenizer, std::vector<ValueType>& list) {
		if (aTokenizer.current().data == "(") {
			if (aTokenizer.next() && aTokenizer.more()) {
				while (aTokenizer.current().data != ")") {
					if(!aTokenizer.more())return StatusResult(Errors::unknownCommand);
					if (aTokenizer.current().data == "'") {
						if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().type == TokenType::identifier) {
							list.push_back(aTokenizer.current().data);
							if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().data == "'") {
								if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().data == ",") {
									aTokenizer.next();
								}
							}
							else {
								while (aTokenizer.next());
								return StatusResult(Errors::unknownCommand);
							}
						}
						else if (aTokenizer.current().data == "'") {
							list.push_back("");
							if (aTokenizer.next() && aTokenizer.more()){
								if (aTokenizer.current().data == ",") {
									aTokenizer.next();
								}
							}
							else return StatusResult(Errors::unknownCommand);
						}
						else {
							while (aTokenizer.next());
							return StatusResult(Errors::unknownCommand);
						}
					}
					else {
						std::string str = aTokenizer.current().data;
						if (aTokenizer.current().type == TokenType::identifier) {
							if (str == "true")list.push_back(true);
							else if (str == "false")list.push_back(false);
							else list.push_back(str);
							if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().data == ",") {
								aTokenizer.next();
							}
						}
						else if (aTokenizer.current().type == TokenType::number) {
							int pos = str.find_first_of(".");
							if (pos == -1) {
								uint32_t v = std::stoi(str);
								list.push_back(v);
							}
							else { 
								std::stringstream ss(str);
								float tr;
								if ((ss >> tr).fail())std::clog << "494 in dbstatement" << "\n";
								list.push_back(std::stof(str)); 
							}
							if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().data == ",") {
								aTokenizer.next();
							}
						}
						else return StatusResult(Errors::unknownCommand);
					}
					if (!aTokenizer.more())return StatusResult(Errors::unknownCommand);
				}
			}
		}
		else return StatusResult(Errors::unknownCommand);
		return StatusResult();
	}

	std::string to_string(ValueType val) {
		switch (val.index()) {
		case 0:
			return std::string(std::to_string(std::get<0>(val)));
			break;
		case 1:
			return std::string(std::to_string(std::get<1>(val)));
			break;
		case 2:
			if(std::get<2>(val) == true)
			return std::string("TRUE");
			else return std::string("FALSE");
			break;
		default:
			return std::get<3>(val);
			break;
		}
	}

	bool validate(std::string& data, std::string attributes, std::vector<std::string> fields, std::vector<ValueType> values) {
		if (fields.size() != values.size()) return false;
		const char* d = ",";
		char* p;
		p = strtok((char*)attributes.c_str(), d);
		std::map<std::string, std::pair <int, ValueType>> keyValueList;
		std::map<std::string, int> nameSize;
		while (p)
		{
			int pos1;
			std::stringstream ss(p);
			std::string f, v, de;
			ss >> f;			
			ss >> v;
			if (f == "")break;
			int i = 3;
			while (i > 0) {
				de = "";
				ss >> de;
				--i;
			}
			switch (v[0])
			{
			case 'i':
				if (de == "NULL") {
					if (f == "id") { 
						uint32_t defaultV = aNumk;
						keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(0, defaultV)));
						aNumk=aNumk+1; 
					}
					else keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(0, de)));
				}
				else {
					uint32_t defaultV = std::stoi(de);
					keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(0, defaultV)));
				}
				break;
			case 'f':
				if (de == "NULL") {
					keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(1, de)));
				}
				else {
					std::stringstream ss(de);
					float defaultV;
					if ((ss >> defaultV).fail())defaultV = 0;
					keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(1, defaultV)));
				}
				break;
			case 'b':
				if (de == "NULL") {
					keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(2, de)));
				}
				else {
					bool defaultV;
					if (de == "FALSE")defaultV = false;
					else defaultV = true;
					keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(2, defaultV)));
				}
				break;
			case 'v':
				pos1 = v.find_first_of("(") + 1;
				if (pos1 != 0) {
					nameSize.insert(std::pair(f, std::stoi(v.substr(pos1, v.find_first_of(")") - pos1))));
				}
				keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(3, de)));
				break;
			case 'd':
				keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(4, de)));
				break;
			case 't':
				keyValueList.insert(std::pair<std::string, std::pair <int, ValueType>>(f, std::pair(5, de)));
				break;
			default:			
				break;
			}
			p = strtok(NULL, d);
		}
		for (int i = 0; i < fields.size(); ++i) {
			auto it = keyValueList.find(fields[i]);
			if (it == keyValueList.end())return false;
			else {
				int index = values[i].index();
				if (index < 3) {
					if (!it->second.first == index)return false;
					else {
						it->second.second = values[i];
					}
				}
				else {
					std::string val = std::get<std::string>(values[i]);
					if (index == 3) {
						auto i = nameSize.find(it->first);
						if (i != nameSize.end()) {
							if (i->second < val.size())return false;
						}
						it->second.second = val;
					}
					else it->second.second = val;
				}
			}
		}
		for (auto it = keyValueList.begin(); it != keyValueList.end(); ++it) {
			data = data + it->first + " " + to_string(it->second.second)+",";
		}
		return true;
	}

	StatusResult InsertStatement::getTableInsert(Database* inUseDB, int bn, std::vector<std::string> fields, std::vector<ValueType> values) {
		int i = bn;
		int freeBN = inUseDB->getStorage().findFreeBlockNum().value;
		std::string attributes, index;
		inUseDB->getStorage().stream.clear(std::ios::goodbit);
		inUseDB->getStorage().stream.seekg(std::ios::beg);
		while (i != 0) {
			attributes = "";
			std::getline(inUseDB->getStorage().stream, attributes);
			--i;
		}
		int position1 = attributes.find_first_of("|")+1;
		int length = attributes.find_last_of("]")-position1;
		attributes = attributes.substr(position1, length);
		std::getline(inUseDB->getStorage().stream, index);
		uint32_t pos = index.find_last_of(",");
		if (pos == -1)pos = index.find_first_of("|");
		i = bn;
		inUseDB->getStorage().stream.clear(std::ios::goodbit);
		inUseDB->getStorage().stream.seekg(std::ios::beg);
		inUseDB->getStorage().stream.seekp(std::ios::beg);
		while (i != 0) {
			std::string blocks;
			std::getline(inUseDB->getStorage().stream, blocks);
			--i;
		}
		uint32_t p = inUseDB->getStorage().stream.tellg();
		std::string data;
		if(!validate(data, attributes, fields, values))return StatusResult(Errors::unknownCommand);
		inUseDB->getStorage().stream.seekp(pos + 1 + p);
		inUseDB->getStorage().stream << freeBN << ",";
		inUseDB->getStorage().update();
		return process->insert(freeBN, data);
	}

	StatusResult InsertStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().keyword != Keywords::into_kw) {
				return StatusResult(Errors::unknownCommand);
			}
		}
		if (aTokenizer.next() && aTokenizer.more()) {
			std::string tableName = aTokenizer.current().data;
			auto inUseDB = process->getActiveDatabase();
			StatusResult aResult = findTable(inUseDB, tableName);
			if(!aResult)return StatusResult(Errors::unknownTable);
			if (aTokenizer.next() && aTokenizer.more()) {
				std::vector<std::string> fields;
				if(parseField(aTokenizer, fields) == StatusResult(unknownCommand))return StatusResult(Errors::unknownCommand);
				if (aTokenizer.next() && aTokenizer.more()) {
					if(aTokenizer.current().keyword != Keywords::values_kw)return StatusResult(Errors::unknownCommand);
					StatusResult result;
					while (aTokenizer.next() && aTokenizer.more()) {
						std::vector<ValueType> values;
						if (parseValue(aTokenizer, values) == StatusResult(unknownCommand))return StatusResult(Errors::unknownCommand);
						result = getTableInsert(inUseDB, aResult.value, fields, values);
						if (result.code == Errors::unknownCommand)return result;
						if (aTokenizer.next() && aTokenizer.more()) {
							if(aTokenizer.current().data!=",")return StatusResult(Errors::unknownCommand);
						}
					}
					return result;
				}
			}
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult InsertStatement::run(std::ostream& aStream, std::string aName) const { return StatusResult(); }

//-------------DeleteTablestatement------------------//
	DeleteStatement::DeleteStatement() :Statement(Keywords::delete_kw) {}
	DeleteStatement::DeleteStatement(SQLProcessor* aProcess) : Statement(Keywords::delete_kw), process(aProcess) {}
	DeleteStatement::DeleteStatement(DeleteStatement& aCopy) : Statement(Keywords::delete_kw), process(aCopy.process) {}
	DeleteStatement::~DeleteStatement() {}
	StatusResult DeleteStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
		auto inUseDB = process->getActiveDatabase();
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().keyword == Keywords::from_kw) {
				if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().type == TokenType::identifier) {
					std::string tableName = aTokenizer.current().data;
					TableHelper aHelper;
					if (aTokenizer.next() && aTokenizer.more()) {
						while (aTokenizer.next());
						return StatusResult(Errors::unknownCommand);
					}
					StatusResult aResult = aHelper.findTable(inUseDB, tableName);
					if (!aResult)return StatusResult(Errors::unknownTable);
					return process->deleteData(aResult.value);
				}
			}
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult DeleteStatement::run(std::ostream& aStream, std::string aName) const { return StatusResult(); }

	SelectStatement::SelectStatement() : Statement(Keywords::select_kw){}
	SelectStatement::SelectStatement(SQLProcessor* aProcess) : Statement(Keywords::select_kw), process(aProcess) {}
	SelectStatement::SelectStatement(SelectStatement& aCopy) : Statement(Keywords::select_kw), process(aCopy.process) {}
	SelectStatement::~SelectStatement() {}
	StatusResult  SelectStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
		TableHelper aHelper;
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().type == TokenType::keyword)return StatusResult(Errors::unknownCommand);
			std::set<std::string> dataNameList;
			do{
				if (aTokenizer.current().data == ",")continue;
				dataNameList.insert(aTokenizer.current().data);
			} while (aTokenizer.next()&&aTokenizer.more()&&aTokenizer.current().type != TokenType::keyword);
			Database* inUseDB;
			std::string tableName;
			StatusResult aResult;
			if (aTokenizer.current().keyword == Keywords::from_kw && aTokenizer.peek(3).keyword== Keywords::join_kw&&aTokenizer.next() && aTokenizer.more()) {
				inUseDB = process->getActiveDatabase();
				tableName = aTokenizer.current().data;
				aResult = aHelper.findTable(inUseDB, tableName);
				if (!aResult)return aResult;
			}
			else if (aTokenizer.current().keyword == Keywords::from_kw && aTokenizer.next() && aTokenizer.more()) {
				inUseDB = process->getActiveDatabase();
				tableName = aTokenizer.current().data;
				aResult = aHelper.findTable(inUseDB, tableName);
				std::map<std::string, std::string> dataTypePairs;
				if (!aResult)return aResult;
				if (dataNameList.find("*") == dataNameList.end()) {
					StatusResult anotherResult = aHelper.findData(inUseDB, dataNameList, aResult.value, dataTypePairs);
					if (!anotherResult)return anotherResult;
				}
				else {
					dataTypePairs.insert({ "*", "*" });
				}
			}
			else return StatusResult(Errors::unknownCommand);
			Filters aFilter;
			bool isleft = false;
			while (aTokenizer.next()&&aTokenizer.more()) {
				if(aTokenizer.current().type!=TokenType::keyword)return StatusResult(Errors::unknownCommand);
				switch (aTokenizer.current().keyword)
				{
				case(Keywords::left_kw):
					isleft = true;
				case(Keywords::right_kw): {
					aTokenizer.next(2);
					std::string anotherTable(aTokenizer.current().data);
					StatusResult aResult2 = aHelper.findTable(inUseDB, anotherTable);
					aTokenizer.next(4);
					std::string compField1(aTokenizer.current().data);
					aTokenizer.next(4);
					std::string compField2(aTokenizer.current().data);
					while (aTokenizer.next());
					return process->seleteJoinData(aResult.value, aResult2.value, compField1, compField2, dataNameList, isleft);
					break; 
				}
				case(Keywords::where_kw):
					if (aTokenizer.next() && aTokenizer.more()&&aTokenizer.current().type == TokenType::identifier) {
						std::string iden = aTokenizer.current().data;
						StatusResult anotherResult1 = aHelper.findAttr(inUseDB, iden, aResult.value);
						if (!anotherResult1)return anotherResult1;
						if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().type == TokenType::operators) {
							Operators oper = aTokenizer.current().op;
							if (aTokenizer.next() && aTokenizer.more()) {
								if (aTokenizer.current().data == "'") {
									if (aTokenizer.next() && aTokenizer.more()) {
										std::string da = aTokenizer.current().data;
										Operand lo = Operand(iden, TokenType::identifier);
										Operand ro = Operand(da, TokenType::string);
										Expression aEx(lo, oper, ro);
										aFilter.add(aEx);
										if (aTokenizer.next() && aTokenizer.more()&&aTokenizer.current().data == "'") {
											break;
										}
									}
								}
								else {
									if (aTokenizer.current().type == TokenType::number) {
										Operand lo = Operand(iden, TokenType::identifier);
										if (aTokenizer.current().data.find(".") != -1) {
											std::stringstream ss(aTokenizer.current().data);
											float tr;
											if ((ss >> tr).fail())std::clog << "766 in dbstatement" << "\n";
											ValueType value(std::stof(aTokenizer.current().data));
											Operand ro = Operand(aTokenizer.current().data, TokenType::number, value);
											Expression aEx(lo, oper, ro);
											aFilter.add(aEx);
											break;
										}
										else {
											uint32_t a;
											std::stringstream ss(aTokenizer.current().data);
											ss >> a;
											ValueType value = a;
											Operand ro = Operand(aTokenizer.current().data, TokenType::number, value);
											Expression aEx(lo, oper, ro);
											aFilter.add(aEx);
											break;
										}
									}
									else {
										ValueType value = aTokenizer.current().data == "true"? true : false;
										Operand lo = Operand(iden, TokenType::identifier);
										Operand ro = Operand(aTokenizer.current().data, TokenType::number, value);
										Expression aEx(lo, oper, ro);
										aFilter.add(aEx);
										break;
									}
								}
							}
						}
					}
					return StatusResult(Errors::unknownCommand);
				case(Keywords::limit_kw):
					if (aTokenizer.next() && aTokenizer.more()&&aTokenizer.current().type==TokenType::number) {
						aFilter.setLimit(std::stoi(aTokenizer.current().data));
						break;
					}
					return StatusResult(Errors::unknownCommand);
				case(Keywords::order_kw):
					if (aTokenizer.next() && aTokenizer.more()) {
						if (aTokenizer.current().keyword == Keywords::by_kw) {
							if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().type == TokenType::identifier) {
								std::string iden = aTokenizer.current().data;
								std::set<std::string> ide{ iden };
								std::map<std::string, std::string> noUse;
								StatusResult anotherResult2 = aHelper.findData(inUseDB, ide, aResult.value, noUse);
								if (!anotherResult2)return anotherResult2;
								aFilter.hasOrder = true;
								aFilter.orderBy = aTokenizer.current().data;
								while (aTokenizer.next());
								break;
							}
						}
					}
					return StatusResult(Errors::unknownCommand);
					break;
				default:
					break;
				}
				return process->seleteData(aResult.value, aFilter, dataNameList);
			}
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult  SelectStatement::run(std::ostream& aStream, std::string aName) const {
		return StatusResult();
	}


	UpdateStatement::UpdateStatement() : Statement(Keywords::update_kw){}
	UpdateStatement::UpdateStatement(SQLProcessor* aProcess) : Statement(Keywords::update_kw), process(aProcess) {}
	UpdateStatement::UpdateStatement(UpdateStatement& aCopy) : Statement(Keywords::update_kw), process(aCopy.process){}
	StatusResult UpdateStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
		TableHelper aHelper;
		if (aTokenizer.next() && aTokenizer.more()) {
			if (aTokenizer.current().type != TokenType::identifier)return StatusResult(Errors::unknownCommand);
			std::string tableName = aTokenizer.current().data;
			Database* inUseDB = process->getActiveDatabase();
			StatusResult aResult = aHelper.findTable(inUseDB, tableName);
			if (!aResult)return StatusResult(Errors::unknownTable);
			if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().keyword == Keywords::set_kw) {
				std::map<std::string, std::string> updateList;
				while (aTokenizer.next() && aTokenizer.more()) {
					std::string updateAttr = aTokenizer.current().data;
					StatusResult anotherResult = aHelper.findAttr(inUseDB, updateAttr, aResult.value);
					if (!anotherResult)return StatusResult(Errors::unknownAttribute);
					if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().data == "=") {
						if (aTokenizer.next() && aTokenizer.more()) {
							std::string updateValue;
							if (aTokenizer.current().data == "'" && aTokenizer.next() && aTokenizer.more()) {
								while (aTokenizer.current().data != "'" && aTokenizer.hasnext()) {
									updateValue = updateValue + " " + aTokenizer.current().data;
									aTokenizer.next();
								}
								if (aTokenizer.current().data != "'")return StatusResult(Errors::unknownCommand);
							}
							else if(aTokenizer.more()){
								updateValue = aTokenizer.current().data;
							}
							else return StatusResult(Errors::unknownCommand);
							updateList.insert({ updateAttr, updateValue });
							if (aTokenizer.next() && aTokenizer.current().data != ",")break;
						}
						else return StatusResult(Errors::unknownCommand);
					}
					else return StatusResult(Errors::unknownCommand);
				}
				Filters aFilter;
				if (aTokenizer.more() && aTokenizer.current().keyword == Keywords::where_kw) {
					bool isAnd = true;
					while (aTokenizer.next() && aTokenizer.more()) {
						std::string attr = aTokenizer.current().data;
						Operand lo(attr, TokenType::identifier);
						if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().type == TokenType::operators) {
							Operators op = aTokenizer.current().op;
							if (aTokenizer.next() && aTokenizer.more()) {
								std::string val;
								if (aTokenizer.current().data == "'" && aTokenizer.next() && aTokenizer.more()) {
									while (aTokenizer.current().data != "'" && aTokenizer.hasnext()) {
										val = val + aTokenizer.current().data + " ";
										aTokenizer.next();
									}
									if (aTokenizer.current().data != "'")return StatusResult(Errors::unknownCommand);
								}
								else if(aTokenizer.more()){
									val = aTokenizer.current().data;
								}
								else return StatusResult(Errors::unknownCommand);
								Operand ro(val, TokenType::string);
								Expression aEx(lo, op, ro);
								isAnd ? aFilter.addAnd(aEx):aFilter.addOr(aEx);
							}
						}
						else return StatusResult(Errors::unknownCommand);
						if (aTokenizer.next() && aTokenizer.more()) {
							if (aTokenizer.current().keyword == Keywords::or_kw) {
								isAnd = false;
							}
							else if (aTokenizer.current().keyword == Keywords::and_kw) {
								isAnd = true;
							}
							else return StatusResult(Errors::unknownCommand);
						}
					}
				}
				while (aTokenizer.next());
				return process->updateData(updateList, aFilter, aResult.value);
			}
		}
		return StatusResult(Errors::unknownCommand);
	}

	StatusResult UpdateStatement::run(std::ostream& aStream, std::string aName) const {
		return StatusResult();
	}

	ShowIndexStatement::ShowIndexStatement() : Statement(Keywords::index_kw) {}
	ShowIndexStatement::ShowIndexStatement(SQLProcessor* aProcess) : Statement(Keywords::index_kw), process(aProcess) {};
	ShowIndexStatement::ShowIndexStatement(ShowIndexStatement& aCopy) : Statement(Keywords::index_kw), process(aCopy.process){}
	StatusResult ShowIndexStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase()) { 
			return StatusResult(Errors::noDatabaseSpecified); 
		}
		if (aTokenizer.size()>3) {
			return StatusResult(Errors::unknownCommand);
		}
		else {
			process->showIndex();
			while (aTokenizer.next());
			return StatusResult();
		}
	}
	StatusResult ShowIndexStatement::run(std::ostream& aStream, std::string aName) const {
		return StatusResult();
	}

	AlterStatement::AlterStatement():Statement(Keywords::alter_kw) {}
	AlterStatement::AlterStatement(SQLProcessor* aProcess): Statement(Keywords::alter_kw), process(aProcess) {}
	AlterStatement::AlterStatement(AlterStatement& aCopy): Statement(Keywords::alter_kw), process(aCopy.process) {}
	StatusResult AlterStatement::parse(Tokenizer& aTokenizer) {
		if (!process->getActiveDatabase())return StatusResult(Errors::noDatabaseSpecified);
		TableHelper aHelper;
		if (aTokenizer.next() && aTokenizer.more()&&aTokenizer.current().keyword == Keywords::table_kw) {
			if (aTokenizer.next() && aTokenizer.more()) {
				std::string tableName = aTokenizer.current().data;
				Database* inUseDB = process->getActiveDatabase();
				StatusResult aResult = aHelper.findTable(inUseDB, tableName);
				if (!aResult)return StatusResult(Errors::unknownTable);
				if (aTokenizer.next() && aTokenizer.more() && aTokenizer.current().type == TokenType::keyword) {
					Keywords altOperation = aTokenizer.current().keyword;
					if (aTokenizer.next() && aTokenizer.more()) {
						std::string fieldName;
						std::string attrType;
						switch (altOperation)
						{
						case Keywords::add_kw:
							fieldName = aTokenizer.current().data;
							if (aTokenizer.next() && aTokenizer.more()) {
								attrType = aTokenizer.current().data;
								if (attrType == "varchar" && aTokenizer.remaining() >= 4) {
									int i = 3;
									while (i > 0) {
										aTokenizer.next();
										attrType = attrType + aTokenizer.current().data;
										i--;
									}
								}
								while (aTokenizer.next());
								return process->addField(fieldName, attrType, aResult.value);
							}
							else return StatusResult(Errors::unknownCommand);
						default:
							break;
						}
					}
				}
			}
		}
		return StatusResult(Errors::unknownCommand);
	}
	StatusResult AlterStatement::run(std::ostream& aStream, std::string aName) const {
		return StatusResult();
	}
}


