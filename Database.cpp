//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//
#include "stdio.h"
#include <vector>
#include <sstream>
#include "Database.hpp"
#include "View.hpp"
#include "Storage.hpp"
#include <iomanip>
#include <filesystem>

//this class represents the database object.
//This class should do actual database related work,
//we called upon by your db processor or commands

namespace ECE141 {
    
  Database::Database(const std::string aName, CreateNewStorage)
    : name(aName), storage(aName, CreateNewStorage{}) {
  }
  
  Database::Database(const std::string aName, OpenExistingStorage)
    : name(aName), storage(aName, OpenExistingStorage{}) {
  }

  Database::Database(const std::string aName, DropExistingStorage)
      : name(aName), storage(aName, DropExistingStorage{}) {
  }
  
  Database::~Database() {
  }

  void Database::createDatabase(std::string& aName) {
      StorageBlock aBlock = StorageBlock(BlockType::meta_block);
      storage.writeBlock(aBlock, 1);
  }

  void Database::describeDatabase(std::ostream& aStream, std::string& aName) {
      int blk = 0;
      std::map<char, std::string> type2string = {
          {'T', "Meta" },
          {'F', "Free" },
          {'D', "Data" },
          {'V', "Unknown" },
          {'E', "Entity" },
          {'I', "Index" }
      };
      aStream << std::setw(6) << std::setiosflags(std::ios::left)<< "Blk#" << std::setw(8) << "Type" << "Other" << "\n";
      aStream << std::setw(29) << std::setfill('-') <<'-'<< "\n";
      uint32_t totalBlock = storage.getTotalBlockCount();
      aStream << std::setw(6) << std::setiosflags(std::ios::left) << blk << std::setw(8) << "meta" << "\n";
      ++blk;
      int bn;
      std::vector<std::pair<std::string, int>> tablelist;
      getStorage().anotherstream.clear(std::ios::goodbit);
      getStorage().anotherstream.seekg(std::ios::beg);
      while (!getStorage().anotherstream.eof()) {
          std::string tables, table, bN;
          std::getline(getStorage().anotherstream, tables);
          std::stringstream aStringstream;
          aStringstream << tables;
          aStringstream >> table;
          if (table == "")break;
          aStringstream >> bN;
          bn = std::stoi(bN);
          tablelist.push_back(std::pair<std::string, int>(table, bn));
      }
      getStorage().anotherstream.clear(std::ios::goodbit);
      getStorage().anotherstream.seekg(std::ios::beg);
      StorageBlock aBlock = StorageBlock(BlockType::index_block);
      for (auto it = tablelist.begin(); it != tablelist.end(); ++it) {
          aStream << std::setw(6) << std::setiosflags(std::ios::left) << blk << std::setw(8) << "schema"<<it->first<<"\n";
          ++blk;
          int id = 1;
          getStorage().readBlock(aBlock, it->second + 1);
          if (aBlock.header.type == 'I') {
              std::stringstream ss(aBlock.data);
              std::string str;
              ss >> str;
              const char* d = ",";
              char* p;
              p = strtok((char*)str.c_str(), d);
              while (p)
              {
                  aStream << std::setw(6) << std::setiosflags(std::ios::left) << blk << std::setw(8) << "data"<<"id="<<id<<",   \""<<it->first<<"\""<<"\n";
                  ++blk;
                  ++id;
                  p = strtok(NULL, d); 
              }
          }
      }
      aStream << blk << " rows in set" << "\n";
  }

  void Database::dropDatabase(std::string& aName) {
      std::filesystem::path aPath = StorageInfo::getDefaultStoragePath() + aName + ".txt";
      if (std::filesystem::exists(aPath))std::cerr << "fail to drop database " << aName;
  }
}

