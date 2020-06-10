//
//  main.cpp
//  Database2
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>

#include "AppProcessor.hpp"
#include "DBProcessor.hpp"
#include "SQLProcessor.hpp"
#include "Tokenizer.hpp"
#include "Errors.hpp"
#include "Storage.hpp"


// USE: ---------------------------------------------

static std::map<int, std::string> theErrorMessages = {
  {ECE141::illegalIdentifier, "Illegal identifier"},
  {ECE141::unknownIdentifier, "Unknown identifier"},
  {ECE141::databaseExists, "Database exists"},
  {ECE141::tableExists, "Table Exists"},
  {ECE141::syntaxError, "Syntax Error"},
  {ECE141::unknownCommand, "Unknown command"},
  {ECE141::unknownDatabase,"Unknown database"},
  {ECE141::unknownTable,   "Unknown table"},
  {ECE141::unknownError,   "Unknown error"}
};

void showError(ECE141::StatusResult &aResult) {
  std::string theMessage="Unknown Error";
  if(theErrorMessages.count(aResult.code)) {
    theMessage=theErrorMessages[aResult.code];
  }
  std::cout << "Error (" << aResult.code << ") " << theMessage << "\n";
}

//build a tokenizer, tokenize input, ask processors to handle...
ECE141::StatusResult handleInput(std::istream &aStream, ECE141::CommandProcessor &aProcessor) {
  ECE141::Tokenizer theTokenizer(aStream);
  //tokenize the input from aStream...
  ECE141::StatusResult theResult=theTokenizer.tokenize();
  while(theResult && theTokenizer.more()) {
    if(";"==theTokenizer.current().data) {
      theTokenizer.next();  //skip the ";"...
    }
    else theResult=aProcessor.processInput(theTokenizer);
  }
  return theResult;
}

//----------------------------------------------

int main(int argc, const char * argv[]) {

  const char* path = ECE141::StorageInfo::getDefaultStoragePath();
  ECE141::SQLProcessor   theProcessor3;
  ECE141::AppCmdProcessor   theProcessor2(&theProcessor3);  //add your db processor here too!
  ECE141::DBCmdProcessor   theProcessor1(&theProcessor2);
  ECE141::StatusResult      theResult{};
  if(argc>1) {
    std::ifstream theStream(argv[1]);
    return handleInput(theStream, theProcessor1);
  }
  else {
    std::string theUserInput;
    bool running=true;
    do {
        std::cout << "\n> ";
        if (std::getline(std::cin, theUserInput)) {
            if (theUserInput.length()) {
                std::stringstream theStream(theUserInput);
                theResult = handleInput(theStream, theProcessor1);
                if (!theResult) showError(theResult);
            }
            if (ECE141::userTerminated == theResult.code)
                running = false;
        }
    }
    while (running);
  }
  

  return 0;
}

