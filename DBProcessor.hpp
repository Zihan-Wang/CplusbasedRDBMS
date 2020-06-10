#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include "stdio.h"
#include "CommandProcessor.hpp"

namespace ECE141 {
	class DBCmdProcessor :public CommandProcessor {
    public:
        DBCmdProcessor(CommandProcessor* aNext = nullptr);
        virtual ~DBCmdProcessor();
        virtual StatusResult processInput(Tokenizer& aTokenizer);
        virtual Statement* getStatement(Tokenizer& aTokenizer);
        virtual StatusResult  interpreter(Statement& aStatement, Tokenizer& aTokenizer);
        virtual StatusResult interpret(const Statement& aStatement);
	};
}


#endif
