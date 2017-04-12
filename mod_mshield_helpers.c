#include "mod_mshield.h"

/*
 * Remove trailing slash from arg1 except if arg1 is only "/"
 */
const char *
mshield_remove_trailing_slash(const char *arg1) {

    char *temp = (char *) arg1;
    //temp = malloc((sizeof(arg1) + 1) * sizeof(char));
    //apr_cpystrn(temp, arg1, sizeof(temp));
    size_t len = strlen(temp);
    ap_log_error(PC_LOG_CRIT, NULL, "temp after stpncpy: [%s]", temp);

    if (len > 1) {
        if (temp[len - 1] == '/') {	
            temp[len - 1] = '\0';
        }
    }

    return temp;
}
