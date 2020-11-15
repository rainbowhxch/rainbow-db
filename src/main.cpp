#include "../include/RainbowDB.h"

int main(int argc, char *argv[])
{
    RainbowDB *rainbow_db = new RainbowDB("rainbow.db");
    rainbow_db->db_loop();

    return 0;
}
