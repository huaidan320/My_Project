#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#define	LOG_ERR	stderr
//#define	LOG_DEB stdout
#define	LOG_DEB stderr

#ifdef DEBUG
#define logDebug(x...) fprintf(LOG_DEB, x)
#else
#define logDebug(x...)
#endif

#define logError(x...) fprintf(LOG_ERR, x)

#define CL_UNUSED(x) (void)x

#endif /* __LOG_H__ */
