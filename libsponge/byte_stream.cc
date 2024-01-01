#include "byte_stream.hh"

#include <algorithm>
#include <iterator>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) 
    : buffer_capacity(capacity), buffer(), EndOfFile(false),
      bytes_written_cnt(0), bytes_read_cnt(0) {
    this->buffer.clear();  // empty buffer initially
}

size_t ByteStream::write(const string &data) {
    size_t i = 0;
    for (auto & c : data) {
        if (this->buffer.size() >= this->buffer_capacity) {
            break;  // no more bytes can be written
        } else {
            i++;
            this->buffer.push_back(c);
        }
    }
    this->bytes_written_cnt += i;  // total number of bytes written
    return i;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string output = "";
    int idx = 0;  // the index in the buffer to be read
    int length = len;  // total number of bytes requested
    int size = this->buffer.size();  // total number of bytes in the buffer
    while (length > 0) {
        if (size > idx) {  // safe to read
            output += this->buffer[idx];
            idx++;  length--;
        } else {
            break;
        }
    }
    return output;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    for (size_t i = 0; i < len; ++i)
        if (!this->buffer_empty()) {
            this->buffer.pop_front();
            this->bytes_read_cnt++;  // total number of bytes popped
        } else {
            return;
        }
}

void ByteStream::end_input() { this->EndOfFile = true; }

bool ByteStream::input_ended() const { return this->EndOfFile; }

size_t ByteStream::buffer_size() const { return this->buffer.size(); }

bool ByteStream::buffer_empty() const { return this->buffer_size() == 0; }

bool ByteStream::eof() const { return this->EndOfFile && this->buffer_empty(); }

size_t ByteStream::bytes_written() const { return this->bytes_written_cnt; }

size_t ByteStream::bytes_read() const { return this->bytes_read_cnt; }

size_t ByteStream::remaining_capacity() const { return this->buffer_capacity - this->buffer_size(); }
