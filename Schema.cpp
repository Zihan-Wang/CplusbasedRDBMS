//
//  Schema.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Schema.hpp"
#include <iostream>
namespace ECE141 {

 //STUDENT: Implement the Schema class here...
    Schema::Schema(const std::string aName) : name(aName) { blockNum = 0; changed = false; }
    Schema::Schema(const Schema& aCopy) : name(aCopy.name), blockNum(aCopy.blockNum), attributes(aCopy.attributes), changed(aCopy.changed){}
    Schema::~Schema() {}
    Schema& Schema::addAttribute(const Attribute& anAttribute) {
        attributes.push_back(anAttribute);
        return *this;
    }
    const Attributeopt& Schema::getAttribute(const std::string& aName) const {
//		Attributeopt temp = std::nullopt;
        for (auto it = attributes.begin(); it != attributes.end(); ++it) {
            if (it->getName() == aName)return *it;
        }
//		return temp;
		
    }

    std::string Schema::getPrimaryKeyName() const {
        for (auto it = attributes.begin(); it != attributes.end(); ++it) {
            if (it->isPrimaryKey())return it->getName();
        }
//		return "Null";
    }
}
