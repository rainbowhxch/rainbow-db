#include "../include/Actuator.h"

Actuator::Actuator(string filename)
{
    b_plus_tree = new BPlusTree(filename);
}

HandleResult Actuator::exec_statement(Statement *statement)
{
    switch (statement->get_type()) {
        case StatementType::EXIT:
            {
                return HandleResult::EXIT;
            }
        case StatementType::INSERT:
            {
                return exec_insert(statement);
                break;
            }
        case StatementType::SELECT:
            {
                return exec_select(statement);
                break;
            }
        case StatementType::UPDATE:
            {
                return exec_update(statement);
            }
        case StatementType::REMOVE:
            {
                return exec_remove(statement);
            }
    }
    return HandleResult::SUCCESS;
}

HandleResult Actuator::exec_insert(Statement *statement)
{
    uint32_t key = statement->get_row()->get_id();
    bool result = b_plus_tree->insert(key, statement->get_row());
    if (result == false) {
        return HandleResult::DUPLICATE_KEY;
    }

    cout << "insert success" << endl;
    return HandleResult::SUCCESS;
}

HandleResult Actuator::exec_select(Statement *statement)
{
    uint32_t key = statement->get_row()->get_id();
    bool result = b_plus_tree->search(key, statement->get_row());
    if (result == false) {
        return HandleResult::NO_SUCH_KEY;
    }
    statement->get_row()->print_row();

    return HandleResult::SUCCESS;
}


HandleResult Actuator::exec_update(Statement *statement)
{
    uint32_t key = statement->get_row()->get_id();
    bool result = b_plus_tree->update(key, statement->get_row());
    if (result == false) {
        return HandleResult::NO_SUCH_KEY;
    }

    cout << "update success" << endl;
    return HandleResult::SUCCESS;
}

HandleResult Actuator::exec_remove(Statement *statement)
{
    uint32_t key = statement->get_row()->get_id();
    bool result = b_plus_tree->remove(key);
    if (result == false) {
        return HandleResult::NO_SUCH_KEY;
    }

    cout << "remove success" << endl;
    return HandleResult::SUCCESS;
}

Actuator::~Actuator()
{
    delete b_plus_tree;
}
