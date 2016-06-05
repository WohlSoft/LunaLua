#ifndef LunaGenHelperUtils_hhhhh
#define LunaGenHelperUtils_hhhhh

#include <string>
#include <luabind/luabind.hpp>

namespace LunaGen 
{
    template<class T, char const* clsName>
    struct LunaGenBaseClassUtils
    {
        using cls = T;

        static luabind::class_<T> defClass() {
            return luabind::class_<T>(clsName);
        }

        static std::string getName(T& cls) {
            return std::string(clsName);
        }

        static const char *getRawName() {
            return clsName;
        }
    };
}

#ifdef

#endif

#define LUNAGEN_HELPCLASS_NAME(name) HelperClass_ ## name
#define LUNAGEN_HELPCLASS_STR_NAME(name) _cls_ ## name

#define LUNAGEN_DEF_CLASS_HELPER(classType, name) \
    extern const char LUNAGEN_HELPCLASS_STR_NAME(name) [] = #name ; \
    typedef LunaGen::LunaGenBaseClassUtils< classType , LUNAGEN_HELPCLASS_STR_NAME(name) > LUNAGEN_HELPCLASS_NAME(name) ;

#define LUNAGEN_DEF_CLASS(name) \
    luabind::class_< LUNAGEN_HELPCLASS_NAME(name) ::cls>( LUNAGEN_HELPCLASS_NAME(name) ::getRawName()) \
        .property("__type", & LUNAGEN_HELPCLASS_NAME(name) ::getName)

#define LUNAGEN_DEF_CLASS_SMART_PTR_SHARED(name, smartPtrClass) \
    luabind::class_< LUNAGEN_HELPCLASS_NAME(name) ::cls, smartPtrClass ## < LUNAGEN_HELPCLASS_NAME(name) ::cls > >( LUNAGEN_HELPCLASS_NAME(name) ::getRawName()) \
        .property("__type", & LUNAGEN_HELPCLASS_NAME(name) ::getName)


#endif