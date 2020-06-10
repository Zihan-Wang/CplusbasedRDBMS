//
//  Helpers.hpp
//  Database3
//
//  Created by rick gessner on 4/14/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Helpers_h
#define Helpers_h

#include "keywords.hpp"
#include <algorithm>
#include "Database.hpp"
#include <sstream>
#include <set>
#include "Filters.hpp"
#include "Row.hpp"
#include "Timer.hpp"
namespace ECE141 {

  template<typename T, size_t aSize>
  bool in_array(T (&anArray)[aSize], const T &aValue) {
    return std::find(std::begin(anArray), std::end(anArray), aValue);
  };
  

  static std::map<std::string, Operators> gOperators = {
    std::make_pair(".", Operators::dot_op),
    std::make_pair("+", Operators::add_op),
    std::make_pair("-", Operators::subtract_op),
    std::make_pair("*", Operators::multiply_op),
    std::make_pair("/", Operators::divide_op),
    std::make_pair("^", Operators::power_op),
    std::make_pair("%", Operators::mod_op),
    std::make_pair("=", Operators::equal_op),
    std::make_pair("!=", Operators::notequal_op),
    std::make_pair("not", Operators::notequal_op),
    std::make_pair("<", Operators::lt_op),
    std::make_pair("<=", Operators::lte_op),
    std::make_pair(">", Operators::gt_op),
    std::make_pair(">=", Operators::gte_op),
    std::make_pair("and", Operators::and_op),
    std::make_pair("or", Operators::or_op),
    std::make_pair("nor", Operators::nor_op),
    std::make_pair("between", Operators::between_op),
  };
  
  static ECE141::Keywords gJoinTypes[]={
    ECE141::Keywords::cross_kw,ECE141::Keywords::full_kw, ECE141::Keywords::inner_kw,
    ECE141::Keywords::left_kw, ECE141::Keywords::right_kw
  };
  
