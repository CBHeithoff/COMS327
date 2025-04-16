#ifndef C_FUNCTION_H
#define C_FUNCTION_H


# ifdef __cplusplus
using namespace std; // needs to go here so only cpp sees it, since it is invalid c elsewhere
extern "C" {
#else
    typedef void ostream; // only seen by c compiler, since we cannot let it stump on cpp ostream definition
# endif

void print(const char *s);

# ifdef __cplusplus
}
# endif

#endif