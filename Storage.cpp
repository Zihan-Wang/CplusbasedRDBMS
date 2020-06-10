//
//  Storage.cpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <iomanip> 

namespace ECE141 {

  // USE: Our main class for managing storage...
  const char* StorageInfo::getDefaultStoragePath() {
    //STUDENT -- MAKE SURE TO SET AN ENVIRONMENT VAR for DB_PATH! 
    //           This lets us change the storage location during autograder testing

    //WINDOWS USERS:  Use forward slash (/) not backslash (\) to separate paths.
    //                (Windows convert forward slashes for you)
    
    const char* thePath = std::getenv("DB_PATH");
//	const char* thePath = "/Users/hebolin/Documents/DB_PATH";
    return thePath;
  }

  //----------------------------------------------------------

  //path to the folder where you want to store your DB's...
  std::string getDatabasePath(const std::string &aDBName) {
    std::string thePath(StorageInfo::getDefaultStoragePath());
    
    //build a full path (in default storage location) to a given db file..
    thePath = thePath  + '/' + aDBName + ".txt";
    return thePath;
  }

  std::string getDatabaseIndexPath(const std::string& aDBName) {
      std::string thePath(StorageInfo::getDefaultStoragePath());

      //build a full path (in default storage location) to a given db file..
      thePath = thePath + '/' + aDBName +"_index"+".txt";
      return thePath;
  }

  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName, CreateNewStorage) : name(aName) {
    std::string thePath = getDatabasePath(name);
    std::string theIndexPath = getDatabaseIndexPath(name);
    std::filesystem::path aPath(thePath);
    std::filesystem::path anIndexPath(theIndexPath);
    anotherstream.open(anIndexPath);
    //try to create a new db file in known storage location.
    //throw error if it fails...
    stream.open(aPath);
    if (!std::filesystem::exists(aPath)&& !std::filesystem::exists(anIndexPath)) {
        std::cerr<< "fail to create a new database file"<<std::endl;
    }
  }

  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName, OpenExistingStorage) : name(aName) {
      std::string thePath = getDatabasePath(name);
      std::string theIndexPath = getDatabaseIndexPath(name);
      std::filesystem::path aPath(thePath);
      std::filesystem::path anIndexPath(theIndexPath);
    //try to OPEN a db file a given storage location
    //if it fails, throw an error
    if (!std::filesystem::exists(thePath)) {
        throw "no such directory or file";
    }
    stream.open(thePath);
    anotherstream.open(anIndexPath);
    if (!stream.is_open()||!anotherstream.is_open()) {
        throw "fail to open file";
    }
  }

  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName, DropExistingStorage) : name(aName) {
      std::string thePath = getDatabasePath(name);
      std::string theIndexPath = getDatabaseIndexPath(name);
      std::filesystem::path aPath(thePath);
      std::filesystem::path anIndexPath(theIndexPath);
      //try to OPEN a db file a given storage location
      //if it fails, throw an error
      if (!std::filesystem::exists(aPath)) {
          throw "no such directory or file";
      }
      stream.close();
      anotherstream.close();
      if (stream.is_open()) {
          throw "fail to close database file";
      }
      std::filesystem::remove(aPath);
      std::filesystem::remove(anIndexPath);
  }

  // USE: dtor ---------------------------------------
  Storage::~Storage() {
    stream.close();
    anotherstream.close();
  }

  void Storage::update() {
      std::string thePath = getDatabasePath(name);
      std::string theIndexPath = getDatabaseIndexPath(name);
      std::filesystem::path aPath(thePath);
      std::filesystem::path anIndexPath(theIndexPath);
      stream.close();
      stream.open(aPath);
      anotherstream.close();
      anotherstream.open(anIndexPath);
  }

  // USE: validate we're open and ready ---------------------------------------
  bool Storage::isReady() const {
    return stream.is_open();
  }

  // USE: count blocks in file ---------------------------------------
  uint32_t Storage::getTotalBlockCount() {
    //how can we compute the total number of blocks in storage?
    stream.seekg(std::ios::beg);
    uint32_t theCount=0;
    std::string tmp;
    std::getline(stream, tmp);
    if (!isReady())throw "no file opens";
    while (!stream.eof()) {
        ++theCount;
        std::getline(stream, tmp);
    }
    stream.clear(std::ios::goodbit);
    stream.seekg(std::ios::beg);
    return theCount;
  }

  // Call this to locate a free block in this storage file.
  // If you can't find a free block, then append a new block and return its blocknumber
  ECE141::BlockType checkType(std::string aBlockString) {
      if (aBlockString.size() < 2)return BlockType::unknown_block;
      switch (aBlockString[1]) {
      case 'F':
          return BlockType::free_block;
      case 'D':
          return BlockType::data_block;
      case 'M':
          return BlockType::meta_block;
      case 'I':
          return BlockType::index_block;
      case 'E':
          return BlockType::entity_block;
      default:
          return BlockType::unknown_block;
      }
  }

  StatusResult Storage::findFreeBlockNum() {
      stream.clear(std::ios::goodbit);
      stream.seekg(std::ios::beg);
      if (!isReady())throw "no file opens";
      std::string tmp;
      uint32_t count = 1;
      while (!stream.eof()) {
          std::getline(stream, tmp);
          if (checkType(tmp) == BlockType::free_block || checkType(tmp) == BlockType::unknown_block) {
              stream.clear();
              return StatusResult{ noError, count };
          }
          ++count;
      }
      stream.clear(std::ios::goodbit);
      stream.seekg(std::ios::beg);
      return StatusResult{ noError, count }; //return blocknumber in the 'value' field...
  }

  // USE: for use with "storable API" [NOT REQUIRED -- but very useful]

  StatusResult Storage::save(Storable &aStorable) {
    //High-level IO: save a storable object (like a table row)...
     return StatusResult{noError};
  }

