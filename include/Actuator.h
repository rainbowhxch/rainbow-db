#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_

#include <cstdlib>
#include "Statement.h"
#include "Pager.h"
#include "Cursor.h"

enum class HandleResult : unsigned int
{
    SUCCESS = 0, EXIT
};

class Actuator
{
public:
    Actuator(const char *filename);
    ~Actuator();
    void set_cursor(Cursor *cursor);
    void set_pager(Pager *pager);
    HandleResult exec_statement(Statement *statement);

private:
    Pager *pager;
    Cursor *cursor;
};

#endif /* _ACTUATOR_H_ */
