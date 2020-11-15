#ifndef _CURSOR_H_
#define _CURSOR_H_

#include <cstdint>
#include "Pager.h"
#include "Row.h"

class Cursor
{
public:
    Cursor(uint32_t page_num, uint32_t row_num);
    void advance();
    void set_page_num(uint32_t page_num);
    uint32_t get_page_num();
    uint32_t get_row_num();
    void set_row_num(uint32_t row_num);
    static const int ROWS_OF_PAGE = SIZE_OF_PAGE / Row::SIZE_OF_ROW;
private:
    uint32_t page_num;
    uint32_t row_num;
};

#endif /* _CURSOR_H_ */
