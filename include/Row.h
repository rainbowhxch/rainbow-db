#ifndef _ROW_H_
#define _ROW_H_

#include <iostream>

class Row {
public:
    Row() = default;
    Row(uint32_t id, const char *name, const char *email);
    uint32_t get_id();
    void print_row();
    void serialize_row(uint8_t *destination);
    void deserialize_row(uint8_t *source);
    static constexpr uint32_t SIZE_OF_ID = sizeof(uint32_t);
    static constexpr uint32_t SIZE_OF_NAME = 256;
    static constexpr uint32_t SIZE_OF_EMAIL = 256;
    static constexpr uint32_t SIZE_OF_ROW = SIZE_OF_ID + SIZE_OF_NAME + SIZE_OF_EMAIL;
private:
    uint32_t id;
    char name[SIZE_OF_NAME];
    char email[SIZE_OF_EMAIL];
};

#endif /* _ROW_H_ */
