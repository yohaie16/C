#ifndef __PREPROCESSOR_H_
#define __PREPROCESSOR_H_

enum line_roll {
    empty,
    macro_start,
    macro_end,
    exists,
    bad_definition,
    bad_macro_ending,
    macro_call,
    bad_macro_call,
    other
};


const char * preprocessor(const char * file);

#endif
