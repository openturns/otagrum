
#ifndef OTAGRUM_PRIVATE_HXX
#define OTAGRUM_PRIVATE_HXX

/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define OTAGRUM_HELPER_DLL_IMPORT __declspec(dllimport)
#define OTAGRUM_HELPER_DLL_EXPORT __declspec(dllexport)
#define OTAGRUM_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OTAGRUM_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define OTAGRUM_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define OTAGRUM_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define OTAGRUM_HELPER_DLL_IMPORT
#define OTAGRUM_HELPER_DLL_EXPORT
#define OTAGRUM_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define OTAGRUM_API and OTAGRUM_LOCAL.
 * OTAGRUM_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * OTAGRUM_LOCAL is used for non-api symbols. */

#ifndef OTAGRUM_STATIC /* defined if OT is compiled as a DLL */
#ifdef OTAGRUM_DLL_EXPORTS /* defined if we are building the OT DLL (instead of using it) */
#define OTAGRUM_API OTAGRUM_HELPER_DLL_EXPORT
#else
#define OTAGRUM_API OTAGRUM_HELPER_DLL_IMPORT
#endif /* OTAGRUM_DLL_EXPORTS */
#define OTAGRUM_LOCAL OTAGRUM_HELPER_DLL_LOCAL
#else /* OTAGRUM_STATIC is defined: this means OT is a static lib. */
#define OTAGRUM_API
#define OTAGRUM_LOCAL
#endif /* !OTAGRUM_STATIC */


#endif // OTAGRUM_PRIVATE_HXX

