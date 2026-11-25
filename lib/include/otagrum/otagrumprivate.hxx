
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

#ifdef __cplusplus
#ifndef SWIG
// Workaround for ambiguity between OpenTURNS and aGrUM operator<< for std::pair
// aGrUM defines std::operator<<(ostream, pair) in namespace std
// (agrum/base/core/utils_misc_tpl.h) while OpenTURNS defines
// OT::operator<<(ostream, pair) in namespace OT (openturns/OSS.hxx).
// When both headers are visible (always the case in otagrum TUs combining
// both libraries), OT::Cache::__repr__ (openturns/Cache.hxx, which does
// oss << std::make_pair(key, value) since OpenTURNS 1.28) becomes ambiguous
// (C2593 on MSVC). A non-template overload in namespace OT is preferred over
// both function templates and resolves the ambiguity. It must be visible in
// every TU including both libraries, hence it lives here (included by all
// otagrum headers).
#include <ostream>
#include <utility>
#include <openturns/OTtypes.hxx>
#include <openturns/PersistentCollection.hxx>
#include <openturns/Point.hxx>
#include <openturns/Sample.hxx>
namespace OT
{
inline std::ostream & operator<<(std::ostream & os, const std::pair<PersistentCollection<Scalar>, PersistentCollection<Scalar>> & p)
{
  return os << "(" << p.first << "," << p.second << ")";
}
inline std::ostream & operator<<(std::ostream & os, const std::pair<Point, Point> & p)
{
  return os << "(" << p.first << "," << p.second << ")";
}
inline std::ostream & operator<<(std::ostream & os, const std::pair<Sample, Sample> & p)
{
  return os << "(" << p.first << "," << p.second << ")";
}
}
#endif // SWIG
#endif // __cplusplus

#endif // OTAGRUM_PRIVATE_HXX

