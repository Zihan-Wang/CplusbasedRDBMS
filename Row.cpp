//
//  Row.cpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Row.hpp"

namespace ECE141 {
  
  //STUDENT: You need to fully implement these methods...

  Row::Row() {}
  Row::Row(const Row &aRow) : data(aRow.data){}
  Row& Row::operator=(const Row &aRow) {
      data = aRow.data;
      return *this;
  }
  bool Row::operator==(Row &aCopy) const {
      if (data.size() == aCopy.data.size()) {
          for (auto i = data.begin(); i != data.end(); ++i) {
              if (aCopy.data.find(i->first) == aCopy.data.end())return false;
              else if(aCopy.data.find(i->first)->second != i->second)return false;
          }
          return true;
      }
      return false;
  }
  Row::~Row() {}

}
