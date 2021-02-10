#include "../include/RainbowDB.h"

RainbowDB::RainbowDB(const char *filename)
{
    actuator = new Actuator(filename);
    parser = new Parser();
}

void RainbowDB::db_loop()
{
    bool is_continue = true;

    while (is_continue) {
        parser->print_prompt();
        parser->read_input();
        Statement *statement = parser->prepare_statement();
        switch (actuator->exec_statement(statement)) {
            case HandleResult::EXIT:
                {
                    is_continue = false;
                    break;
                }
            case HandleResult::SUCCESS:
                {
                    break;
                }
            case HandleResult::NO_SUCH_KEY:
                {
                    std::cout << "no such key" << std::endl;
                    break;
                }
            default:
                {
                    std::cout << "wrong command" << std::endl;
                    break;
                }
        }
        delete statement;
    }
    parser->print_bye();
}

RainbowDB::~RainbowDB()
{
    delete parser;
    delete actuator;
}
