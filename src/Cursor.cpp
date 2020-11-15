#include "../include/Cursor.h"

Cursor::Cursor(uint32_t page_num, uint32_t row_num) : page_num(page_num), row_num(row_num)
{}

void Cursor::set_page_num(uint32_t page_num)
{
    this->page_num = page_num;
}

uint32_t Cursor::get_page_num()
{
    return this->page_num;
}

void Cursor::set_row_num(uint32_t row_num)
{
    this->row_num = row_num;
}

uint32_t Cursor::get_row_num()
{
    return this->row_num;
}

void Cursor::advance()
{
    row_num++;
    if (row_num == ROWS_OF_PAGE) {
        page_num++;
        row_num = 0;
    }
}
