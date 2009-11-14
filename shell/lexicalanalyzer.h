#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <istream>
#include <string>

class ShellExc;

class LexicalAnalyzer
{
public:
    enum Token {
        NUMBER,
        LETTER,
        LPAR,
        RPAR,
        PLUS,
        MULT,
        ASSIGN,
        COMMA,
        CMD,
        END
    };

    enum Command {
        SUM,
        PROD,
        MINIMIZE,
        EXIT
    };

    static const char * const TN_NUMBER  = "NUMBER";
    static const char * const TN_LETTER  = "LETTER";
    static const char * const TN_LPAR    = "LPAR";
    static const char * const TN_RPAR    = "RPAR";
    static const char * const TN_PLUS    = "PLUS";
    static const char * const TN_MULT    = "MULT";
    static const char * const TN_ASSIGN  = "ASSIGN";
    static const char * const TN_COMMA   = "COMMA";
    static const char * const TN_CMD     = "CMD";
    static const char * const TN_END     = "END";

    static const char SYM_LPAR         = '(';
    static const char SYM_RPAR         = ')';
    static const char SYM_PLUS         = '+';
    static const char SYM_MULT         = '*';
    static const char SYM_ASSIGN       = '=';
    static const char SYM_COMMA        = ',';

    static const char * const CMD_SUM      = "sum";
    static const char * const CMD_PROD     = "prod";
    static const char * const CMD_MINIMIZE = "minimize";
    static const char * const CMD_EXIT     = "exit";

    LexicalAnalyzer();

    void analyze(std::string &str);
    void analyze(std::istream &s);

    Token readToken() throw(ShellExc);
    inline Token getToken() const { return token; }
    inline const char *getTokenName() const { return getTokenName(token); }
    const char *getTokenName(Token tok) const;
    inline Command getCommand() const { return command; }
    inline const char *getCommandName() const { return getCommandName(command); }
    const char *getCommandName(Command cmd);



    inline int getNumber() const { return number; }
    inline char getLetter() const { return letter; }
    inline std::string getWord() const { return word; }

    inline int getCol() const { return col; }

private:

    enum InputType { ALPHA, DIGIT, BLANK, OTHER, EOI };


    InputType readInput();
    // case insensitive comparing
    bool strcmpi(const std::string &str, const char *cmd);
    bool isCommand(const std::string &str);

    bool readNext;

    InputType inputType;
    char inputChar;
    int inputDigit;

    Token token;
    Command command;

    int col;

    std::istream is;

    char letter;
    int number;
    std::string word;

};

#endif // LEXICALANALYZER_H
