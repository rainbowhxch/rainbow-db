#include <bits/stdint-uintn.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdint>
#include <cstring>
#include <sys/types.h>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::uint32_t;

#define WORD_HELLO "rainbow-db> "
#define WORD_BYE "BYE"
#define SIZE_USERNAME 255
#define SIZE_EMAIL 255

#define size_of_attribute(Type, Member) sizeof(((Type*)0)->Member)

#define SIZE_PAGE 4096
#define TABLE_MAX_PAGES 100

struct InputBuffer {
    string data;
};

struct Row {
    uint32_t id;
    char username[SIZE_USERNAME+1];
    char email[SIZE_EMAIL+1];
};

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t NUM_ROW_OF_PAGE = SIZE_PAGE / ROW_SIZE;

struct PagesBuffer {
    uint32_t num_pages;
    uint8_t *pages[TABLE_MAX_PAGES];
};

struct Cursor {
    uint32_t cur_page_num;
    uint32_t cur_row_num;
};

struct Table {
    PagesBuffer *pages_buffer;
    Cursor *cursor;
};

enum class STATEMENT_TYPE : unsigned int {
    INSERT,
    SELECT
};

struct Statement {
    STATEMENT_TYPE type;
    Row *insert_row;
};

enum class PREPARE_RESULT : unsigned int {
    PREPARE_SUCCESS,
    PREPARE_FAIL
};

enum class HANDLE_RESULT : unsigned int {
    HANDLE_SUCCESS = 0,
    INVALID_INPUT,
    INSERT_FAIL
};

Table *table;

void print_prompt();
void read_input(InputBuffer *input_buffer);
HANDLE_RESULT handle_input(InputBuffer *input_buffer);
HANDLE_RESULT handle_dot_command(InputBuffer *input_buffer);
HANDLE_RESULT handle_exec_command(Statement *statement);
PREPARE_RESULT prepare_statement(InputBuffer *input_buffer, Statement *statement);
HANDLE_RESULT exec_insert(Statement *statement);
HANDLE_RESULT exec_select();
void print_row(Row *row);
void init_table(Table *&table);
uint8_t *get_page(uint32_t page_num);

int main(int argc, char *argv[])
{
    init_table(table);
    InputBuffer *input_buffer = new InputBuffer;

    while (true) {
        print_prompt();
        read_input(input_buffer);
        handle_input(input_buffer);
    }

    return 0;
}

void print_prompt()
{
    cout << WORD_HELLO;
}

void read_input(InputBuffer *input_buffer) {
    std::getline(std::cin, input_buffer->data);
}

HANDLE_RESULT handle_input(InputBuffer *input_buffer) {
    if (input_buffer->data.front() == '.') {
        switch (handle_dot_command(input_buffer)) {
            case HANDLE_RESULT::HANDLE_SUCCESS:
                return HANDLE_RESULT::HANDLE_SUCCESS;
            case HANDLE_RESULT::INVALID_INPUT:
                return HANDLE_RESULT::INVALID_INPUT;
            default:
                break;
        }
    } else {
        Statement *statement = new Statement;
        switch (prepare_statement(input_buffer, statement)) {
            case PREPARE_RESULT::PREPARE_FAIL:
                cout << "insert prepare failed" << endl;
                return HANDLE_RESULT::INSERT_FAIL;
            case PREPARE_RESULT::PREPARE_SUCCESS:
                break;
            default:
                break;
        }

        switch (handle_exec_command(statement)) {
            /* TODO: some exec command results */
            default:
                break;
        }
    }

    return HANDLE_RESULT::HANDLE_SUCCESS;
}

HANDLE_RESULT handle_dot_command(InputBuffer *input_buffer) {
    if (input_buffer->data == ".exit") {
        cout << WORD_BYE;
        exit(EXIT_SUCCESS);
    } else {
        return HANDLE_RESULT::INVALID_INPUT;
    }
}

HANDLE_RESULT handle_exec_command(Statement *statement) {
    switch (statement->type) {
        case STATEMENT_TYPE::INSERT:
            return exec_insert(statement);
            break;
        case STATEMENT_TYPE::SELECT:
            return exec_select();
            break;
        default:
            break;
    }

    return HANDLE_RESULT::HANDLE_SUCCESS;
}

