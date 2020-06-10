//
//  SQLProcessor.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//
#include <iostream>
#include <sstream>
#include <string>
#include "SQLProcessor.hpp"
#include "dbstatement.hpp"
#include "Database.hpp"
#include "StorageBlock.hpp"
#include "Attribute.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include "Value.hpp"
#include "Helpers.hpp"
#include "Row.hpp"
#include "Timer.hpp"
namespace ECE141 {

 //STUDENT: Implement the SQLProcessor class here...
    SQLProcessor::SQLProcessor(CommandProcessor* aNext) : CommandProcessor(aNext), inUseDB(nullptr) {}
    SQLProcessor::~SQLProcessor() { delete inUseDB; }
    StatusResult SQLProcessor::processInput(Tokenizer& aTokenizer) {
        std::unique_ptr<Statement> aStatement(getStatement(aTokenizer)); // smart pointer
        if (aStatement) {
            StatusResult aResult = interpreter(*aStatement, aTokenizer);
            return aResult;
        }
        else if (next) {
            return next->processInput(aTokenizer);
        }
        return StatusResult{ ECE141::unknownCommand };
    }

    Statement* SQLProcessor::getStatement(Tokenizer& aTokenizer) {
        Token curToken = aTokenizer.current();
        if (!aTokenizer.hasnext())return nullptr;
        Token nextToken = aTokenizer.peek(1);
        switch (curToken.keyword) {
        case Keywords::create_kw:
            if (nextToken.keyword != Keywords::table_kw)break;
            return new CreateTableStatement(this);
            break;
        case Keywords::drop_kw:
            if (nextToken.keyword != Keywords::table_kw)break;
            return new DropTableStatement(this);
            break;
        case Keywords::show_kw:
            if (nextToken.keyword == Keywords::tables_kw) {
                return new ShowTableStatement(this);
            }
            else if (nextToken.keyword == Keywords::index_kw) {
                return new ShowIndexStatement(this);
            }
            break;
        case Keywords::describe_kw:
            return new DescribeTableStatement(this);
            break;
        case Keywords::use_kw:
            return new UseStatement(this);
            break;
        case Keywords::insert_kw:
            return new InsertStatement(this);
            break;
        case Keywords::delete_kw:
            return new DeleteStatement(this);
            break;
        case Keywords::select_kw:
            return new SelectStatement(this);
            break;
        case Keywords::update_kw:
            return new UpdateStatement(this);
            break;
        case Keywords::alter_kw:
            return new AlterStatement(this);
            break;
        default:
            return nullptr;
        };
        return nullptr;
    }

    StatusResult SQLProcessor::interpreter(Statement& aStatement, Tokenizer& aTokenizer) {       
        ECE141::StatusResult aResult = aStatement.parse(aTokenizer);
        return aResult;
    }

    StatusResult SQLProcessor::interpret(const Statement& aStatement) {
        return StatusResult();
    }

    Database* SQLProcessor::getActiveDatabase() {
        return inUseDB;
    }

    std::map<DataType, std::string> data2string{
        {DataType::no_type, "no"},
        {DataType::bool_type, "bool"},
        {DataType::datetime_type, "datatime"},
        {DataType::timestamp_type, "timestamp"},
        {DataType::float_type, "float"},
        {DataType::int_type, "integer"},
        {DataType::varchar_type, "varchar"}
    };


