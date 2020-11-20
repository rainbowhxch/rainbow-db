#ifndef _RAINBOWDB_H_
#define _RAINBOWDB_H_

#include "Parser.h"
#include "Actuator.h"

class RainbowDB
{
public:
    RainbowDB(const char *filename);
    ~RainbowDB();
    void db_loop();

private:
    Parser *parser;
    Actuator *actuator;
};

#endif /* _RAINBOWDB_H_ */
