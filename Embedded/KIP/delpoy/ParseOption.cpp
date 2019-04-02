#include "ParseOption.h"

ParseOption::ParseOption(void (*action)(char), char c[]) {
    void (*_action)(char) = action;
}

void ParseOption::test(char c) {
    _action(c);
}