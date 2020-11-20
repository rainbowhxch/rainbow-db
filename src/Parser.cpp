#include "../include/Parser.h"

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
    Statement *statement = new Statement();
    if (input_buffer.front() == '.') {
        if (input_buffer == ".exit") {
            statement->set_type(StatementType::EXIT);
            statement->set_row(nullptr);
        }
    } else {
        if (strncmp(input_buffer.c_str(), "insert", strlen("insert")) == 0) {
            statement->set_type(StatementType::INSERT);
            int id;
            char name[256], email[256];
            sscanf(input_buffer.c_str(), "insert %d %s %s into user", &id, name, email);
            statement->set_row(new Row(id, name, email));
        } else if (strncmp(input_buffer.c_str(), "select", strlen("select")) == 0) {
            statement->set_type(StatementType::SELECT);
            int id;
            char name[256], email[256];
            sscanf(input_buffer.c_str(), "select * from user where id=%d", &id);
            statement->set_row(new Row(id, name, email));
        } else if (strncmp(input_buffer.c_str(), "update", strlen("update")) == 0) {
            statement->set_type(StatementType::UPDATE);
            int id;
            char name[256], email[256];
            sscanf(input_buffer.c_str(), "update user %s %s where id=%d", name, email, &id);
            statement->set_row(new Row(id, name, email));
        } else if (strncmp(input_buffer.c_str(), "delete", strlen("delete")) == 0) {
            statement->set_type(StatementType::REMOVE);
            int id;
            char name[256], email[256];
            sscanf(input_buffer.c_str(), "delete from user where id=%d", &id);
            statement->set_row(new Row(id, name, email));
        }
    }

    return statement;
}
