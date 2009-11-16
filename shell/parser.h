#ifndef PARSER_H
#define PARSER_H

#include "lexicalanalyzer.h"

#include <istream>
#include <string>
#include <list>
#include <vector>
#include <set>

class Kernel;
class Formula;
class FormulaDecl;
class FormulaSpec;
class Term;
class ShellExc;
class SyntaxExc;
class CommandExc;

class Parser
{
public:
    enum PrintForm {
        SUM,  // sum of minterms' indexes
        PROD, // product of minterms's indexes
        SOP,  // sum of products
        POS   // product of sums
    };

    Parser();

    void parse(std::string str);

    std::string termToString(Term &term, std::vector<char> vars, PrintForm form = PROD);
    std::string formulaToString(PrintForm form = SUM, Formula *formula = 0);

private:
    Kernel *kernel;
    LexicalAnalyzer lex;

    void program() throw(ShellExc);
    void command() throw(ShellExc);
    void fceDef() throw(ShellExc);
    FormulaDecl *fceDecl() throw(ShellExc);
    char fceName() throw(ShellExc);
    std::list<char> *fceVars() throw(ShellExc);
    std::list<char> *fceVarsRem() throw(ShellExc);
    FormulaSpec *fceBody() throw(ShellExc);
    FormulaSpec *sum() throw(ShellExc);
    std::set<int> *sumRem() throw(ShellExc);
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

    inline void readToken() { lex.readToken(); }

    inline SyntaxExc syntaxExc();
    inline CommandExc commandExc();

};

#endif // PARSER_H
