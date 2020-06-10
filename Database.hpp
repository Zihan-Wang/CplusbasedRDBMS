//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include "Storage.hpp"


namespace ECE141 {
  
  class Database  {
  public:
    
    Database(const std::string aName, CreateNewStorage);
    Database(const std::string aName, OpenExistingStorage);
    Database(const std::string aName, DropExistingStorage);
    ~Database();
    
    Storage&          getStorage() {return storage;}
    std::string&      getName() {return name;}
    void createDatabase(std::string& aName);
    void describeDatabase(std::ostream& aStream, std::string& aName);
    void dropDatabase(std::string& aName);
  protected:
    
    std::string     name;
    Storage         storage;
  };
  
}

#endif /* Database_hpp */
