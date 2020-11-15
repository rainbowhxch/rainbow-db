#ifndef  _PARSER_H_
#define _PARSER_H_

#include <string>
#include <iostream>
#include "Statement.h"

using std::string;

class Parser
{
#define WORD_HELLO "rainbow-db> "
#define WORD_BYE "BYE"

public:
    void read_input();
    void print_prompt();
    void print_bye();
    Statement *prepare_statement();
private:
    string input_buffer;
};

#endif /* _PARSER_H_ */
