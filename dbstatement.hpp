#ifndef dbstatement_hpp
#define dbstatement_hpp

#include"stdio.h"
#include "Value.hpp"
#include <string>
#include <iostream>
#include "Statement.hpp"
#include "SQLProcessor.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
	class CreateStatement : public Statement {
	public:
		CreateStatement();
		CreateStatement(CreateStatement& aCopy);
		~CreateStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "create statement"; }
		StatusResult  run(std::ostream& aStream) const override;
	};

	class DropStatement : public Statement {
	public:
		DropStatement();
		DropStatement(DropStatement& aCopy);
		~DropStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "drop statement"; }
		StatusResult  run(std::ostream& aStream) const override;
	};

	class UseStatement : public Statement {
	public:
		UseStatement(SQLProcessor* aProcess);
		UseStatement(UseStatement& aCopy);
		~UseStatement();
		StatusResult  parse(Tokenizer& aTokenizer);
		const char* getStatementName() const { return "use statement"; }
		StatusResult  run(std::ostream& aStream) const override;
	private:
		SQLProcessor* Process;
	};

	class ShowStatement : public Statement {
	public:
		ShowStatement();
		ShowStatement(ShowStatement& aCopy);
		~ShowStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "show statement"; }
		StatusResult  run(std::ostream& aStream) const override;
	};

	class DescribeStatement : public Statement {
	public:
		DescribeStatement();
		DescribeStatement(DescribeStatement& aCopy);
		~DescribeStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "describe statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
	};

	class CreateTableStatement : public Statement {
	public:
		CreateTableStatement();
		CreateTableStatement(SQLProcessor* aProcess);
		CreateTableStatement(CreateTableStatement& aCopy);
		~CreateTableStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "create table statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};
	
	class DropTableStatement : public Statement {
	public:
		DropTableStatement();
		DropTableStatement(SQLProcessor* aProcess);
		DropTableStatement(DropTableStatement& aCopy);
		~DropTableStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "Drop table statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};
	
	class DescribeTableStatement : public Statement {
	public:
		DescribeTableStatement();
		DescribeTableStatement(SQLProcessor* aProcess);
		DescribeTableStatement(DescribeTableStatement& aCopy);
		~DescribeTableStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "describe table statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};

	class ShowTableStatement : public Statement {
	public:
		ShowTableStatement();
		ShowTableStatement(SQLProcessor* aProcess);
		ShowTableStatement(ShowTableStatement& aCopy);
		~ShowTableStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "show table statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};

	class InsertStatement: public Statement {
	public:
		InsertStatement();
		InsertStatement(SQLProcessor* aProcess);
		InsertStatement(InsertStatement& aCopy);
		~InsertStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "insert statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
		StatusResult findTable(Database* inUseDB, std::string tablename);
		StatusResult getTableInsert(Database* inUseDB, int bn, std::vector<std::string> fields, std::vector<ValueType> values);
		StatusResult parseField(Tokenizer& aTokenizer, std::vector<std::string>& list);
		StatusResult parseValue(Tokenizer& aTokenizer, std::vector<ValueType>& list);
	};

	class DeleteStatement : public Statement {
	public:
		DeleteStatement();
		DeleteStatement(SQLProcessor* aProcess);
		DeleteStatement(DeleteStatement& aCopy);
		~DeleteStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "delete statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};

	class SelectStatement :public Statement {
	public:
		SelectStatement();
		SelectStatement(SQLProcessor* aProcess);
		SelectStatement(SelectStatement& aCopy);
		~SelectStatement();
		StatusResult  parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "select statement"; }
		StatusResult  run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};

	class UpdateStatement :public Statement {
	public:
		UpdateStatement();
		UpdateStatement(SQLProcessor* aProcess);
		UpdateStatement(UpdateStatement & aCopy);
		~UpdateStatement() {}
		StatusResult parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "update statement"; }
		StatusResult run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};

	class ShowIndexStatement :public Statement {
	public:
		ShowIndexStatement();
		ShowIndexStatement(SQLProcessor* aProcess);
		ShowIndexStatement(ShowIndexStatement& aCopy);
		~ShowIndexStatement() {}
		StatusResult parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "show index statement"; }
		StatusResult run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};

	class AlterStatement :public Statement {
	public:
		AlterStatement();
		AlterStatement(SQLProcessor* aProcess);
		AlterStatement(AlterStatement& aCopy);
		~AlterStatement() {}
		StatusResult parse(Tokenizer& aTokenizer) override;
		const char* getStatementName() const { return "alter index statement"; }
		StatusResult run(std::ostream& aStream, std::string aName) const;
		SQLProcessor* process;
	};
}
#endif
