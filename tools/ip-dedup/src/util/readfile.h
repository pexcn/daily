#ifndef _HAVE_UTIL_READFILE_H_
#define _HAVE_UTIL_READFILE_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum {
    READFILE_RET_LINE = 0,
    READFILE_RET_EMPTY_LINE = 1,
    READFILE_RET_COMMENT_LINE = 2,
    READFILE_RET_EOF = 3
};

struct readfile_state {
    FILE*   stream;
    bool    close_stream;

    char*   line_buf;
    size_t  line_buf_size;

    char*   line;       /* shared ref */
    size_t  line_len;
};


void readfile_state_init_null ( struct readfile_state* const rstate );
void readfile_state_free_data ( struct readfile_state* const rstate );
void readfile_state_close_file ( struct readfile_state* const rstate );

int readfile_init_stream (
    struct readfile_state* const rstate,
    FILE* const input_stream
);


int readfile_init_file (
    struct readfile_state* const rstate,
    const char* const input_file
);


int readfile_next (
    struct readfile_state* const rstate
);


int readfile_next_effective (
    struct readfile_state* const rstate
);

#endif  /* _HAVE_UTIL_READFILE_H_ */
