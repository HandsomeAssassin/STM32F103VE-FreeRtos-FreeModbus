#ifndef _VERSION_H_
#define _VERSION_H_

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#define VERSION_NUM ((VERSION_MAJOR << 16) | \
                     (VERSION_MINOR << 8)  | \
                     (VERSION_PATCH))

#define _VERSTR(x)	#x
#define VERSTR(x)	_VERSTR(x)
#define VERSION_STR VERSTR(VERSION_MAJOR) "." \
                    VERSTR(VERSION_MINOR) "." \
                    VERSTR(VERSION_PATCH)

#endif /* _VERSION_H_ */