  //a list of known functions...
  static std::map<std::string,int> gFunctions = {
    std::make_pair("avg", 10),
    std::make_pair("count", 20),
    std::make_pair("max", 30),
    std::make_pair("min", 40)
  };
  
  
  //This map binds a keyword string with a Keyword (token)...
  static std::map<std::string,  Keywords> gDictionary = {
    std::make_pair("add",       Keywords::add_kw),
    std::make_pair("all",       Keywords::all_kw),
    std::make_pair("alter",     Keywords::alter_kw),
    std::make_pair("and",       Keywords::and_kw),
    std::make_pair("as",        Keywords::as_kw),
    std::make_pair("avg",       ECE141::Keywords::avg_kw),
    std::make_pair("auto_increment", Keywords::auto_increment_kw),
    std::make_pair("between",   ECE141::Keywords::between_kw),
    std::make_pair("boolean",   ECE141::Keywords::boolean_kw),
    std::make_pair("by",        ECE141::Keywords::by_kw),
    std::make_pair("char",      ECE141::Keywords::char_kw),
    std::make_pair("column",    ECE141::Keywords::column_kw),
    std::make_pair("count",     ECE141::Keywords::count_kw),
    std::make_pair("create",    ECE141::Keywords::create_kw),
    std::make_pair("cross",     ECE141::Keywords::cross_kw),
    std::make_pair("database",  ECE141::Keywords::database_kw),
    std::make_pair("databases", ECE141::Keywords::databases_kw),
    std::make_pair("datetime",  ECE141::Keywords::datetime_kw),
    std::make_pair("decimal",   ECE141::Keywords::decimal_kw),
    std::make_pair("default",   ECE141::Keywords::default_kw),
    std::make_pair("delete",    ECE141::Keywords::delete_kw),
    std::make_pair("describe",  ECE141::Keywords::describe_kw),
    std::make_pair("distinct",  ECE141::Keywords::distinct_kw),
    std::make_pair("double",    ECE141::Keywords::double_kw),
    std::make_pair("drop",      ECE141::Keywords::drop_kw),
    std::make_pair("explain",   ECE141::Keywords::explain_kw),
    std::make_pair("float",     ECE141::Keywords::float_kw),
    std::make_pair("foreign",   ECE141::Keywords::foreign_kw),
    std::make_pair("from",      ECE141::Keywords::from_kw),
    std::make_pair("full",      ECE141::Keywords::full_kw),
    std::make_pair("group",     ECE141::Keywords::group_kw),
    std::make_pair("help",      ECE141::Keywords::help_kw),
    std::make_pair("in",        ECE141::Keywords::in_kw),
    std::make_pair("indexes",   ECE141::Keywords::index_kw),
    std::make_pair("inner",     ECE141::Keywords::inner_kw),
    std::make_pair("insert",    ECE141::Keywords::insert_kw),
    std::make_pair("int",       ECE141::Keywords::integer_kw),
    std::make_pair("integer",   ECE141::Keywords::integer_kw),
    std::make_pair("into",      ECE141::Keywords::into_kw),
    std::make_pair("join",      ECE141::Keywords::join_kw),
    std::make_pair("key",       ECE141::Keywords::key_kw),
    std::make_pair("left",      ECE141::Keywords::left_kw),
    std::make_pair("limit",      ECE141::Keywords::limit_kw),
    std::make_pair("max",       ECE141::Keywords::max_kw),
    std::make_pair("min",       ECE141::Keywords::min_kw),
    std::make_pair("modify",    ECE141::Keywords::modify_kw),
    std::make_pair("not",       ECE141::Keywords::not_kw),
    std::make_pair("null",      ECE141::Keywords::null_kw),
    std::make_pair("on",        ECE141::Keywords::on_kw),
    std::make_pair("or",        ECE141::Keywords::or_kw),
    std::make_pair("order",     ECE141::Keywords::order_kw),
    std::make_pair("outer",     ECE141::Keywords::outer_kw),
    std::make_pair("primary",   ECE141::Keywords::primary_kw),
    std::make_pair("quit",      ECE141::Keywords::quit_kw),
    std::make_pair("references", ECE141::Keywords::references_kw),
    std::make_pair("right",     ECE141::Keywords::right_kw),
    std::make_pair("select",    ECE141::Keywords::select_kw),
    std::make_pair("self",      ECE141::Keywords::self_kw),
    std::make_pair("set",       ECE141::Keywords::set_kw),
    std::make_pair("show",      ECE141::Keywords::show_kw),
    std::make_pair("sum",       ECE141::Keywords::sum_kw),
    std::make_pair("timestamp",     ECE141::Keywords::timestamp_kw),
    std::make_pair("table",     ECE141::Keywords::table_kw),
    std::make_pair("tables",    ECE141::Keywords::tables_kw),
    std::make_pair("unique",    ECE141::Keywords::unique_kw),
    std::make_pair("update",    ECE141::Keywords::update_kw),
    std::make_pair("use",       ECE141::Keywords::use_kw),
    std::make_pair("values",    ECE141::Keywords::values_kw),
    std::make_pair("varchar",   ECE141::Keywords::varchar_kw),
    std::make_pair("version",   ECE141::Keywords::version_kw),
    std::make_pair("where",     ECE141::Keywords::where_kw)
  };
  
  
  class Helpers {
  public:
    Helpers() {}

    static Keywords getKeywordId(const std::string aKeyword) {
      auto theIter = gDictionary.find(aKeyword);
      if (theIter != gDictionary.end()) {
        return theIter->second;
      }
      return Keywords::unknown_kw;
    }
    
    //convert from char to keyword...
    static Keywords charToKeyword(char aChar) {
      switch(toupper(aChar)) {
        case 'I': return Keywords::integer_kw;
        case 'T': return Keywords::datetime_kw;
        case 'B': return Keywords::boolean_kw;
        case 'F': return Keywords::float_kw;
        case 'V': return Keywords::varchar_kw;
        default:  return Keywords::unknown_kw;
      }
    }
        
    static const char* keywordToString(Keywords aType) {
      switch(aType) {
        case Keywords::integer_kw:  return "integer";
        case Keywords::float_kw:    return "float";
        case Keywords::boolean_kw:  return "bool";
        case Keywords::datetime_kw: return "datetime";
        case Keywords::varchar_kw:  return "varchar";
        default:                    return "unknown";
      }
    }
  
    // USE: ---validate that given keyword is a datatype...
    static bool isDatatype(Keywords aKeyword) {
      switch(aKeyword) {
        case Keywords::char_kw:
        case Keywords::datetime_kw:
        case Keywords::float_kw:
        case Keywords::integer_kw:
        case Keywords::varchar_kw:
          return true;
        default: return false;
      }
    }
    
    static Operators toOperator(std::string aString) {
      auto theIter = gOperators.find(aString);
      if (theIter != gOperators.end()) {
        return theIter->second;
      }
      return Operators::unknown_op;
    }
    
    static int getFunctionId(const std::string anIdentifier) {
      auto theIter = gFunctions.find(anIdentifier);
      if (theIter != gFunctions.end()) {
        return theIter->second;
      }
      return 0;
    }
    
