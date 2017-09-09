#ifndef __CARLIFE_DEBUG_H__
#define __CARLIFE_DEBUG_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define CL_UNUSED(x) (void)x

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "[Carlife DEBUG] %s:%d: " M "\n",\
        __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr,\
        "[Carlife ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__,\
        clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr,\
        "[Carlife WARN] (%s:%d: errno: %s) " M "\n",\
        __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[Carlife INFO] (%s:%d) " M "\n",\
        __FILE__, __LINE__, ##__VA_ARGS__)

typedef enum {
    STATE_CHANGE_SUCCESS,
    STATE_CHANGE_FAILURE,
    STATE_CHANGE_INVALID
}STATE_CHANGE_RESULT;

#endif //__CARLIFE_DEBUG_H__
