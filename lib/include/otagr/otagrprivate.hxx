
#ifndef OTAGR_PRIVATE_HXX
#define OTAGR_PRIVATE_HXX

/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define OTAGR_HELPER_DLL_IMPORT __declspec(dllimport)
#define OTAGR_HELPER_DLL_EXPORT __declspec(dllexport)
#define OTAGR_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OTAGR_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define OTAGR_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define OTAGR_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define OTAGR_HELPER_DLL_IMPORT
#define OTAGR_HELPER_DLL_EXPORT
#define OTAGR_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define OTAGR_API and OTAGR_LOCAL.
 * OTAGR_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * OTAGR_LOCAL is used for non-api symbols. */

#ifndef OTAGR_STATIC /* defined if OT is compiled as a DLL */
#ifdef OTAGR_DLL_EXPORTS /* defined if we are building the OT DLL (instead of using it) */
#define OTAGR_API OTAGR_HELPER_DLL_EXPORT
#else
#define OTAGR_API OTAGR_HELPER_DLL_IMPORT
#endif /* OTAGR_DLL_EXPORTS */
#define OTAGR_LOCAL OTAGR_HELPER_DLL_LOCAL
#else /* OTAGR_STATIC is defined: this means OT is a static lib. */
#define OTAGR_API
#define OTAGR_LOCAL
#endif /* !OTAGR_STATIC */


#endif // OTAGR_PRIVATE_HXX

