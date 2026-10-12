/* 組み立てた共有ライブラリを実際に読み込み、関数を呼ぶ。
   使い方: load_test <ライブラリのパス> */
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#  include <windows.h>
typedef HMODULE lib_t;
static lib_t lib_open(const char *p) { return LoadLibraryA(p); }
static void *lib_sym(lib_t l, const char *n) { return (void *)GetProcAddress(l, n); }
static void lib_error(char *buf, size_t n) { snprintf(buf, n, "Win32エラー番号 %lu", GetLastError()); }
#else
#  include <dlfcn.h>
typedef void *lib_t;
static lib_t lib_open(const char *p) { return dlopen(p, RTLD_NOW); }
static void *lib_sym(lib_t l, const char *n) { return dlsym(l, n); }
static void lib_error(char *buf, size_t n) { const char *e = dlerror(); snprintf(buf, n, "%s", e ? e : "不明"); }
#endif

int main(int argc, char **argv) {
    char err[512];
    if (argc < 2) { fprintf(stderr, "使い方: %s <ライブラリのパス>\n", argv[0]); return 2; }

    lib_t lib = lib_open(argv[1]);
    if (!lib) { lib_error(err, sizeof err); fprintf(stderr, "LOAD_FAIL: %s を読み込めない (%s)\n", argv[1], err); return 1; }

    int (*add)(int, int) = (int (*)(int, int))lib_sym(lib, "winmac_add");
    const char *(*platform)(void) = (const char *(*)(void))lib_sym(lib, "winmac_platform");
    if (!add || !platform) { fprintf(stderr, "SYMBOL_FAIL: 関数が見つからない（エクスポート漏れ）\n"); return 1; }

    if (add(2, 3) != 5) { fprintf(stderr, "CALL_FAIL: winmac_add(2,3) = %d（期待 5）\n", add(2, 3)); return 1; }
    printf("OK: %s で %s を読み込み、関数を呼べた\n", platform(), argv[1]);
    return 0;
}
