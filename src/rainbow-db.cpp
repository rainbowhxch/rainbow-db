#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::string;

#define WORD_HELLO "rainbow-db> "
#define WORD_BYE "BYE"

struct InputBuffer {
    string data;
};

enum class HANDLE_RESULT : unsigned int {
    VALID_INPUT,
    INVALID_INPUT
};

void print_prompt();
void read_input(InputBuffer *input_buffer);
HANDLE_RESULT handle_input(InputBuffer *input_buffer);

int main(int argc, char *argv[])
{
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
    cin >> input_buffer->data;
}

HANDLE_RESULT handle_input(InputBuffer *input_buffer) {
    if (input_buffer->data.at(0) == '.') {
        if (input_buffer->data == ".exit") {
            cout << WORD_BYE;
            exit(EXIT_SUCCESS);
        } else {
            return HANDLE_RESULT::INVALID_INPUT;
        }
    } else {
        return HANDLE_RESULT::VALID_INPUT;
    }
}