// USE: for use with "storable API" [NOT REQUIRED -- but very useful]

  StatusResult Storage::load(Storable &aStorable) {
    //high-level IO: load a storable object (like a table row)
    return StatusResult{noError};
  }

  // USE: write data a given block (after seek)
  static std::map<char, std::string> type2string = {
          {'T', "Meta" },
          {'F', "Free" },
          {'D', "Data" },
          {'V', "Unknown" },
          {'E', "Entity" },
          {'I', "Index" }
  };
  static std::map<char, char> string2type = {
          {'M','T'},
          {'F','F'},
          {'D','D'},
          {'U','V'},
          {'E','E'},
          {'I','I'}
  };
  StatusResult Storage::writeBlock(StorageBlock& aBlock, uint32_t aBlockNumber) {
      //STUDENT: Implement this; this is your low-level block IO...
      stream.clear(std::ios::goodbit);
      stream.seekg(std::ios::beg);
      stream.seekp(std::ios::beg);
      std::string type(type2string[aBlock.header.type]);
      int i = aBlockNumber - 1;
      while (i != 0) {
          std::string str;
          std::getline(stream, str);
          --i;
      }
      stream.seekp(stream.tellg());
      stream << "[" << std::setw(7) << std::setiosflags(std::ios::left) << type << "|" << aBlock.data << "]" << "\n";
      update();
      return StatusResult{};
  }

  // USE: read data from a given block (after seek)
  StatusResult Storage::readBlock(StorageBlock &aBlock, uint32_t aBlockNumber) {
    //STUDENT: Implement this; this is your low-level block IO...
      if (aBlockNumber >= getTotalBlockCount())throw "out of index";
      stream.clear(std::ios::goodbit);
      stream.seekg(std::ios::beg);
      stream.seekp(std::ios::beg);
      uint32_t count = aBlockNumber;
      std::string tmp;
      while (count != 0) {
          tmp = "";
          std::getline(stream, tmp);
          --count;
      }
      size_t posOfDataStart = tmp.find_first_of('|') + 1;
      size_t posOfType = tmp.find_first_of('[') + 1;
      size_t length = tmp.find_last_of(']') - posOfDataStart;
      aBlock.header.type = string2type[tmp[posOfType]];
      std::memcpy(aBlock.data, tmp.substr(posOfDataStart, length).c_str(), length);
      return StatusResult{};
  }


}


