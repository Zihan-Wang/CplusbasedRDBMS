//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "AppProcessor.hpp"
#include "Tokenizer.hpp"
#include "appstatement.hpp"
#include <memory>

namespace ECE141 {
  //.....................................
  AppCmdProcessor::AppCmdProcessor(CommandProcessor *aNext) : CommandProcessor(aNext) {
  }
  
  AppCmdProcessor::~AppCmdProcessor() { delete next; }
  
  // USE: -----------------------------------------------------

  // please update version after submission !!!!!!!
  StatusResult doVersionCommand() {
      std::cout << "     ECE141b-5" << std::endl;
      return StatusResult();
  }

  StatusResult AppCmdProcessor::interpret(const Statement &aStatement) {
    //STUDENT: write code related to given statement
      switch (aStatement.getType()) {
      case Keywords::help_kw:
          break;
      case Keywords::quit_kw:
          exit(0);
          break;
      case Keywords::version_kw:
          doVersionCommand();
          break;
      }
    return StatusResult(Errors::noError);
  }
  
  // USE: factory to create statement based on given tokens...
  Statement* AppCmdProcessor::getStatement(Tokenizer &aTokenizer) {
    //STUDENT: Analyze tokens in tokenizer, see if they match one of the
    //         statements you are supposed to handle. If so, create a
    //         statement object of that type on heap and return it.
      Token curToken = aTokenizer.current();
      Statement* curStatement;
      switch (curToken.keyword) {
      case Keywords::help_kw: 
          curStatement = new HelpStatement();
          break;
      case Keywords::quit_kw:
          curStatement = new QuitStatement();
          break;
      case Keywords::version_kw:
          curStatement = new VersionStatement();
          break;
      default:
          return nullptr;
      };
    //         If you recognize the tokens, consider using a factory
    //         to construct a custom statement object subclass.
      StatusResult aStatusResult = curStatement->parse(aTokenizer);
      if (aStatusResult.code == Errors::unknownCommand)return nullptr;
      else return curStatement;
  }
  
}
