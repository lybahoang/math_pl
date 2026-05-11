#ifndef INVALIDCHAR_H
#define INVALIDCHAR_H

#include <stdexcept> // class của c++ để lưu lỗi sau đó coòn vứt vào try-catch
#include <string>
using namespace std;

class InvalidCharacter : public runtime_error {
public:
    explicit InvalidCharacter(const string& message)
        : runtime_error(message) {}
};

#endif