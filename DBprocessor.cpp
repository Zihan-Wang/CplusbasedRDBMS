#include "DBProcessor.hpp"
#include "Tokenizer.hpp"
#include "dbstatement.hpp"
#include <memory>

namespace ECE141 {

	DBCmdProcessor::DBCmdProcessor(CommandProcessor* aNext) : CommandProcessor(aNext){}
    DBCmdProcessor::~DBCmdProcessor() { delete next; }
    StatusResult DBCmdProcessor::processInput(Tokenizer& aTokenizer) {
        std::unique_ptr<Statement> aStatement(getStatement(aTokenizer)); // smart pointer
        if (aStatement) {
            StatusResult aResult = interpreter(*aStatement, aTokenizer);
            return aResult;
        }
        else if (next) {
            return next->processInput(aTokenizer);
        }
        return StatusResult{ ECE141::unknownCommand };
    }

	Statement* DBCmdProcessor::getStatement(Tokenizer &aTokenizer) {
        Token curToken = aTokenizer.current();
        if (!aTokenizer.hasnext())return nullptr;
        Token nextToken = aTokenizer.peek(1);
        switch (curToken.keyword) {
        case Keywords::create_kw:
            if (nextToken.keyword != Keywords::database_kw)break;
            return new CreateStatement();
            break;
        case Keywords::drop_kw:
            if (nextToken.keyword != Keywords::database_kw)break;
            return new DropStatement();
            break;
        case Keywords::show_kw:
            if (nextToken.keyword != Keywords::databases_kw)break;
            return new ShowStatement();
            break;
        case Keywords::describe_kw:
            if (nextToken.keyword != Keywords::database_kw)break;
            return new DescribeStatement();
            break;
        default:
            return nullptr;
        };
        return nullptr;
	}

	StatusResult DBCmdProcessor::interpreter(Statement &aStatement, Tokenizer& aTokenizer) {
        ECE141::StatusResult aResult = aStatement.parse(aTokenizer);
        return aResult;
    }

    StatusResult DBCmdProcessor::interpret(const Statement& aStatement) {
        return StatusResult();
    }
}