    static bool isNumericKeyword(Keywords aKeyword) {
      static Keywords theTypes[]={Keywords::decimal_kw, Keywords::double_kw, Keywords::float_kw, Keywords::integer_kw};
      for(auto k : theTypes) {
        if(aKeyword==k) return true;
      }
      return false;
    }
  };
  
  class TableHelper {
  public:
      TableHelper() {}
      TableHelper(TableHelper &aCopy) {}
      StatusResult findTable(Database* inUseDB, std::string tablename) {
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

      StatusResult findAttr(Database* inUseDB, std::string iden, int BlockNum) {
          inUseDB->getStorage().stream.clear(std::ios::goodbit);
          inUseDB->getStorage().stream.seekg(std::ios::beg);
          int i = BlockNum-1;
          while (i != 0) {
              std::string str;
              std::getline(inUseDB->getStorage().stream, str);
              --i;
          }
          std::set<std::string> attList;
          std::string entity;
          std::getline(inUseDB->getStorage().stream, entity);
          int dataStartPos = entity.find_first_of("|") + 1;
          int length = entity.find_last_of("]") - dataStartPos;
          entity = entity.substr(dataStartPos, length);
          const char* d = ",";
          char* p;
          p = strtok((char*)entity.c_str(), d);
          while (p)
          {
              std::stringstream ss(p);
              std::string attr;
              ss >> attr;
              if (attr == "")break;
              attList.insert(attr);
              p = strtok(NULL, d);
          }
          if (attList.find(iden) == attList.end())return StatusResult(Errors::unknownAttribute);
          return StatusResult();
      }

      StatusResult findData(Database* inUseDB, std::set<std::string> dataNameList, int BlockNum, std::map<std::string, std::string>& dataTypePairs) {
          inUseDB->getStorage().stream.clear(std::ios::goodbit);
          inUseDB->getStorage().stream.seekg(std::ios::beg);
          int i = BlockNum-1;
          while (i != 0) {
              std::string str;
              std::getline(inUseDB->getStorage().stream, str);
              --i;
          }
          std::string entity;
          std::getline(inUseDB->getStorage().stream, entity);
          int dataStartPos = entity.find_first_of("|") + 1;
          int length = entity.find_last_of("]") - dataStartPos;
          entity = entity.substr(dataStartPos, length);
          const char* d = ",";
          char* p;
          p = strtok((char*)entity.c_str(), d);
          while (p)
          {
              std::stringstream ss(p);
              std::string dataName;
              ss >> dataName;
              if (dataName == "")break;
              std::string type;
              for (int i = 5; i > 0; i--) {
                  type="";
                  ss >> type;
              }
              dataTypePairs.insert({ dataName, type });
              p = strtok(NULL, d); 
          }
          for (auto i = dataNameList.begin(); i != dataNameList.end(); ++i) {
              if(dataTypePairs.find(*i) == dataTypePairs.end())return StatusResult(Errors::unknownAttribute);
          }
          return StatusResult();
      }

      void TableView(std::vector<Row> RowCollection, std::set<std::string> dataNameList, Timer aTimer) {
          int row = 0;
          if (dataNameList.find("*") == dataNameList.end()) {
              for (auto it = RowCollection.begin(); it != RowCollection.end(); ++it) {
                  for (auto i = dataNameList.begin(); i != dataNameList.end(); ++i) {
                      std::cout <<"  " << it->data[*i] << "  ";
                  }
                  std::cout << "\n";
                  ++row;
              }
              std::cout << row << " rows in set" << "\n";
          }
          else {
              for (auto it = RowCollection.begin(); it != RowCollection.end(); ++it) {
                  for (auto m = (*it).data.begin(); m != (*it).data.end(); ++m) {
                      std::cout << "  " << m->second << "  ";
                  }
                  std::cout << "\n";
                  ++row;
              }
              std::cout << row << " rows in set (" <<aTimer.elapsed()<< " ms. )\n";
          }
      }

      void writeInto(Row aRow, int BlockNum, Database* inUseDB) {
          StorageBlock aBlock;
          std::string data;
          for (auto i = aRow.data.begin(); i != aRow.data.end(); ++i) {
              data = data + i->first + " " + i->second + ",";
          }
          std::memcpy(aBlock.data, data.c_str(), data.size());
          inUseDB->getStorage().writeBlock(aBlock, BlockNum);
      }
  };
  
}
#endif /* Helpers_h */
