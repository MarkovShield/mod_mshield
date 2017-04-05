/*
 * Error number definitions.
 *
 * We use apr_status_t from APR as status type and use the
 * error number range reserved for the application using APR.
 *
 * APR reserves the range APR_OS_START_USERERR ... +50000
 * for the application using APR.  It is therefore safe to
 * intermingle apr_status_t from APR functions with our
 * own STATUS_* status numbers.
 */

#ifndef MOD_MSHIELD_ERRNO_H
#define MOD_MSHIELD_ERRNO_H

/* Make sure our errors are within the APR user error range */
#define MOD_MSHIELD_ERRNO_OFFSET    200
#define NEW_MOD_MSHIELD_STATUS(x)    (APR_OS_START_USERERR + MOD_MSHIELD_ERRNO_OFFSET + (x))

/*
 * mod_mshield error definitions
 */
#define STATUS_OK           APR_SUCCESS                 /* success                    */
#define STATUS_ERROR        NEW_MOD_MSHIELD_STATUS(1)   /* unspecified error          */
#define STATUS_ENOEXIST     NEW_MOD_MSHIELD_STATUS(2)   /* does not exist, not found  */
#define STATUS_ESHMFULL     NEW_MOD_MSHIELD_STATUS(3)   /* shared memory full         */
#define STATUS_ETIMEOUT     NEW_MOD_MSHIELD_STATUS(4)   /* session timeout            */

#define STATUS_ELOGIN       NEW_MOD_MSHIELD_STATUS(5)   /* login required             */
#define STATUS_ESTEPUP1     NEW_MOD_MSHIELD_STATUS(6)   /* stepup 1 required          */
#define STATUS_ESTEPUP2     NEW_MOD_MSHIELD_STATUS(7)   /* stepup 2 required          */
#define STATUS_EDENIED      NEW_MOD_MSHIELD_STATUS(8)   /* access denied              */

#define STATUS_MATCH        STATUS_OK                   /* regexp did match           */
#define STATUS_NOMATCH      STATUS_ENOEXIST             /* regexp did not match       */

#endif /* MOD_MSHIELD_ERRNO_H */
