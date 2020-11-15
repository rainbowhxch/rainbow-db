#include "../include/Pager.h"

Pager::Pager(const char *filename)
{
    db_fd = fopen(filename, "r+");
    if (db_fd == nullptr)
        db_fd = fopen(filename, "w+");
    fseek(db_fd, 0, SEEK_END);
    uint32_t db_length = ftell(db_fd);

    num_of_pages = db_length / SIZE_OF_PAGE;
    num_of_rows_in_last_page = db_length % SIZE_OF_PAGE / Row::SIZE_OF_ROW;

    for (int i = 0;i < MAX_PAGES_NUM;++i)
        pages[i] = nullptr;
}

void Pager::set_num_of_pages(uint32_t num_of_pages)
{
    this->num_of_pages = num_of_pages;
}

uint32_t Pager::get_num_of_pages()
{
    return num_of_pages;
}

void Pager::set_num_of_row_in_last_page(uint32_t num_of_rows_in_last_page)
{
    this->num_of_rows_in_last_page = num_of_rows_in_last_page;
}

uint32_t Pager::get_num_of_rows_in_last_page()
{
    return num_of_rows_in_last_page;
}

uint8_t *Pager::get_page(uint32_t page_num)
{
    if (page_num > MAX_PAGES_NUM)
        exit(EXIT_FAILURE);

    if (pages[page_num] == nullptr)
        pages[page_num] = new uint8_t[SIZE_OF_PAGE];

    if (page_num <= num_of_pages) {
        fseek(db_fd, page_num*SIZE_OF_PAGE, SEEK_SET);
        fread(pages[page_num], SIZE_OF_PAGE, 1, db_fd);
    } else {
        num_of_pages = page_num;
    }

    return pages[page_num];
}

void Pager::page_flush_and_close()
{
    for (uint32_t i = 0;i < num_of_pages;++i) {
        if (pages[i] != nullptr) {
            fseek(db_fd, i*SIZE_OF_PAGE, SEEK_SET);
            fwrite(pages[i], SIZE_OF_PAGE, 1, db_fd);
            delete [] pages[i];
            pages[i] = nullptr;
        }
    }

    if (pages[num_of_pages] != nullptr) {
        for (uint32_t i = 0;i < num_of_rows_in_last_page;++i) {
            fseek(db_fd, num_of_pages*SIZE_OF_PAGE+i*Row::SIZE_OF_ROW, SEEK_SET);
            fwrite(pages[num_of_pages]+i*Row::SIZE_OF_ROW, Row::SIZE_OF_ROW, 1, db_fd);
        }
        delete [] pages[num_of_pages];
        pages[num_of_pages] = nullptr;
    }

    fclose(db_fd);
}
