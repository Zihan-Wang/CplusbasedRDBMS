#ifndef appstatement_hpp
#define appstatement_hpp

#include <stdio.h>
#include <iostream>
#include "Statement.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
    class VersionStatement : public Statement {
    public:
        VersionStatement();
        VersionStatement(VersionStatement& aCopy);
        ~VersionStatement();

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult run(std::ostream& aStream) const override;

    };

    class HelpStatement : public Statement {
    public:
        HelpStatement();
        HelpStatement(HelpStatement& aHelpStatement);
        ~HelpStatement();

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult run(std::ostream& aStream) const override;
    };

    class QuitStatement : public Statement {
    public:
        QuitStatement();
        QuitStatement(QuitStatement& aQuitStatement);
        ~QuitStatement();

        StatusResult parse(Tokenizer& aTokenizer) override;

        StatusResult run(std::ostream& aStream) const override;
    };
}

#endif