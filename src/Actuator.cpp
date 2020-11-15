#include "../include/Actuator.h"

Actuator::Actuator(const char *filename)
{
    pager = new Pager(filename);
    cursor = new Cursor(pager->get_num_of_pages(), pager->get_num_of_rows_in_last_page());
}

void Actuator::set_cursor(Cursor *cursor)
{
    this->cursor = cursor;
}

void Actuator::set_pager(Pager *pager)
{
    this->pager = pager;
}

HandleResult Actuator::exec_statement(Statement *statement)
{
    switch (statement->get_type()) {
        case StatementType::EXIT:
            {
                pager->set_num_of_pages(cursor->get_page_num());
                pager->set_num_of_row_in_last_page(cursor->get_row_num());
                pager->page_flush_and_close();
                return HandleResult::EXIT;
            }
        case StatementType::INSERT:
            {
                uint8_t *page = pager->get_page(cursor->get_page_num());
                statement->get_row_to_insert()->serialize_row(page+cursor->get_row_num()*Row::SIZE_OF_ROW);
                cursor->advance();
                break;
            }
        case StatementType::SELECT:
            {
                for (uint32_t i = 0;i < cursor->get_page_num();++i) {
                    for (uint32_t j = 0;j < Cursor::ROWS_OF_PAGE;++j) {
                        Row *tmp_row = new Row();
                        uint8_t *page = pager->get_page(i);
                        tmp_row->deserialize_row(page+j*Row::SIZE_OF_ROW);
                        tmp_row->print_row();
                        delete tmp_row;
                    }
                }
                uint8_t *page = pager->get_page(cursor->get_page_num());
                for (uint32_t i = 0;i < cursor->get_row_num();++i) {
                    Row *tmp_row = new Row();
                    tmp_row->deserialize_row(page+i*Row::SIZE_OF_ROW);
                    tmp_row->print_row();
                    delete tmp_row;
                }
                break;
            }
    }
    return HandleResult::SUCCESS;
}

Actuator::~Actuator()
{
    delete cursor;
    delete pager;
}
