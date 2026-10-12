#ifndef WINMAC_DEMO_H
#define WINMAC_DEMO_H

#if defined(_WIN32)
#  if defined(WINMAC_BUILDING)
#    define WINMAC_API __declspec(dllexport)
#  else
#    define WINMAC_API __declspec(dllimport)
#  endif
#else
#  define WINMAC_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 足し算。読み込みテストで「呼べた」ことを確かめる用 */
WINMAC_API int winmac_add(int a, int b);

/* 動いているOSの名前を返す（"windows" / "macos" / "linux"）。静的文字列なので解放不要 */
WINMAC_API const char *winmac_platform(void);

#ifdef __cplusplus
}
#endif

#endif
