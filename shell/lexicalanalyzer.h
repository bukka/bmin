#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <istream>
#include <string>

class LexicalAnalyzer
{
private:
    static const char * const SYMBOL_SUM =  "sum";
    static const char * const SYMBOL_PROD = "prod";

    enum InputType { ALPHA, DIGIT, BLANK, OTHER, EOI };

    InputType readInput();

    bool readNext;

    InputType inputType;
    char inputChar;
    int inputDigit;

    int linePosition;

    std::istream is;

    char letter;
    int wordnumber;
    std::string word;

public:
    enum Token {
        NUMBER,
        LETTER,
        WORD,
        L_PAR,
        R_PAR,
        PLUS,
        MULT,
        ASSIGN,
        COMMA,
        SUM,
        PROD,
        END
    };

    LexicalAnalyzer();

    void analyze(std::string & str);
    void analyze(std::istream & s);

    Token readToken(bool read = true);

    inline int getNumber() { return number; }
    inline char getLetter() { return letter; }
    inline std::string getWord() { return word; }

};

#endif // LEXICALANALYZER_H
