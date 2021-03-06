/*
 * parser.h - parser of konsole's commands
 * created date: 10/28/2009
 *
 * Copyright (C) 2010 Jakub Zelenka.
 *
 * Bmin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * Bmin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bmin; if not, see <http://www.gnu.org/licenses/>.
 */

#include "parser.h"
#include "lexicalanalyzer.h"
#include "plaformat.h"
#include "shellexc.h"


// kernel
#include "kernel.h"
#include "kernelexc.h"
#include "formula.h"
#include "term.h"
#include "outputvalue.h"
#include "literalvalue.h"

#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <set>
#include <algorithm>

using namespace std;

Parser::Parser()
{
    kernel = Kernel::instance();
}

string Parser::termToString(const Term &term, const vector<char> &vars, PrintForm form)
{
    ostringstream oss;
    bool allMissing = true;
    if (form == PF_SOP || form == PF_PROD) {
        for (unsigned i = 0; i < term.getSize(); i++) {
            if (!term[i].isMissing()) {
                allMissing = false;
                oss << vars[i];
                if (term[i].isZero())
                    oss << '\''; // negation
            }
        }
        if (allMissing)
            oss << '1';
    }
    else if (form == PF_POS || form == PF_SUM) {
        bool first = true;
        for (unsigned i = 0; i < term.getSize(); i++) {
            if (!term[i].isMissing()) {
                allMissing = false;
                if (first)
                    first = false;
                else
                    oss << " + ";
                oss << vars[i];
                if (term[i].isOne())
                    oss << '\''; // negation
            }
        }
        if (allMissing)
            oss << '0';
    }

    return oss.str();
}

string Parser::formulaToString(PrintForm form, Formula *f)
{
    if (!f)
        return "";

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

    if (form == PF_SUM || form == PF_PROD) {
        vector<int> idx;
        if (form == PF_SUM) {
            f->getTermsIdx(OutputValue::ONE, idx);
            oss << CMD_SUM << ' ';
        }
        else {
            f->getTermsIdx(OutputValue::ZERO, idx);
            oss << CMD_PROD << ' ';
        }
        oss << FCE_MINTERM << SYM_LPAR;
        sort(idx.begin(), idx.end());
        for (unsigned i = 0; i < idx.size(); i++) {
            if (i != 0)
                oss << SYM_COMMA;
            oss << idx[i];
        }
        oss << SYM_RPAR;
        idx = f->getTermsIdx(OutputValue::DC, idx);
        if (!idx.empty()) {
            if (form == PF_SUM) {
                oss << ' ' << SYM_PLUS << ' ';
                oss << CMD_SUM << ' ';
            }
            else {
                oss << ' ' << SYM_MULT << ' ';
                oss << CMD_PROD << ' ';
            }
            oss << FCE_DC << SYM_LPAR;
            sort(idx.begin(), idx.end());
            for (unsigned i = 0; i < idx.size(); i++) {
                if (i != 0)
                    oss << SYM_COMMA;
                oss << idx[i];
            }
            oss << SYM_RPAR;
        }
    }
    else if (f->isContradiction())
            oss << " 0";
    else if (f->isTautology())
            oss << " 1";
    else {
        f->itInit();
        bool first = true;
        while (f->itHasNext()) {
            if (first)
                first = false;
            else if (form == PF_SOP)
                oss << ' ' << SYM_PLUS << ' ';

            Term t = f->itNext();
            bool bracket = (form == PF_POS && t.getSize(false) > 1);
            if (bracket)
                oss << '(';
            oss << termToString(t, vars, form);
            if (bracket)
                oss << ')';
        }
    }
    
    return oss.str();
}

// parsing PLA file
void Parser::parsePLA(const string &fileName)
{
    ifstream fin;
    fin.open(fileName.c_str());
    try {
        if (fin.is_open()) {
            PLAFormat pla(fin);
            kernel->setFormulas(pla.formulas);
        }
        else
            throw FileExc(fileName);
    }
    catch (exception &exc) {
        kernel->error(exc);
    }
}

// creating PLA file
void Parser::createPLA(const string &fileName)
{
    if (!kernel->hasFormula())
        return;

    ofstream fout;
    fout.open(fileName.c_str());
    try {
        if (fout.is_open()) {
            bool pos = kernel->getRepre() == Formula::REP_POS;
            if (pos)
                kernel->setRepre(Formula::REP_SOP);

            Formula *formula = kernel->hasMinimizedFormula()?
                               kernel->getMinimizedFormula(): kernel->getFormula();
            PLAFormat::create(fout, formula);

            if (pos)
                kernel->setRepre(Formula::REP_POS);
        }
        else
            throw FileExc(fileName);
    }
    catch (exception &exc) {
        kernel->error(exc);
    }
}

// Parse Command Line

void Parser::parse(const std::string &str)
{
    lex.analyze(str);

    try {
        program();
        cmpre(LexicalAnalyzer::END);
    }
    catch (exception &exc) {
        kernel->error(exc);
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
    case LexicalAnalyzer::HELP:
        kernel->help();
        break;
    case LexicalAnalyzer::QM:
        kernel->setAlgorithm(Kernel::QM);
        break;
    case LexicalAnalyzer::ESPRESSO:
        kernel->setAlgorithm(Kernel::ESPRESSO);
        break;
    case LexicalAnalyzer::SOP:
        kernel->setRepre(Formula::REP_SOP);
        break;
    case LexicalAnalyzer::POS:
        kernel->setRepre(Formula::REP_POS);
        break;
    case LexicalAnalyzer::LOAD:
        readToken();
        parsePLA(filePath());
        break;
    case LexicalAnalyzer::SAVE:
        readToken();
        createPLA(filePath());
        break;
    case LexicalAnalyzer::SHOW:
        readToken();
        showArg();
        break;
    default:
        throw commandExc();
    }
    readToken();
}

void Parser::showArg() throw(ShellExc)
{
    if (cmp(LexicalAnalyzer::END))
        kernel->showFce();
    else if (cmp(LexicalAnalyzer::LETTER))
        kernel->showFce(lex.getLetter());
    else {
        cmpe(LexicalAnalyzer::CMD);
        switch (lex.getCommand()) {
        case LexicalAnalyzer::QM:
            kernel->showQm();
            break;
        case LexicalAnalyzer::ESPRESSO:
            kernel->showEspresso();
            break;
        case LexicalAnalyzer::KMAP:
            kernel->showKMap();
            break;
        case LexicalAnalyzer::CUBE:
            kernel->showCube();
            break;
        default:
            throw commandExc();
        }
    }
}

string Parser::filePath() throw(ShellExc)
{
    cmpe(LexicalAnalyzer::STRING);
    return lex.getWord();
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
        spec->sop = true;
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
        spec->sop = false;
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


