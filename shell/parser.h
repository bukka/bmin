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

#ifndef PARSER_H
#define PARSER_H

#include "lexicalanalyzer.h"
#include "shellexc.h"
// kernel
#include "kernelexc.h"

#include <istream>
#include <string>
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
        PF_SUM,  // sum of minterms' indexes
        PF_PROD, // product of minterms's indexes
        PF_SOP,  // sum of products
        PF_POS   // product of sums
    };

    static std::string termToString(const Term &term, const std::vector<char> &vars, PrintForm form);
    static std::string formulaToString(PrintForm form, Formula *formula);

    Parser();

    void parse(std::string str);

private:
    Kernel *kernel;
    LexicalAnalyzer lex;

    void program() throw(ShellExc, KernelExc);
    void command() throw(ShellExc);
    void showArg() throw(ShellExc);
    void fceDef() throw(ShellExc, KernelExc);
    FormulaDecl *fceDecl() throw(ShellExc);
    char fceName() throw(ShellExc);
    std::vector<char> *fceVars() throw(ShellExc);
    std::vector<char> *fceVarsRem() throw(ShellExc);
    FormulaSpec *fceBody() throw(ShellExc);
    FormulaSpec *sum() throw(ShellExc);
    std::set<int> *sumRem() throw(ShellExc);
    FormulaSpec *prod() throw(ShellExc);
    std::set<int> *prodRem() throw(ShellExc);
    std::set<int> *mTerms() throw(ShellExc);
    std::set<int> *dTerms() throw(ShellExc);
    std::set<int> *fceArgs() throw(ShellExc);
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
