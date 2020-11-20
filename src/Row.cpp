#include "../include/Row.h"

Row::Row() = default;

Row::Row(uint32_t id, const char *name, const char *email)
{
    this->id = id;
    strcpy(this->name, name);
    strcpy(this->email, email);
}

void Row::print_row() {
    std::cout << "----------------------------------"  << std::endl;
    std::cout << "id:      \t" << id                   << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++"  << std::endl;
    std::cout << "name:    \t" << name                 << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++"  << std::endl;
    std::cout << "email:   \t" << email                << std::endl;
    std::cout << "----------------------------------"  << std::endl;
}

void Row::serialize_row(uint8_t *destination)
{
    memcpy(destination+OFFSET_OF_ID, &id, SIZE_OF_ID);
    memcpy(destination+OFFSET_OF_NAME, name, SIZE_OF_NAME);
    memcpy(destination+OFFSET_OF_EMAIL, email, SIZE_OF_EMAIL);
}

void Row::deserialize_row(uint8_t *source)
{
    memcpy(&id, source+OFFSET_OF_ID, SIZE_OF_ID);
    memcpy(name, source+OFFSET_OF_NAME, SIZE_OF_NAME);
    memcpy(email, source+OFFSET_OF_EMAIL, SIZE_OF_EMAIL);
}

uint32_t Row::get_id()
{
    return id;
}
