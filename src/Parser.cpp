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
            statement->set_row_to_insert(nullptr);
        }
    } else {
        if (strncmp(input_buffer.c_str(), "insert", strlen("insert")) == 0) {
            statement->set_type(StatementType::INSERT);

            size_t pos = 0;
            std::string delimiter = " ";
            int id;
            const char *name, *email;
            if ((pos = input_buffer.find(delimiter)) != string::npos)
                input_buffer.erase(0, pos + delimiter.length());
            if ((pos = input_buffer.find(delimiter)) != string::npos)
                id = std::stoi(input_buffer.substr(0, pos));
            input_buffer.erase(0, pos + delimiter.length());
            if ((pos = input_buffer.find(delimiter)) != string::npos)
                name = input_buffer.substr(0, pos).c_str();
            input_buffer.erase(0, pos + delimiter.length());
            email = input_buffer.c_str();
            statement->set_row_to_insert(new Row(id, name, email));
        } else if (strncmp(input_buffer.c_str(), "select", strlen("select")) == 0) {
            statement->set_type(StatementType::SELECT);
        }
    }

    return statement;
}
