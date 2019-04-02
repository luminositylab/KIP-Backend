/*
  ParseOption.h - Creates an independant parse option that will execute a function when it's parse option is in the buffer.
*/

#ifndef __PARSEOPTION_H_
#define __PARSEOPTION_H_

class ParseOption {
    public:
        ParseOption(void (*action)(char), char c[]);
        void (*_action)(char);
        void test(char c);
    private:

};
#endif