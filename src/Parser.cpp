#include "../include/Parser.h"

#include <cstring>

constexpr char Parser::WORD_HELLO[];
constexpr char Parser::WORD_BYE[];

void Parser::read_input()
{
    std::getline(std::cin, input_buffer);
}

void Parser::print_prompt()
{
    std::cout << WORD_HELLO;
}

void Parser::print_bye()
{
    std::cout << WORD_BYE;
}

Statement *Parser::prepare_statement()
{
    Statement *statement = nullptr;
    int id;
    char name[256], email[256];
    if (input_buffer.front() == '.') {
        if (input_buffer == ".exit") {
            statement = new Statement(StatementType::EXIT, Row());
        }
    } else {
        if (strncmp(input_buffer.c_str(), "insert", strlen("insert")) == 0) {
            sscanf(input_buffer.c_str(), "insert %d %s %s into user", &id, name, email);
            statement = new Statement(StatementType::INSERT, Row(id, name, email));
        } else if (strncmp(input_buffer.c_str(), "select", strlen("select")) == 0) {
            sscanf(input_buffer.c_str(), "select * from user where id=%d", &id);
            statement = new Statement(StatementType::SELECT, Row(id, name, email));
        } else if (strncmp(input_buffer.c_str(), "update", strlen("update")) == 0) {
            sscanf(input_buffer.c_str(), "update user %s %s where id=%d", name, email, &id);
            statement = new Statement(StatementType::UPDATE, Row(id, name, email));
        } else if (strncmp(input_buffer.c_str(), "delete", strlen("delete")) == 0) {
            sscanf(input_buffer.c_str(), "delete from user where id=%d", &id);
            statement = new Statement(StatementType::REMOVE, Row(id, name, email));
        }
    }

    return statement;
}
