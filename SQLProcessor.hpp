//
//  SQLProcessor.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Schema.hpp"
#include <iostream>
#include "Statement.hpp"
#include "Filters.hpp"
#include <set>
class Statement;
class Database; //define this later...

namespace ECE141 {

  class SQLProcessor : public CommandProcessor {
  public:
    
    SQLProcessor(CommandProcessor *aNext=nullptr);
    virtual ~SQLProcessor();
    virtual StatusResult processInput(Tokenizer& aTokenizer);
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);
    virtual StatusResult  interpreter(Statement& aStatement, Tokenizer& aTokenizer);
    virtual Database* getActiveDatabase()override;
    StatusResult createTable(const Schema &aSchema);
    StatusResult dropTable(const std::string &aName);
    StatusResult describeTable(const std::string &aName) const;
    StatusResult showTables() const;
    StatusResult showIndex() const;
    StatusResult insert(uint32_t blocknum, std::string data);
    StatusResult deleteData(int blockNum);
    StatusResult seleteData(int indBlockNum, Filters aFilter, std::set<std::string> dataNameList);
    StatusResult seleteJoinData(int mainIndBlockNum, int IndBlockNum, std::string field1, std::string field2, std::set<std::string> dataNameList, bool isleft);
    StatusResult updateData(std::map<std::string, std::string> updateList, Filters aFilter, int tableBlockNum);
    StatusResult addField(std::string fieldName, std::string fieldType, int tableBlockNum);
    std::vector<Row> getRowCollection(Database* inUseDB, int IndBlockNum);
    friend class UseStatement;
/*  do these in next assignment
    StatusResult insert();
    StatusResult update();
    StatusResult delete();
*/
    
  protected:
    
    //do you need other data members?
    Database* inUseDB;
  };

}
#endif /* SQLProcessor_hpp */
