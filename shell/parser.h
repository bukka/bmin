#ifndef PARSER_H
#define PARSER_H

#include "lexicalanalyzer.h"
#include "shellexc.h"

#include <iostream>
#include <list>
#include <set>

class Kernel;
class FormulaDecl;
class FormulaSpec;
class Term;

class Parser
{
public:
    enum PrintForm { SUM, PROD, VARS }

    Parser();

    void parse(std::string &str);
    void parse(std::istream &is);

    std::ostream &printFce(std::ostream &os, PrintForm form, Formula *f = 0);
    std::ostream &printTerm(std::ostream &os, Term &term);

private:
    Kernel *kernel;
    LexicalAnalyzer lex;

    void program() throw(ShellExc);
    void command() throw(ShellExc);
    void fceDef() throw(ShellExc);
    const FormulaDecl *fceDecl() throw(ShellExc);
    char fceName() throw(ShellExc);
    std::list<char> *fceVars() throw(ShellExc);
    std::list<char> *fceVarsRem() throw(ShellExc);
    FormulaSpec *fceBody() throw(ShellExc);
    FormulaSpec *sum() throw(ShellExc);
    std::set<int> *sumRem();
    FormulaSpec *prod() throw(ShellExc);
    std::set<int> *prodRem() throw(ShellExc);
    std::set<int> *mTerms() throw(ShellExc);
    std::set<int> *dTerms() throw(ShellExc);
    std::set<int> *fceIndexes() throw(ShellExc);
    std::set<int> *fceIndexesRem() throw(ShellExc);

    // only compare tokens
    inline bool cmp(LexicalAnalyzer::Token tok);
    // compare and if true read token
    bool cmpr(LexicalAnalyzer::Token tok);
    // compare and if false throw exception
    inline bool cmpe(LexicalAnalyzer::Token tok) throw(ShellExc);
    // compare and if true read token otherwise throw exception
    inline bool cmpre(LexicalAnalyzer::Token tok) throw(ShellExc);

    inline SyntaxExc syntaxExc();
    inline CommandExc commandExc();

};

#endif // PARSER_H
