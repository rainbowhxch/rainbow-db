#ifndef _PAGER_H_
#define _PAGER_H_

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include "Row.h"

#define MAX_PAGES_NUM 1024
#define SIZE_OF_PAGE 4096

class Pager
{
public:
    Pager(const char *filename);
    uint8_t *get_page(uint32_t page_num);
    void page_flush_and_close();
    void set_num_of_pages(uint32_t num_of_pages);
    uint32_t get_num_of_pages();
    void set_num_of_row_in_last_page(uint32_t num_of_rows_in_last_page);
    uint32_t get_num_of_rows_in_last_page();
private:
    FILE *db_fd;
    uint32_t num_of_pages;
    uint32_t num_of_rows_in_last_page;
    uint8_t *pages[MAX_PAGES_NUM];
};

#endif /* _PAGER_H_ */
