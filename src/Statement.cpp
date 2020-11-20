#include "../include/Statement.h"

void Statement::set_type(StatementType type)
{
    this->type = type;
}

StatementType Statement::get_type()
{
    return type;
}

void Statement::set_row(Row *row)
{
    this->row = row;
}

Row *Statement::get_row()
{
    return this->row;
}
