//
//  Row.hpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "Storage.hpp"
#include "Database.hpp"
#include "Value.hpp"
namespace ECE141 {


  class Row {
  public:

    Row();
    Row(const Row &aRow);
    ~Row();
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    
      //STUDENT: What other methods do you require?
    std::map<std::string, std::string> data;  //you're free to change this if you like...
  };

}

#endif /* Row_hpp */
