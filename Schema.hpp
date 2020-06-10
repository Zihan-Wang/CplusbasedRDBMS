//
//  Schema.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Schema_hpp
#define Schema_hpp

#include <stdio.h>
#include <vector>
#include <optional>
#include "Attribute.hpp"
#include "Errors.hpp"
//#include "Row.hpp"

namespace ECE141 {
  
  struct Block;
  struct Expression;
  class  Database;
  class  Tokenizer;
  
  //using StringList = std::vector<std::string>;
  //using attribute_callback = std::function<bool(const Attribute &anAttribute)>;
  
  using AttributeList = std::vector<Attribute>;
  using Attributeopt = std::optional<Attribute>;
  //STUDENT: If you're using the Storable interface, add that to Schema class?

  class Schema  {
  public:
                          Schema(const std::string aName);
                          Schema(const Schema &aCopy);
    
                          ~Schema();
        
    const std::string&    getName() const {return name;}
    const AttributeList&  getAttributes() const {return attributes;}
    uint32_t              getBlockNum() const {return blockNum;}
    bool                  isChanged() {return changed;}
    
    Schema&               addAttribute(const Attribute &anAttribute);
    const Attributeopt&      getAttribute(const std::string &aName) const;
      
        //STUDENT: These methods will be implemented in the next assignment...
    
    //Value                 getDefaultValue(const Attribute &anAttribute) const;
    //StatusResult          validate(KeyValues &aList);
    
    std::string           getPrimaryKeyName() const;
    //uint32_t              getNextAutoIncrementValue();
    
    
    //STUDENT: Do you want to provide an each() method for observers?
    
    //friend class Database; //is this helpful?

  protected:
    
    AttributeList   attributes;
    std::string     name;
    uint32_t        blockNum;  //storage location.
    bool            changed;
  };
  
}
#endif /* Schema_hpp */
