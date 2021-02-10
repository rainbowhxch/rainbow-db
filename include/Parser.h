#ifndef  _PARSER_H_
#define _PARSER_H_

#include <string>

#include "Statement.h"

using std::string;

class Parser
{
public:
    void read_input();
    void print_prompt();
    void print_bye();
    Statement *prepare_statement();
private:
    string input_buffer;
    static constexpr char WORD_HELLO[] = "rainbow-db> ";
    static constexpr char WORD_BYE[] = "BYE";
};

#endif /* _PARSER_H_ */
