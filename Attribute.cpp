//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Attribute.hpp"

namespace ECE141 {

 //STUDENT: Implement the attribute class here... 
    Attribute::Attribute(DataType aType) : type(aType) { is_primarykey = false; is_nullable = true; size = 24; is_auto_increment = false; }
    Attribute::Attribute(const Attribute& aCopy) : type(aCopy.type), name(aCopy.name), defaultValue(aCopy.defaultValue), size(aCopy.size), is_primarykey(aCopy.is_primarykey), is_nullable(aCopy.is_nullable), is_auto_increment(aCopy.is_auto_increment) {}
    Attribute::Attribute(std::string aName, DataType aType, uint32_t aSize, bool primaryKey, bool nullable, std::string aDefaultValue, bool auto_increment) : type(aType), name(aName), size(aSize), is_primarykey(primaryKey), is_nullable(nullable), defaultValue(aDefaultValue), is_auto_increment(auto_increment) {}
    Attribute::~Attribute() {}
    Attribute& Attribute::setName(std::string& aName) { name = aName; return *this; }
    Attribute& Attribute::setType(DataType aType) { type = aType; return *this; }
    bool Attribute::isValid() { return true; }
    bool Attribute::isPrimaryKey() const{ return is_primarykey; }
    bool Attribute::isNullable() const{ return is_nullable; }
    Attribute& Attribute::setSize(uint32_t aSize) { size = aSize; return *this; }
    Attribute& Attribute::setPrimaryKey(bool y) { is_primarykey = y; return *this;}
    Attribute& Attribute::setNullable(bool y) { is_nullable = y;return *this; }
    Attribute& Attribute::setDefaultValue(std::string aValue) {
        defaultValue = aValue;
        return *this;
    }
    Attribute& Attribute::setAutoIncrement(bool y) { is_auto_increment = y; return *this;}
}