PREPARE_RESULT prepare_statement(InputBuffer *input_buffer, Statement *statement) {
    if (strncmp(input_buffer->data.c_str(), "insert", strlen("insert")) == 0) {
        statement->type = STATEMENT_TYPE::INSERT;
        statement->insert_row = new Row;

        size_t pos = 0;
        std::string delimiter = " ";
        if ((pos = input_buffer->data.find(delimiter)) != string::npos)
            input_buffer->data.erase(0, pos + delimiter.length());
        else
            return PREPARE_RESULT::PREPARE_FAIL;
        if ((pos = input_buffer->data.find(delimiter)) != string::npos)
            statement->insert_row->id = std::stoi(input_buffer->data.substr(0, pos));
        else
            return PREPARE_RESULT::PREPARE_FAIL;
        input_buffer->data.erase(0, pos + delimiter.length());
        if ((pos = input_buffer->data.find(delimiter)) != string::npos)
            strncpy(statement->insert_row->username, input_buffer->data.substr(0, pos).c_str(), input_buffer->data.substr(0, pos).length());
        else
            return PREPARE_RESULT::PREPARE_FAIL;
        input_buffer->data.erase(0, pos + delimiter.length());
        if (input_buffer->data.length() == 0)
            return PREPARE_RESULT::PREPARE_FAIL;
        strncpy(statement->insert_row->email, input_buffer->data.c_str(), input_buffer->data.length());

        return PREPARE_RESULT::PREPARE_SUCCESS;
    } else if (strncmp(input_buffer->data.c_str(), "select", strlen("select")) == 0) {
        statement->type = STATEMENT_TYPE::SELECT;

        return PREPARE_RESULT::PREPARE_SUCCESS;
    } else {
        return PREPARE_RESULT::PREPARE_FAIL;
    }

    return PREPARE_RESULT::PREPARE_SUCCESS;
}

HANDLE_RESULT exec_insert(Statement *statement) {
    uint8_t *page = get_page(table->cursor->cur_page_num);
    memcpy(page+table->cursor->cur_row_num*ROW_SIZE, statement->insert_row, ROW_SIZE);

    table->cursor->cur_row_num++;
    if (table->cursor->cur_row_num == NUM_ROW_OF_PAGE) {
        table->cursor->cur_page_num++;
        table->cursor->cur_row_num = 0;
    }

    return HANDLE_RESULT::HANDLE_SUCCESS;
}

HANDLE_RESULT exec_select() {
    cout << "----------------------------------" << endl;
    for (uint32_t i = 0;i < table->cursor->cur_page_num;++i) {
        for (uint32_t j = 0;j < NUM_ROW_OF_PAGE;++j) {
            Row *tmp_row = new Row;
            uint8_t *page = get_page(i);
            memcpy(tmp_row, page+j*ROW_SIZE, ROW_SIZE);
            print_row(tmp_row);
            cout << "----------------------------------" << endl;
        }
    }
    for (uint32_t i = 0;i < table->cursor->cur_row_num;++i) {
        Row *tmp_row = new Row;
        uint8_t *page = get_page(table->cursor->cur_page_num);
        memcpy(tmp_row, page+i*ROW_SIZE, ROW_SIZE);
        print_row(tmp_row);
        cout << "----------------------------------" << endl;
    }

    return HANDLE_RESULT::HANDLE_SUCCESS;
}

void print_row(Row *row) {
    cout << "id:      \t" << row->id << endl;
    cout << "++++++++++++++++++++++++++++++++++" << endl;
    cout << "username:\t" << row->username << endl;
    cout << "++++++++++++++++++++++++++++++++++" << endl;
    cout << "email:   \t" << row->email << endl;
}

void init_table(Table *&table) {
    table = new Table;

    table->pages_buffer = new PagesBuffer;
    table->pages_buffer->num_pages = 0;

    table->cursor = new Cursor;
    table->cursor->cur_page_num = 0;
    table->cursor->cur_row_num = 0;
}

uint8_t *get_page(uint32_t page_num) {
    if (table->pages_buffer->pages[page_num] == nullptr) {
        table->pages_buffer->pages[page_num] = new uint8_t[SIZE_PAGE];
    }
    return table->pages_buffer->pages[page_num];
}
