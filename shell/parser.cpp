#include "parser.h"
#include "lexicalanalyzer.h"
#include "shellexc.h"

#include "kernel/kernel.h"
#include "kernel/kernelexc.h"
#include "kernel/formula.h"
#include "kernel/term.h"
#include "kernel/outputvalue.h"
#include "kernel/literalvalue.h"

#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include <set>

using namespace std;

Parser::Parser()
{
    kernel = Kernel::instance();
}

string Parser::termToString(Term &term, vector<char> vars, PrintForm form)
{
    ostringstream oss;
    if (form == PF_PROD || form == PF_SOP) {
        for (int i = 0; i < term.getSize(); i++) {
            if (!term[i].isMissing()) {
                oss << vars[i];
                if (term[i].isZero())
                    oss << '\''; // negation
            }
        }
    }
    // TODO prod

    return oss.str();
}

string Parser::formulaToString(PrintForm form, Formula *f)
{
    if (!f) {
        f = (form == PF_SUM || form == PF_PROD)?
            kernel->getFormula(): kernel->getMinimizedFormula();
        if (!f)
            return "unknown";
    }

    ostringstream oss;
    // variables
    oss << f->getName() << SYM_LPAR;
    vector<char> vars = f->getVars();
    for (unsigned i = vars.size(); i > 0; i--) {
        if (i != vars.size())
            oss << SYM_COMMA;
        oss << vars[i - 1];
    }
    oss << SYM_RPAR << ' ' << SYM_ASSIGN << ' ';

    if (form == PF_SUM) {
        vector<int> idx;
        oss << CMD_SUM << ' ';
        oss << FCE_MINTERM << SYM_LPAR;
        f->getTermsIdx(idx, OutputValue::ONE);
        for (unsigned i = 0; i < idx.size(); i++) {
            if (i != 0)
                oss << SYM_COMMA;
            oss << idx[i];
        }
        oss << SYM_RPAR;
        idx = f->getTermsIdx(idx, OutputValue::DC);
        if (!idx.empty()) {
            oss << ' ' << SYM_PLUS << ' ';
            oss << CMD_SUM << ' ';
            oss << FCE_DC << SYM_LPAR;
            for (unsigned i = 0; i < idx.size(); i++) {
                if (i != 0)
                    oss << SYM_COMMA;
                oss << idx[i];
            }
            oss << SYM_RPAR;
        }
    }
    else if (f->getSize() == 0)
            oss << " 0";
    else {
        f->itInit();
        bool first = true;
        while (f->itHasNext()) {
            if (first)
                first = false;
            else
                oss << ' ' << SYM_PLUS << ' ';
            oss << termToString(f->itNext(), vars, PF_PROD);
        }
    }
    
    return oss.str();
}


void Parser::parse(std::string str)
{
    lex.analyze(str);

    try {
        program();
        cmpre(LexicalAnalyzer::END);
    }
    catch (exception &exc) {
        cout << exc.what() << endl;
    }
}

void Parser::program() throw(ShellExc, KernelExc)
{
    readToken();

    if (cmp(LexicalAnalyzer::CMD))
        command();
    else if (cmp(LexicalAnalyzer::LETTER))
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
    readToken();
}

void Parser::fceDef() throw(ShellExc, KernelExc)
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
    vector<char> *v = fceVars();
    readToken(); // RPAR - assured by fceVarsRem

    return new FormulaDecl(v, name);
}

char Parser::fceName() throw(ShellExc)
{
    char name = lex.getLetter();
    readToken();
    return name;
}

vector<char> *Parser::fceVars() throw(ShellExc)
{
    cmpe(LexicalAnalyzer::LETTER);
    char var = lex.getLetter();
    readToken();
    vector<char> *v = fceVarsRem();
    v->push_back(var);
    return v;
}

vector<char> *Parser::fceVarsRem() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::RPAR))
        return new vector<char>;
    else if (cmpr(LexicalAnalyzer::COMMA) && cmp(LexicalAnalyzer::LETTER)) {
        char var = lex.getLetter();
        readToken();
        vector<char> *v = fceVarsRem();
        v->push_back(var);
        return v;
    }
    else
        throw syntaxExc();
}

FormulaSpec *Parser::fceBody() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::CMD)) {
        switch (lex.getCommand()) {
        case LexicalAnalyzer::SUM:
            return sum();
        case LexicalAnalyzer::PROD:
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
        if (cmpr(LexicalAnalyzer::PLUS))
            spec->d = sumRem();
    }
    catch (ShellExc &exc) {
        delete spec;
        throw;
    }
    return spec;
}

set<int> *Parser::sumRem() throw(ShellExc)
{
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
        if (cmpr(LexicalAnalyzer::PLUS))
            spec->d = prodRem();
    }
    catch (ShellExc &exc) {
        delete spec;
        throw;
    }
    return spec;
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
    if (lex.getLetter() != FCE_MINTERM)
        throw LexicalExc(lex.getLetter(), lex.getCol());
    readToken(); // letter
    return fceArgs();
}

set<int> *Parser::dTerms() throw(ShellExc)
{
    cmpe(LexicalAnalyzer::LETTER);
    if (lex.getLetter() != FCE_DC)
        throw LexicalExc(lex.getLetter(), lex.getCol());
    readToken(); // letter
    return fceArgs();
}

set<int> *Parser::fceArgs() throw(ShellExc)
{
    cmpre(LexicalAnalyzer::LPAR);
    set<int> *s = fceIndexes();
    readToken(); // RPAR - assured by fceIndexes and fceIndexesRem
    return s;
}

set<int> *Parser::fceIndexes() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::RPAR))
        return 0;
    cmpe(LexicalAnalyzer::NUMBER);
    int num = lex.getNumber();
    readToken();
    set<int> *s = fceIndexesRem();
    s->insert(num);
    return s;
}

set<int> *Parser::fceIndexesRem() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::RPAR))
        return new set<int>;
    else if (cmpr(LexicalAnalyzer::COMMA) && cmp(LexicalAnalyzer::NUMBER)) {
        int num = lex.getNumber();
        readToken();
        set<int> *s = fceIndexesRem();
        s->insert(num);
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
        readToken();
        return true;
    }
    else
        return false;
}

inline bool Parser::cmpe(LexicalAnalyzer::Token tok) throw(ShellExc)
{
    if (!cmp(tok))
        throw syntaxExc();
    return true;
}

inline bool Parser::cmpre(LexicalAnalyzer::Token tok) throw(ShellExc)
{
    if (!cmpr(tok))
        throw syntaxExc();
    return true;
}

inline SyntaxExc Parser::syntaxExc()
{
    return SyntaxExc(lex.getTokenName(), lex.getCol());
}

inline CommandExc Parser::commandExc()
{
    return CommandExc(lex.getCommandName(), CommandExc::CONTEXT, lex.getCol());
}