    StatusResult SQLProcessor::createTable(const Schema& aSchema) {
        StorageBlock aBlock(BlockType::entity_block);
        int bn;
        std::vector<std::pair<std::string, int>> tablelist;
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
            tablelist.push_back(std::pair<std::string, int>(table, bn));
        }
        for (auto it = tablelist.begin(); it != tablelist.end(); ++it) {
            if(it->first == aSchema.getName())return StatusResult(Errors::tableExists);
        }
        inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        std::vector<Attribute> list = aSchema.getAttributes();
        std::string data;
        for (auto it = list.begin(); it != list.end(); ++it) {
            data = data + it->getName() + " ";
            if(it->getType() == DataType::varchar_type)data = data + data2string[it->getType()] + "(" + std::to_string(it->getSize()) + ") ";
            else data = data + data2string[it->getType()] + " ";
            it->getNullable() ? data = data + "YES " : data = data + "NO ";
            it->getPrimaryKey() ? data = data + "YES " : data = data + "NO ";
            data = data + it->getDefaultValue() + " ";
            it->getAuto_Increment() ? data = data + "auto_increment " : data;
            it->getPrimaryKey() ? data = data + "primary key " : data;
            data = data + ",";
        }
        std::memcpy(aBlock.data, data.c_str(), data.size());
        int bNum = inUseDB->getStorage().getTotalBlockCount()+1;
        inUseDB->getStorage().writeBlock(aBlock, bNum);
        std::vector attributes = aSchema.getAttributes();
        StorageBlock indexBlock(BlockType::index_block);
        inUseDB->getStorage().writeBlock(indexBlock, bNum + 1);
        inUseDB->getStorage().anotherstream.seekp(0, std::ios_base::end);
        inUseDB->getStorage().anotherstream << aSchema.getName() <<" "<<bNum<< "\n";
        inUseDB->getStorage().update();
        return StatusResult(); 
    }

    StatusResult SQLProcessor::dropTable(const std::string& aName) {
        std::vector<std::pair<std::string, int>> tablelist;
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        int start = inUseDB->getStorage().anotherstream.tellg(), end = 0;
        while (!inUseDB->getStorage().anotherstream.eof()) {
            std::string tables, table, bN;
            std::getline(inUseDB->getStorage().anotherstream, tables);
            std::stringstream aStringstream;
            if (tables == "")break;
            aStringstream << tables;
            aStringstream >> table;
            aStringstream >> bN;
            if (table == aName) { 
                end = inUseDB->getStorage().anotherstream.tellg();
                break;
            }
            start = inUseDB->getStorage().anotherstream.tellg();
        }
        inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        if((end-start)<=0)return StatusResult(Errors::unknownTable);
        else {
            inUseDB->getStorage().anotherstream.seekp(start);
            int i = end - start;
            while (i != 0) {
                inUseDB->getStorage().anotherstream << " ";
                --i;
            }
            inUseDB->getStorage().update();
            inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
            inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
            return StatusResult();
        }
        inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        return StatusResult(Errors::unknownTable);
    }

    void parsetab(std::string tab) {
        int count = 0;
        size_t posOfDataStart = tab.find_first_of('|') + 1;
        size_t length = tab.find_last_of(',') - posOfDataStart;
        std::string str = tab.substr(posOfDataStart, length);
        const char* d = ",";
        char* p;
        p = strtok((char*)str.c_str(), d);
        while (p)
        {
            std::cout << p << "\n";
            ++count;
            p = strtok(NULL, d);
        }
        std::cout << count << " rows in set" << "\n";
    }

    StatusResult SQLProcessor::describeTable(const std::string& aName) const {
        StorageBlock aBlock(BlockType::entity_block);
        int blockNum;
        std::string tab;
        int bn;
        std::vector<std::pair<std::string, int>> tablelist;
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        while (!inUseDB->getStorage().anotherstream.eof()) {
            std::string tables, table, bN;
            std::getline(inUseDB->getStorage().anotherstream, tables);
            std::stringstream aStringstream;
            aStringstream << tables;
            aStringstream >> table;
            if (table == "")break;
            aStringstream >> bN;
            bn = std::stoi(bN);
            tablelist.push_back(std::pair<std::string, int>(table, bn));
        }
        inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        for (auto it = tablelist.begin(); it != tablelist.end(); ++it) {
            if (it->first == aName) {
                blockNum = it->second;
                inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
                for (int i = 0; i < blockNum;++i) {
                    std::getline(inUseDB->getStorage().stream, tab);
                }
                parsetab(tab);
                return StatusResult();
            };
        }
        return StatusResult(Errors::unknownTable); 
    }

    StatusResult SQLProcessor::showTables() const { 
        int count = 0;
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        while (!inUseDB->getStorage().anotherstream.eof()) {
            std::string tables, table, bN;
            std::getline(inUseDB->getStorage().anotherstream, tables);
            if (tables == "")break;
            std::stringstream aStringstream;
            aStringstream << tables;
            aStringstream >> table;
            aStringstream >> bN;
            std::cout << table << "\n";
            ++count;
        }
        std::cout << count << " rows in set" << "\n";
        inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        return StatusResult(); 
    }

    StatusResult SQLProcessor::insert(uint32_t blocknum, std::string data) {
        StorageBlock aBlock(BlockType::data_block);
        std::memcpy(aBlock.data, data.c_str(), data.size());
        inUseDB->getStorage().writeBlock(aBlock, blocknum);
        return StatusResult();
    }

    StatusResult SQLProcessor::deleteData(int blockNum) {
        int indexBN = blockNum + 1;
        StorageBlock aBlock(BlockType::index_block);
        inUseDB->getStorage().writeBlock(aBlock, indexBN);
        return StatusResult();
    }

    StatusResult SQLProcessor::seleteData(int indBlockNum, Filters aFilter, std::set<std::string> dataNameList) {
        Timer aTimer;
        aTimer.start();
        TableHelper aHelp;
        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        int i = indBlockNum-1;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string attrBlock;
        std::getline(inUseDB->getStorage().stream, attrBlock);
        int attStartPos = attrBlock.find_first_of("|") + 1;
        int len = attrBlock.find_last_of("]") - attStartPos;
        std::string att = attrBlock.substr(attStartPos, len);
        std::map<std::string, int> auto_I;
        const char* d = ",";
        char* p;
        p = strtok((char*)att.c_str(), d);
        while (p)
        {
            std::stringstream ss(p);
            std::string dN;
            ss >> dN;
            std::string check;
            for (int i = 5; i > 0; i--) {
                check = "";
                ss >> check;
            }
            if (check != "")auto_I.insert({ dN,0 });
            p = strtok(NULL, d);
        }



        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        std::vector<Row> RowCollection;
        i = indBlockNum;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string blockString;
        std::getline(inUseDB->getStorage().stream, blockString);
        int dataStartPos = blockString.find_first_of("|")+1;
        int length = blockString.find_last_of("]")-dataStartPos;
        std::string dataInd = blockString.substr(dataStartPos, length);
        std::vector<int> indList;
        p = strtok((char*)dataInd.c_str(), d);
        while (p)
        {
            std::stringstream ss(p);
            std::string indNum;
            ss >> indNum;
            if (indNum == "")break;
            indList.push_back(std::stoi(indNum));
            p = strtok(NULL, d);
        }
        for (auto i = indList.begin(); i != indList.end(); ++i) {
            std::string dataString;
            inUseDB->getStorage().stream.clear(std::ios::goodbit);
            inUseDB->getStorage().stream.seekg(std::ios::beg);
            int in = *i - 1;
            while (in != 0) {
                std::string str;
                std::getline(inUseDB->getStorage().stream, str);
                --in;
            }
            std::getline(inUseDB->getStorage().stream, dataString);
            int dataStartPos = dataString.find_first_of("|") + 1;
            int length = dataString.find_last_of("]") - dataStartPos;
           dataString = dataString.substr(dataStartPos, length);
            p = strtok((char*)dataString.c_str(), d);
            Row aRow;
            while (p)
            {
                std::stringstream ss(p);
                std::string dataKey;
                std::string dataValue;
                ss >> dataKey;
                if (dataKey == "")break;
                ss >> dataValue;
                if (auto_I.find(dataKey) != auto_I.end() && dataValue == "NULL") {
                    dataValue = std::to_string(++auto_I[dataKey]);
                }
                aRow.data.insert({dataKey, dataValue});
                p = strtok(NULL, d);
            }
            RowCollection.push_back(aRow);
        }
        std::vector<Row> anewRowCollection = aFilter.filterRows(RowCollection);
        aTimer.stop();
        aHelp.TableView(anewRowCollection, dataNameList, aTimer);
        return StatusResult();
    }

    std::vector<Row> SQLProcessor::getRowCollection(Database* inUseDB, int indBlockNum) {
        TableHelper aHelp;
        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        int i = indBlockNum - 1;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string attrBlock;
        std::getline(inUseDB->getStorage().stream, attrBlock);
        int attStartPos = attrBlock.find_first_of("|") + 1;
        int len = attrBlock.find_last_of("]") - attStartPos;
        std::string att = attrBlock.substr(attStartPos, len);
        std::map<std::string, int> auto_I;
        const char* d = ",";
        char* p;
        p = strtok((char*)att.c_str(), d);
        while (p)
        {
            std::stringstream ss(p);
            std::string dN;
            ss >> dN;
            std::string check;
            for (int i = 5; i > 0; i--) {
                check = "";
                ss >> check;
            }
            if (check != "")auto_I.insert({ dN,0 });
            p = strtok(NULL, d);
        }



        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        std::vector<Row> RowCollection;
        i = indBlockNum;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string blockString;
        std::getline(inUseDB->getStorage().stream, blockString);
        int dataStartPos = blockString.find_first_of("|") + 1;
        int length = blockString.find_last_of("]") - dataStartPos;
        std::string dataInd = blockString.substr(dataStartPos, length);
        std::vector<int> indList;
        p = strtok((char*)dataInd.c_str(), d);
        while (p)
        {
            std::stringstream ss(p);
            std::string indNum;
            ss >> indNum;
            if (indNum == "")break;
            indList.push_back(std::stoi(indNum));
            p = strtok(NULL, d);
        }
        for (auto i = indList.begin(); i != indList.end(); ++i) {
            std::string dataString;
            inUseDB->getStorage().stream.clear(std::ios::goodbit);
            inUseDB->getStorage().stream.seekg(std::ios::beg);
            int in = *i - 1;
            while (in != 0) {
                std::string str;
                std::getline(inUseDB->getStorage().stream, str);
                --in;
            }
            std::getline(inUseDB->getStorage().stream, dataString);
            int dataStartPos = dataString.find_first_of("|") + 1;
            int length = dataString.find_last_of("]") - dataStartPos;
            dataString = dataString.substr(dataStartPos, length);
            p = strtok((char*)dataString.c_str(), d);
            Row aRow;
            while (p)
            {
                std::stringstream ss(p);
                std::string dataKey;
                std::string dataValue;
                ss >> dataKey;
                if (dataKey == "")break;
                ss >> dataValue;
                if (auto_I.find(dataKey) != auto_I.end() && dataValue == "NULL") {
                    dataValue = std::to_string(++auto_I[dataKey]);
                }
                aRow.data.insert({ dataKey, dataValue });
                p = strtok(NULL, d);
            }
            RowCollection.push_back(aRow);
        }
        return RowCollection;
    }

    Row mergeRow(Row row1, Row row2) {
        Row aRow;
        for (auto i = row1.data.begin(); i != row1.data.end(); ++i) {
            aRow.data[i->first] = i->second;
        }
        for (auto i = row2.data.begin(); i != row2.data.end(); ++i) {
            aRow.data[i->first] = i->second;
        }
        return aRow;
    }

    StatusResult SQLProcessor::seleteJoinData(int mainIndBlockNum, int IndBlockNum, std::string field1, std::string field2, std::set<std::string> dataNameList, bool isleft) {
        TableHelper aHelp;
        Timer aTimer;
        aTimer.start();
        std::vector<Row> mainRowCollection;
        std::vector<Row> RowCollection;
        std::string mainField;
        std::string Field;
        if (isleft) {
            mainRowCollection = getRowCollection(inUseDB, mainIndBlockNum);
            RowCollection = getRowCollection(inUseDB, IndBlockNum);
            mainField = field1;
            Field = field2;
        }
        else {
            mainRowCollection = getRowCollection(inUseDB, IndBlockNum);
            RowCollection = getRowCollection(inUseDB, mainIndBlockNum);
            mainField = field2;
            Field = field1;
        }
        std::vector<Row> joinRowCollection;
        Row nullRow(RowCollection[0]);
        for (auto i = nullRow.data.begin(); i != nullRow.data.end(); ++i) {
            i->second = "Null";
        }
        for (auto i = mainRowCollection.begin(); i != mainRowCollection.end(); ++i) {
            std::string mainFieldValue = i->data[mainField];
            Row joinRow(nullRow);
            for (auto in = RowCollection.begin(); in != RowCollection.end(); ++in) {
                if (in->data[Field] == mainFieldValue) {
                    joinRow = *in;
                    break;
                }
            }
            joinRowCollection.push_back(mergeRow(*i, joinRow));
        }
        aTimer.stop();
        aHelp.TableView(joinRowCollection, dataNameList, aTimer);
        return StatusResult();
    }

    StatusResult SQLProcessor::updateData(std::map<std::string, std::string> updateData, Filters aFilter, int tableBlockNum) {
        TableHelper aHelp;
        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        int i = tableBlockNum-1;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string attrBlock;
        std::getline(inUseDB->getStorage().stream, attrBlock);
        int attStartPos = attrBlock.find_first_of("|") + 1;
        int len = attrBlock.find_last_of("]") - attStartPos;
        std::string att = attrBlock.substr(attStartPos, len);
        std::vector<std::string> attList;
        const char* d = ",";
        char* p;
        p = strtok((char*)att.c_str(), d);
        while (p)
        {
            std::stringstream ss(p);
            std::string dN;
            ss >> dN;
            if (dN== "")break;
            p = strtok(NULL, d);
        }
        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        std::map<int, Row> RowCollection;
        i = tableBlockNum;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string blockString;
        std::getline(inUseDB->getStorage().stream, blockString);
        int dataStartPos = blockString.find_first_of("|") + 1;
        int length = blockString.find_last_of("]") - dataStartPos;
        std::string dataInd = blockString.substr(dataStartPos, length);
        std::vector<int> indList;
        p = strtok((char*)dataInd.c_str(), d);
        while (p)
        {
            std::stringstream ss(p);
            std::string indNum;
            ss >> indNum;
            if (indNum == "")break;
            indList.push_back(std::stoi(indNum));
            p = strtok(NULL, d);
        }
        for (auto i = indList.begin(); i != indList.end(); ++i) {
            std::string dataString;
            inUseDB->getStorage().stream.clear(std::ios::goodbit);
            inUseDB->getStorage().stream.seekg(std::ios::beg);
            int in = *i - 1;
            while (in != 0) {
                std::string str;
                std::getline(inUseDB->getStorage().stream, str);
                --in;
            }
            std::getline(inUseDB->getStorage().stream, dataString);
            int dataStartPos = dataString.find_first_of("|") + 1;
            int length = dataString.find_last_of("]") - dataStartPos;
            dataString = dataString.substr(dataStartPos, length);
            p = strtok((char*)dataString.c_str(), d);
            Row aRow;
            while (p)
            {
                std::stringstream ss(p);
                std::string dataKey;
                std::string dataValue;
                ss >> dataKey;
                if (dataKey == "")break;
                ss >> dataValue;
                aRow.data.insert({ dataKey, dataValue });
                p = strtok(NULL, d);
            }
            RowCollection.insert({ *i, aRow});
        }
        std::map<int, Row> anewRowCollection = aFilter.filterRowCollection(RowCollection, updateData);
        for (auto i = anewRowCollection.begin(); i != anewRowCollection.end(); ++i) {
            int blockNum = i->first;
            Row aRow = i->second;
            aHelp.writeInto(aRow, blockNum, inUseDB);
        }
        return StatusResult();
    }

    StatusResult SQLProcessor::showIndex() const{
        std::string infor;
        int bn;
        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        std::vector<std::pair<std::string, int>> tablelist;
        while (!inUseDB->getStorage().anotherstream.eof()) {
            std::string tables, table, bN;
            std::getline(inUseDB->getStorage().anotherstream, tables);
            std::stringstream aStringstream;
            aStringstream << tables;
            aStringstream >> table;
            if (table == "")break;
            aStringstream >> bN;
            bn = std::stoi(bN);
            tablelist.push_back(std::pair<std::string, int>(table, bn));
        }
        inUseDB->getStorage().anotherstream.clear(std::ios::goodbit);
        inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
        int count = 0;
        for (auto it = tablelist.begin(); it != tablelist.end(); ++it) {
            std::string tab;
            int blockNum = it->second;
            inUseDB->getStorage().anotherstream.seekg(std::ios::beg);
            for (int i = 0; i < blockNum; ++i) {
                std::getline(inUseDB->getStorage().stream, tab);
            }
            size_t posOfDataStart = tab.find_first_of('|') + 1;
            size_t length = tab.find_last_of(',') - posOfDataStart;
            std::string str = tab.substr(posOfDataStart, length);
            ++count;
            std::cout << count << "  " << it->first << " | " << str << "\n";
        }
        std::cout << count << " rows in set" << "\n";
        return StatusResult();
    }

    StatusResult SQLProcessor::addField(std::string fieldName, std::string fieldType, int tableBlockNum) {
        //change entity block
        TableHelper aHelp;
        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        int i = tableBlockNum - 1;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string attrBlock;
        std::getline(inUseDB->getStorage().stream, attrBlock);
        int attStartPos = attrBlock.find_first_of("|") + 1;
        int len = attrBlock.find_last_of(",") - attStartPos + 1;
        std::string att;
        if (len <= 0)att = "";
        else att = attrBlock.substr(attStartPos, len);
        att = att + fieldName + " " + fieldType + " " + "YES NO NULL ,";
        StorageBlock aBlock(BlockType::entity_block);
        std::memcpy(aBlock.data, att.c_str(), att.size());
        inUseDB->getStorage().writeBlock(aBlock, tableBlockNum);

        //add new data type in every data block
        inUseDB->getStorage().stream.clear(std::ios::goodbit);
        inUseDB->getStorage().stream.seekg(std::ios::beg);
        std::map<int, Row> RowCollection;
        i = tableBlockNum;
        while (i != 0) {
            std::string str;
            std::getline(inUseDB->getStorage().stream, str);
            --i;
        }
        std::string blockString;
        std::getline(inUseDB->getStorage().stream, blockString);
        int dataStartPos = blockString.find_first_of("|") + 1;
        int length = blockString.find_last_of("]") - dataStartPos;
        std::string dataInd = blockString.substr(dataStartPos, length);

        std::vector<int> indList;
        const char* d = ",";
        char* p;
        p = strtok((char*)dataInd.c_str(), d);
        while (p)
        {
            std::stringstream ss(p);
            std::string indNum;
            ss >> indNum;
            if (indNum == "")break;
            indList.push_back(std::stoi(indNum));
            p = strtok(NULL, d);
        }

        for (auto i = indList.begin(); i != indList.end(); ++i) {
            std::string dataString;
            inUseDB->getStorage().stream.clear(std::ios::goodbit);
            inUseDB->getStorage().stream.seekg(std::ios::beg);
            int in = *i - 1;
            while (in != 0) {
                std::string str;
                std::getline(inUseDB->getStorage().stream, str);
                --in;
            }
            std::getline(inUseDB->getStorage().stream, dataString);
            int dataStartPos = dataString.find_first_of("|") + 1;
            int length = dataString.find_last_of(",") - dataStartPos + 1;
            dataString = dataString.substr(dataStartPos, length);
            dataString = dataString + fieldName + " NULL,";
            insert(*i, dataString);
        }
        return StatusResult();
    }
}
