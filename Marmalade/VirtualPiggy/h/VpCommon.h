#if !defined(_VP_COMMON_H_)
#define _VP_COMMON_H_

#ifndef NULL
    #define NULL 0
#endif

#define SAFE_DELETE(x) if (x != NULL) { delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) if (x != NULL) { delete [] x; x = NULL; }

typedef int (*VpCallback)(void* result, void* data);
typedef void* VpCallbackData;



#endif // _VP_COMMON_H_