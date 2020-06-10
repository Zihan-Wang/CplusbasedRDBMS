//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include<variant>

namespace ECE141 {
  enum class DataType {
    no_type='N', bool_type='B', datetime_type='D', float_type='F', int_type='I',  varchar_type='V', timestamp_type='T'
  };

  class Attribute {
  protected:
    std::string   name;
    DataType      type;
    bool is_primarykey;
    bool is_nullable;
    bool is_auto_increment;
    uint32_t      size=24;
    uint32_t      autoIncrement;
    std::string defaultValue = "NULL";
    //STUDENT: What other data should you save in each attribute?
    
  public:
          
    Attribute(DataType aType=DataType::no_type);
    Attribute(std::string aName, DataType aType, uint32_t aSize = 24, bool primaryKey=false, bool nullable=true, std::string aDefaultValue="NULL", bool is_auto_increment=false);
    Attribute(const Attribute &aCopy);
    ~Attribute();
    
    Attribute&    setName(std::string &aName);
    Attribute&    setType(DataType aType);

    bool          isValid(); //is this schema valid? Are all the attributes value?
    bool          isPrimaryKey() const;
    bool          isNullable() const;
    const std::string&  getName() const {return name;}
    DataType            getType() const {return type;}

    //STUDENT: are there other getter/setters to manage other attribute properties?
    Attribute& setSize(uint32_t aSize);
    Attribute& setPrimaryKey(bool);
    Attribute& setNullable(bool);
    Attribute& setDefaultValue(std::string aValue);
    Attribute& setAutoIncrement(bool);
    bool getPrimaryKey() { return is_primarykey; }
    bool getNullable() { return is_nullable; }
    bool getAuto_Increment() { return is_auto_increment; }
    uint32_t getSize(){ return size; }
    std::string getDefaultValue() { return defaultValue; }
  };
  

}


#endif /* Attribute_hpp */
