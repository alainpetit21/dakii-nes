#ifndef UNIXDEP_H
#define UNIXDEP_H

#include <sys/param.h>

#ifdef __FreeBSD__
#define SYSTEM_FREEBSD
#endif

#ifdef __NetBSD__
#define SYSTEM_NETBSD
#endif

#ifdef __OpenBSD__
#define SYSTEM_OPENBSD
#endif

/* whoever defined this symbol as an unadorned lowercase should be slapped */
#ifdef linux
#define SYSTEM_LINUX
#endif

#endif /* UNIXDEP_H */
