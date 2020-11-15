#ifndef _STATEMENT_H_
#define _STATEMENT_H_

#include "Row.h"

enum class StatementType : unsigned int
{
    EXIT, INSERT, SELECT
};

class Statement
{
public:
    void set_type(StatementType type);
    StatementType get_type();
    void set_row_to_insert(Row *row);
    Row *get_row_to_insert();
private:
    StatementType type;
    Row *row_to_insert;
};

#endif /* _STATEMENT_H_ */
