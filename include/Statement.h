#ifndef _STATEMENT_H_
#define _STATEMENT_H_

#include "Row.h"

enum class StatementType : unsigned int
{
    EXIT = 0, INSERT, SELECT, UPDATE, REMOVE
};

class Statement
{
public:
    void set_type(StatementType type);
    StatementType get_type();
    void set_row(Row *row);
    Row *get_row();
private:
    StatementType type;
    Row *row;
};

#endif /* _STATEMENT_H_ */
