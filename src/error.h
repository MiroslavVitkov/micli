#ifndef ERROR_H_
#define ERROR_H_


#include "usart.h"


enum tempr_errors_e
{
        SUCCESS = 0,
        ERROR_NO_DEVICE_FOUND = 1,
        ERROR_START_MEASUREMENT,
        ERROR_READ_TEMPERATURE,
};


// #define NDEBUG  // Uncomment for release build.

// Error checking macro. If any function invocation fails, the program halts.
// Foo is the function to be evaluated safely. If the return value is nonzero,
// error message is logged and the program stalls.
// The while(0) part is there to make sure the semicolon after the macro always means the same
// thing in cases of
//if(a)
//  MACRO;
//else
//  somethingElse()
#define CHECK_FOR_ERRORS(foo)                                                   \
    do                                                                          \
    {                                                                           \
        int ret = foo;                                                          \
        if(ret != 0)                                                            \
        {                                                                       \
            handle_error_combined(ret, __FILE__, __LINE__, __func__);     \
        }                                                                       \
    } while(0)

// Crash the program. A program path, leading to critical error occured.
#define CRASH_CRITICAL_ERROR() handle_error_extended(__FILE__, __LINE__, __func__);

// Custom runtime assert statement.
#ifdef NDEBUG
#define ASSERT(expr)
#else
#define ASSERT(expr) (void)((expr) || (handle_error_extended(__FILE__, __LINE__, __func__), 0))
#endif

// Compile-time assert with message.
// Use like: STATIC_ASSERT(exp1 > exp2, error_message_must_be_a_token);
#define STATIC_ASSERT4(COND,MSG) typedef char MSG[(!!(COND))*2-1]
#define STATIC_ASSERT3(X,M,L)  STATIC_ASSERT4(X,static_assertion_at_line_##L##__##M)
#define STATIC_ASSERT2(X,M,L)  STATIC_ASSERT3(X,M,L)
#define STATIC_ASSERT(X,M)     STATIC_ASSERT2(X,M,__LINE__)


typedef int8_t error_t;


inline void handle_error(error_t err)
{
    printf("Error %i" NEWLINE, err);
}


inline void handle_error_extended(char *file, int line, char *func)
{
    printf("assert() failed in %s:%i at %s()." NEWLINE, file, line, func);
}


inline void handle_error_combined(error_t err, char *file, int line, char *func)
{
    printf("%s returned error %i at %s:%i." NEWLINE, func, err, file, line);
}


#endif  // defined(ERROR_H_)
