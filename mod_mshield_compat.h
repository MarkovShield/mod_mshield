/* $Id: mod_mshield_compat.h 147 2010-05-30 20:28:01Z ibuetler $ */

/*
 * Compatibility hacks.
 *
 * This header must be included after <httpd.h>
 */

#ifndef MOD_MSHIELD_COMPAT_H
#define MOD_MSHIELD_COMPAT_H

/* unix specific stuff */
#if !defined(OS2) && !defined(WIN32) && !defined(BEOS) && !defined(NETWARE)
#include "unixd.h"
#define MOD_MSHIELD_SET_MUTEX_PERMS
#endif

/* ap_http_method was renamed to ap_http_scheme between Apache 2.0 and 2.2 */
#if !defined(ap_http_scheme)
# define ap_http_scheme ap_http_method
#endif

#endif /* MOD_MSHIELD_COMPAT_H */
