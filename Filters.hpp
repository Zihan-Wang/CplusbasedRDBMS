//
//  Filters.hpp
//  RGAssignment6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Filters_h
#define Filters_h

#include <stdio.h>
#include <vector>
#include <string>
#include "Errors.hpp"
#include "Value.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  class Row;
  class Schema;
  
  struct Operand {
    Operand(std::string& aName, TokenType aType) : name(aName), type(aType) {}
    Operand(std::string &aName, TokenType aType, ValueType &aValue, uint32_t anId=0)
      : name(aName), type(aType), entityId(anId), value(aValue) {}
    Operand(const Operand& aCopy) :name(aCopy.name), type(aCopy.type), value(aCopy.value),entityId(aCopy.entityId) {}
    TokenType   type; //so we know if it's a field, a const (number, string)...
    std::string name; //for named attr. in schema
    ValueType   value;
    uint32_t    entityId;
  };
  
  //---------------------------------------------------

  struct Expression {
    Operand     lhs;
    Operand     rhs;
    Operators   op;
    
    Expression(Operand &aLHSOperand, Operators anOp, Operand &aRHSOperand)
      : lhs(aLHSOperand), op(anOp), rhs(aRHSOperand) {}
    Expression(const Expression& aCopy) : lhs(aCopy.lhs), op(aCopy.op), rhs(aCopy.rhs) {}
    bool operator()(KeyValues& aList) {
        switch (op)
        {
        case ECE141::Operators::equal_op:
            if (aList[lhs.name] == rhs.value)return true;
            break;
        case ECE141::Operators::notequal_op:
            if (aList[lhs.name] != rhs.value)return true;
            break;
        case ECE141::Operators::lt_op:
            if (aList[lhs.name] < rhs.value)return true;
            break;
        case ECE141::Operators::lte_op:
            if (aList[lhs.name] <= rhs.value)return true;
            break;
        case ECE141::Operators::gt_op:
            if (aList[lhs.name] > rhs.value)return true;
            break;
        case ECE141::Operators::gte_op:
            if (aList[lhs.name] >= rhs.value)return true;
            break;
        default:
            break;
        }
        return false;
    }
  };
  
  //---------------------------------------------------

  using Expressions = std::vector<Expression>;

  //---------------------------------------------------

  class Filters {
  public:
    
    Filters();
    Filters(const Filters &aFilters);
    ~Filters();
    
    size_t        getCount() const {return expressions.size();}
    bool          matche(std::map<std::string, std::string> aList) const;
    bool          matches(Row aRow) const;
    std::vector<Row>          filterRows(std::vector<Row> RowCollection);
    std::map<int, Row>          filterRowCollection(std::map<int, Row> RowCollection, std::map<std::string, std::string> updateData);
    Filters&      add(Expression anExpression);
    Filters& addAnd(Expression anExpression) {
        andExpressions.push_back(anExpression);
        return *this;
    }
    Filters& addOr(Expression anExpression) {
        orExpressions.push_back(anExpression);
        return *this;
    }
    void          setLimit(int t) { isLimit = true; LimitNum = t; }
    int           getLimitNum() {
        if (isLimit) {
            return LimitNum;
        }
        else return -1;
    }
    bool          hasOrder;
    std::string   orderBy;
    friend class Tokenizer;
    
  protected:
    Expressions  expressions;
    Expressions  andExpressions;
    Expressions  orExpressions;
    bool          isLimit;
    int           LimitNum;
  };
   
}

#endif /* Filters_h */

