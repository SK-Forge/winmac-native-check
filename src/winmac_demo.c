#include "winmac_demo.h"

int winmac_add(int a, int b) { return a + b; }

const char *winmac_platform(void) {
#if defined(_WIN32)
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#else
    return "linux";
#endif
}
