//
//  Filters.hpp
//  Assignement6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//


#include <stdio.h>
#include "Filters.hpp"
#include "Row.hpp"
#include "Schema.hpp"
#include <algorithm>
#include <vector>
namespace ECE141 {
    Filters::Filters():hasOrder(false), isLimit(false), LimitNum(-1) {}
    Filters::Filters(const Filters& aFilters) : hasOrder(aFilters.hasOrder), isLimit(aFilters.isLimit), LimitNum(aFilters.LimitNum), expressions(aFilters.expressions), orderBy(aFilters.orderBy), andExpressions(aFilters.andExpressions), orExpressions(aFilters.orExpressions){}
    Filters::~Filters() {}

    bool          Filters::matche(std::map<std::string, std::string> aList) const {
        for (auto i = expressions.begin(); i != expressions.end(); ++i) {
            if (aList.find((*i).lhs.name) == aList.end())return false;
            switch ((*i).op)
            {
            case ECE141::Operators::equal_op:
                if (aList[(*i).lhs.name] != (*i).rhs.name)return false;
                break;
            case ECE141::Operators::notequal_op:
                if (aList[(*i).lhs.name] == (*i).rhs.name)return false;
                break;
            case ECE141::Operators::lt_op:
                if (std::stof(aList[(*i).lhs.name]) >= std::stof((*i).rhs.name))return false;
                break;
            case ECE141::Operators::lte_op:
                if (std::stof(aList[(*i).lhs.name]) > std::stof((*i).rhs.name))return false;
                break;
            case ECE141::Operators::gt_op:
                if (std::stof(aList[(*i).lhs.name]) <= std::stof((*i).rhs.name))return false;
                break;
            case ECE141::Operators::gte_op:
                if (std::stof(aList[(*i).lhs.name]) < std::stof((*i).rhs.name))return false;
                break;
            default:
                break;
            }
        }
        return true; 
    }

    bool Filters::matches(Row aRow) const {
        bool matched = true;
        std::map<std::string, std::string> aList = aRow.data;
        for (auto i = andExpressions.begin(); i != andExpressions.end(); ++i) {
            if (aList.find(i->lhs.name) == aList.end())return false;
            switch ((*i).op)
            {
            case ECE141::Operators::equal_op:
                if (aList[(*i).lhs.name] != (*i).rhs.name)matched = false;
                break;
            case ECE141::Operators::notequal_op:
                if (aList[(*i).lhs.name] == (*i).rhs.name)matched = false;
                break;
            case ECE141::Operators::lt_op:
                if (std::stof(aList[(*i).lhs.name]) >= std::stof((*i).rhs.name))matched = false;
                break;
            case ECE141::Operators::lte_op:
                if (std::stof(aList[(*i).lhs.name]) > std::stof((*i).rhs.name))matched = false;
                break;
            case ECE141::Operators::gt_op:
                if (std::stof(aList[(*i).lhs.name]) <= std::stof((*i).rhs.name))matched = false;
                break;
            case ECE141::Operators::gte_op:
                if (std::stof(aList[(*i).lhs.name]) < std::stof((*i).rhs.name))matched = false;
                break;
            default:
                break;
            }
        }

        for (auto i = orExpressions.begin(); i != orExpressions.end(); ++i) {
            if (aList.find((*i).lhs.name) == aList.end())continue;
            switch ((*i).op)
            {
            case ECE141::Operators::equal_op:
                if (aList[(*i).lhs.name] == (*i).rhs.name)matched = true;
                break;
            case ECE141::Operators::notequal_op:
                if (aList[(*i).lhs.name] != (*i).rhs.name)matched = true;
                break;
            case ECE141::Operators::lt_op:
                if (std::stof(aList[(*i).lhs.name]) < std::stof((*i).rhs.name))matched = true;
                break;
            case ECE141::Operators::lte_op:
                if (std::stof(aList[(*i).lhs.name]) <= std::stof((*i).rhs.name))matched = true;
                break;
            case ECE141::Operators::gt_op:
                if (std::stof(aList[(*i).lhs.name]) > std::stof((*i).rhs.name))matched = true;
                break;
            case ECE141::Operators::gte_op:
                if (std::stof(aList[(*i).lhs.name]) >= std::stof((*i).rhs.name))matched = true;
                break;
            default:
                break;
            }
        }
        return matched;
    }

    Filters& Filters::add(Expression anExpression) {
        expressions.push_back(anExpression);
        return *this;
    }

    struct myclass {
        myclass(std::string aBy) :orderBy(aBy) {}
        std::string orderBy;
        bool operator() (Row data1, Row data2) {
            auto it1 = data1.data.find(orderBy);
            auto it2 = data2.data.find(orderBy);
            if (it1->second.size() == 0 || it2->second.size() == 0)return it1->second.size() < it2->second.size();
            if (it1->second.at(0) - (it2->second.at(0)) == 0) { 
                int result = it1->second.compare(it2->second);
                return result < 0; 
            }
            return (it1->second.at(0) < it2->second.at(0));
        }
    };

    std::vector<Row>          Filters::filterRows(std::vector<Row> RowCollection) {
        std::vector<Row> outPutRows;
        for (auto i = RowCollection.begin(); i != RowCollection.end();++i) {
            if (matche(i->data))outPutRows.push_back(*i);
        }
        if (hasOrder) {
            myclass cmp(orderBy);
            std::sort(outPutRows.begin(), outPutRows.end(), cmp);
        }
        if (isLimit&&LimitNum <= outPutRows.size()) {
            std::vector<Row> aNew;
            for (int i = 0; i < LimitNum; ++i) {
                aNew.push_back(outPutRows[i]);
            }
            return aNew;
        }
        return outPutRows;
    }
    std::map<int, Row> Filters::filterRowCollection(std::map<int, Row> RowCollection, std::map<std::string, std::string> updateData) {
        std::map<int, Row> outPutRows;
        for (auto i = RowCollection.begin(); i != RowCollection.end(); ++i) {
            if (matches(i->second)) {
                for (auto in = updateData.begin(); in != updateData.end(); ++in) {
                    i->second.data[in->first] = in->second;
                }
                outPutRows.insert(*i); 
            }
        }
        return outPutRows;
    }
}



