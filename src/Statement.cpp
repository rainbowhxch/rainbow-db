#include "../include/Statement.h"

void Statement::set_type(StatementType type)
{
    this->type = type;
}

StatementType Statement::get_type()
{
    return type;
}

void Statement::set_row_to_insert(Row *row)
{
    this->row_to_insert = row;
}

Row *Statement::get_row_to_insert()
{
    return this->row_to_insert;
}
