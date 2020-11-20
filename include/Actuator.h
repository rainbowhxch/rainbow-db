#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_

#include <iostream>
#include <cstdlib>
#include "Statement.h"
#include "BPlusTree.h"

using std::cout;
using std::endl;

enum class HandleResult : unsigned int
{
    SUCCESS = 0, EXIT, DUPLICATE_KEY, NO_SUCH_KEY
};

class Actuator
{
public:
    Actuator(string filename);
    ~Actuator();
    HandleResult exec_statement(Statement *statement);
private:
    BPlusTree *b_plus_tree;
    HandleResult exec_insert(Statement *statement);
    HandleResult exec_select(Statement *statement);
    HandleResult exec_update(Statement *statement);
    HandleResult exec_remove(Statement *statement);
};

#endif /* _ACTUATOR_H_ */
