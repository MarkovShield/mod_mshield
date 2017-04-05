/* $Id: mod_mshield_debug.h 147 2010-05-30 20:28:01Z ibuetler $ */

/*
 * Various debugging/logging helpers.
 */

#ifndef MOD_MSHIELD_DEBUG_H
#define MOD_MSHIELD_DEBUG_H

/* log level shortcuts */
#define PC_LOG_DEBUG	APLOG_MARK,APLOG_DEBUG,0
#define PC_LOG_INFO		APLOG_MARK,APLOG_INFO,0
#define PC_LOG_CRIT		APLOG_MARK,APLOG_CRIT,0

/*
 * Convenience logging shortcuts - they assume request_rec *r or server_rec *s
 * are available, depending on variant.
 *
 * *_INFO and DEBUG_GENERAL are used for printf debugging
 * *_CRIT are used for error messages
 *
 * Example:
 *
 *    char *foo = "foo";
 *    char *bar = "bar";
 *    ERRLOG_CRIT("Failed to copy %s to %s", foo, bar);
 *
 * Error log will look like:
 *
 *    [Thu May 29 11:26:18 2008] [crit] [client 127.0.0.1] mod_mshield_example.c:21: Failed to copy foo to bar
 */

#define ERRLOG_REQ(level, format, ...)	    ap_log_rerror(level, r, "[%s] %s:%d: " format, apr_table_get(r->subprocess_env, "UNIQUE_ID"), __FILE__, __LINE__, ##__VA_ARGS__)
#define ERRLOG_REQ_INFO(format, ...)        ERRLOG_REQ(PC_LOG_DEBUG, format, ##__VA_ARGS__)
#define ERRLOG_REQ_CRIT(format, ...)        ERRLOG_REQ(PC_LOG_CRIT, format, ##__VA_ARGS__)

#define ERRLOG_SRV(level, format, ...)	    ap_log_error(level, s, "%s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERRLOG_SRV_INFO(format, ...)	    ERRLOG_SRV(PC_LOG_DEBUG, format, ##__VA_ARGS__)
#define ERRLOG_SRV_CRIT(format, ...)	    ERRLOG_SRV(PC_LOG_CRIT, format, ##__VA_ARGS__)

#define ERRLOG_INFO(format, ...)	        ERRLOG_REQ_INFO(format, ##__VA_ARGS__)
#define ERRLOG_CRIT(format, ...)	        ERRLOG_REQ_CRIT(format, ##__VA_ARGS__)

#endif /* MOD_MSHIELD_DEBUG_H */
