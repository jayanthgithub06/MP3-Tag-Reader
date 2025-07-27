#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    failure,
    success
} Status;

typedef enum
{
    mp3_edit,
    mp3_view,
    mp3_help,
    mp3_search,
    mp3_unsupported
} OperationType;

#endif
