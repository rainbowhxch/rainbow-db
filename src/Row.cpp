#include "../include/Row.h"

#include <cstring>

constexpr uint32_t Row::SIZE_OF_ROW;

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
    memcpy(destination, this, sizeof(*this));
}

void Row::deserialize_row(uint8_t *source)
{
    memcpy(this, source, sizeof(*this));
}

uint32_t Row::get_id()
{
    return id;
}
