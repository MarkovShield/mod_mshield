#include "mod_mshield.h"

/*
 * Remove trailing slash from arg1 except if arg1 is only "/"
 */
const char *
mshield_remove_trailing_slash(const char *arg1) {

    char *temp;
    temp = malloc((strlen(arg1) + 1) * sizeof(char));
    stpncpy(temp, arg1, sizeof(temp));
    size_t len = strlen(temp);

    if (len > 1) {
        if ('/' == temp[len - 1]) {
            temp[len - 1] = '\0';
        }
    }

    return temp;
}
