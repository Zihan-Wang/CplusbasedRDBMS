#include "appstatement.hpp"
#include <unordered_set>

namespace ECE141 {
	void showhelp(Keywords aKeyword) {
		switch (aKeyword) {
		case Keywords::help_kw:
			std::cout << "     --help    // \n";
			break;
		case Keywords::version_kw:
			std::cout << "     --version    // version of ECE141 Database\n";
			break;
		case Keywords::quit_kw:
			std::cout << "     --quit    // quit ECE141 Database\n";
			break;
		case Keywords::create_kw:
			std::cout << "     --create database <name>    // create a new database\n";
			break;
		case Keywords::drop_kw:
			std::cout << "     --drop database <name>    // delete a existed database\n";
			break;
		case Keywords::use_kw:
			std::cout << "     --use database <name>    // use a existed database\n";
			break;
		case Keywords::describe_kw:
			std::cout << "     --describe database <name>    // for test\n";
			break;
		case Keywords::show_kw:
			std::cout << "     --show databases    // show all existed databases\n";
			break;
		}
	}


	VersionStatement::VersionStatement() : Statement(Keywords::version_kw){}
	VersionStatement::VersionStatement(VersionStatement &aVersionStatement) : Statement(Keywords::version_kw) {}
	VersionStatement::~VersionStatement(){}
	StatusResult VersionStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.size() > 1)return StatusResult(Errors::unknownCommand);
		aTokenizer.next();
		return StatusResult(Errors::noError);
	}
	StatusResult VersionStatement::run(std::ostream& aStream) const {
		return StatusResult();
	}


	HelpStatement::HelpStatement() : Statement(Keywords::help_kw) {}
	HelpStatement::HelpStatement(HelpStatement& aHelpStatement) : Statement(Keywords::help_kw) {}
	HelpStatement::~HelpStatement() {}
	StatusResult HelpStatement::parse(Tokenizer& aTokenizer) {
		if(aTokenizer.size()>2)return StatusResult(Errors::unknownCommand);
		std::unordered_set<Keywords> allCommand = { Keywords::help_kw, Keywords::quit_kw, Keywords::version_kw,
			Keywords::create_kw, Keywords::drop_kw, Keywords::show_kw, Keywords::use_kw, Keywords::describe_kw};
		if (aTokenizer.next() && aTokenizer.more()) {
			Token aToken = aTokenizer.current();
			auto it = allCommand.find(aToken.keyword);
			if (it == allCommand.end()) {
				return StatusResult(Errors::unknownCommand);
			}
			else showhelp(aToken.keyword);
			aTokenizer.next();
		}
		else {
			for (auto it = allCommand.begin(); it != allCommand.end(); ++it) {
				showhelp(*it);
			}
		}
		return StatusResult(Errors::noError);
	}
	StatusResult HelpStatement::run(std::ostream& aStream) const {
		return StatusResult(); // no use for help command
	}


	QuitStatement::QuitStatement() : Statement(Keywords::quit_kw) {}
	QuitStatement::QuitStatement(QuitStatement& aQuitStatement) : Statement(Keywords::quit_kw) {}
	QuitStatement::~QuitStatement() {}
	StatusResult QuitStatement::parse(Tokenizer& aTokenizer) {
		if (aTokenizer.size() > 1)return StatusResult(Errors::unknownCommand);
		aTokenizer.next();
		return StatusResult(Errors::noError);
	}
	StatusResult QuitStatement::run(std::ostream& aStream) const {
		return StatusResult(); // no use for quit command
	}
}