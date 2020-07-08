#ifndef DETECTPLATEFORM_H_
#define DETECTPLATEFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

// --- ENDIANNESS ---
#define ARCH_LITTLE_ENDIAN

// --- PLATFORM ---

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC  2
#define PLATFORM_UNIX 3

/* Windows */
#ifdef _WIN32
#define PLATFORM PLATFORM_WINDOWS
#elif _WIN64
#define PLATFORM PLATFORM_WINDOWS
/* Mac OS X */
#elif __APPLE__
#define PLATFORM PLATFORM_MAC
/* UNIX & Linux */
#elif __linux
#define PLATFORM PLATFORM_UNIX
#elif __unix
#define PLATFORM PLATFORM_UNIX
#elif __posix
#define PLATFORM PLATFORM_UNIX
#else
#error unknown platform!
#endif

#ifdef __cplusplus
}
#endif

#endif /* DETECTPLATEFORM_H_ */
