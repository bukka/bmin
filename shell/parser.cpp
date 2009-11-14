#include "parser.h"
#include "lexicalanalyzer.h"
#include "shellexc.h"

#include "kernel/kernel.h"
#include "kernel/formula.h"
#include "kernel/term.h"

#include <string>
#include <istream>
#include <sstream>
#include <list>
#include <set>

using namespace std;

Parser::Parser()
{
    kernel = Kernel::instance();
}

string termToString(Term &term, PrintForm form)
{

}

string Parser::formulaToString(PrintForm form, Formula *f)
{
    if (!f)
        f = kernel->getFormula();

    switch (form) {
    case SUM:

    case PROD:

    case SOP:

    case POS:
    }
}


void parse(std::string & str)
{
    parse(stringstream(str));
}

void parse(std::istream & is)
{
    lex.analyze(is);

    try {
        program();
    }
    catch (ShellExc & exc) {

    }
}

void Parser::program() throw(ShellExc)
{
    lex.readToken();

    if (cmpr(LexicalAnalyzer::COMMAND))
        command();
    else if (cmpr(LexicalAnalyzer::LETTER))
        fceDef();
    else
        throw syntaxExc();

}

void Parser::command() throw(ShellExc)
{
    switch (lex.getCommand()) {
    case LexicalAnalyzer::MINIMIZE:
        kernel->minimizeFormula();
        break;
    case LexicalAnalyzer::EXIT:
        kernel->exit();
        break;
    default:
        throw commandExc();
    }
}

void Parser::fceDef() throw(ShellExc)
{
    FormulaDecl *decl = fceDecl();
    cmpre(LexicalAnalyzer::ASSIGN);
    FormulaSpec *spec = fceBody();

    kernel->setFormula(new Formula(spec, decl));
    delete decl;
    delete spec;
}

FormulaDecl *Parser::fceDecl() throw(ShellExc)
{
    char name = fceName();
    cmpre(LexicalAnalyzer::LPAR);
    list<char> *l = fceVars();
    try {
        cmpre(LexicalAnalyzer::RPAR);
    }
    catch (ShellExc &exc) {
        delete l;
        throw exc;
    }

    vector<char> *v = new vector<char>(l->size());
    copy(l->begin(), l->end(), v->begin());
    delete l;

    return new FormulaDecl(v, name);
}

char Parser::fceName() throw(ShellExc)
{
    char name = lex.getLetter();
    lex.readToken();
    return name;
}

list<char> *Parser::fceVars() throw(ShellExc)
{
    cmpe(LexicalAnalyzer::LETTER);
    list<char> *l = fceVarsRem();
    l->push_front(lex.getLetter());
    return l;
}

list<char> *Parser::fceVarsRem() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::RPAR))
        return new list<char>;
    else if (cmpr(LexicalAnalyzer::COMMA) && cmpr(LexicalAnalyzer::LETTER)) {
        list<char> *l = fceVarsRem();
        l->push_front(lex.getLetter());
        return l;
    }
    else
        throw syntaxExc();
}

FormulaSpec *Parser::fceBody() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::CMD)) {
        switch (lex.getCommand()) {
        case SUM:
            return sum();
        case PROD:
            return prod();
        default:
            throw commandExc();
        }
    }
    else
        throw syntaxExc();
}

FormulaSpec *Parser::sum() throw(ShellExc)
{
    readToken(); // sum

    FormulaSpec *spec = new FormulaSpec;
    try {
        spec->f = mTerms();
        spec->d = sumRem();
    }
    catch (ShellExc &exc) {
        delete spec->f;
        delete spec;
        throw exc;
    }
}

set<int> *Parser::sumRem() throw(ShellExc)
{
    cmpre(LexicalAnalyzer::PLUS);
    cmpe(LexicalAnalyzer::CMD);
    if (lex.getCommand() != LexicalAnalyzer::SUM)
        throw commandExc();
    readToken(); // sum
    return dTerms();
}

FormulaSpec *Parser::prod() throw(ShellExc)
{
    readToken(); // prod

    FormulaSpec *spec = new FormulaSpec;
    try {
        spec->r = mTerms();
        spec->d = prodRem();
    }
    catch (ShellExc &exc) {
        delete spec->r;
        delete spec;
        throw exc;
    }
}

set<int> *Parser::prodRem() throw(ShellExc)
{
    cmpre(LexicalAnalyzer::PLUS);
    cmpe(LexicalAnalyzer::CMD);
    if (lex.getCommand() != LexicalAnalyzer::PROD)
        throw commandExc();
    readToken(); // prod
    return dTerms();
}

set<int> *Parser::mTerms() throw(ShellExc)
{
    cmpe(LexicalAnalyzer::LETTER);
    if (lex.getLetter() != 'm')
        throw LexicalExc(lex.getLetter(), lex.getCol());
    readToken();
    return fceIndexes();
}

set<int> *Parser::dTerms() throw(ShellExc)
{
    cmpe(LexicalAnalyzer::LETTER);
    if (lex.getLetter() != 'd')
        throw LexicalExc(lex.getLetter(), lex.getCol());
    readToken();
    return fceIndexes();
}

set<int> *Parser::fceIndexes() throw(ShellExc)
{
    cmpre(LexicalAnalyzer::LPAR);
    cmpe(LexicalAnalyzer::NUMBER);
    set<int> *s = fceIndexesRem();
    s->insert(lex.getNumber());
    try {
        cmpre(LexicalAnalyzer::RPAR);
    }
    catch (ShellExc &exc) {
        delete s;
        throw exc;
    }
    return s;
}

set<int> *Parser::fceIndexesRem() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::RPAR))
        return new set<int>;
    else if (cmpr(LexicalAnalyzer::COMMA) && cmpr(LexicalAnalyzer::NUMBER)) {
        set<int> *s = fceIndexesRem();
        s->insert(lex.getNumber());
        return s;
    }
    else
        throw syntaxExc();
}

inline bool Parser::cmp(LexicalAnalyzer::Token tok)
{
    return tok == lex.getToken();
}

bool Parser::cmpr(LexicalAnalyzer::Token tok)
{
    if (cmp(tok)) {
        lex.readToken();
        return true;
    }
    else
        return false;
}

inline bool Parser::cmpe(LexicalAnalyzer::Token tok) throw(ShellExc)
{
    if (!cmp(rok))
        throw syntaxExc();
}

inline bool Parser::cmpre(LexicalAnalyzer::Token tok) throw(ShellExc)
{
    if (!cmpr(rok))
        throw syntaxExc();
}

inline SyntaxExc Parser::syntaxExc()
{
    return SyntaxExc(lex.getTokenName(), lex.getCol());
}

inline CommandExc Parser::commandExc()
{
    return CommandExc(lex.getCommandName(), CommandExc::CONTEXT, lex.getCol());
}

