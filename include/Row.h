#ifndef _ROW_H_
#define _ROW_H_

#include <iostream>
#include <cstring>

class Row
{
#define SIZE_OF_ID sizeof(uint32_t)
#define SIZE_OF_NAME 256
#define SIZE_OF_EMAIL 256

public:
    Row();
    Row(uint32_t id, const char *name, const char *email);
    uint32_t get_id();
    void print_row();
    void serialize_row(uint8_t *destination);
    void deserialize_row(uint8_t *source);
    static const uint32_t SIZE_OF_ROW = SIZE_OF_ID + SIZE_OF_NAME + SIZE_OF_EMAIL;
private:
    uint32_t id;
    char name[SIZE_OF_NAME];
    char email[SIZE_OF_EMAIL];
    static const uint32_t OFFSET_OF_ID = 0;
    static const uint32_t OFFSET_OF_NAME = OFFSET_OF_ID + SIZE_OF_ID;
    static const uint32_t OFFSET_OF_EMAIL = OFFSET_OF_NAME + SIZE_OF_NAME;
};

#endif /* _ROW_H_ */
