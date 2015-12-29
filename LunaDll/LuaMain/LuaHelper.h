// LuaHelper.h Helper Functions
#ifndef LuaHelper_HHH
#define LuaHelper_HHH

#include <type_traits>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>
#include "../Misc/VB6StrPtr.h"



namespace LuaHelper {
    luabind::object getEventCallbase(lua_State* base, std::string eventTable);
    bool is_function(struct lua_State * luaState, const char * fname);
    void assignVB6StrPtr(VB6StrPtr* ptr, const luabind::object &value, lua_State* L);
    
    class invalidIDException : std::exception {
    public:
        invalidIDException(int usedID) : std::exception() {
            m_usedID = usedID;
        }
        int usedID() const { return m_usedID; }
        void usedID(int val) { m_usedID = val; }
    private:
        int m_usedID;
    };
    class invalidTypeException : std::exception {
    public:
        invalidTypeException() : std::exception() {}
    };

    bool* generateFilterTable(lua_State* L, luabind::object theFilter, int maxVal, int minVal = 1);

    // Helper template to get a Lua object list, filtered
    template<typename C, typename W, typename F>
    luabind::object getObjList(C count, const W &wrapFunc, const F &filterFunc, lua_State* L)
    {
        luabind::object retTable = luabind::newtable(L);
        unsigned int retIndex = 1;

        for (C i = 0; i < count; i++) {
            if (filterFunc(i)) {
                retTable[retIndex++] = wrapFunc(i);
            }
        }

        return retTable;
    }

    // Helper template to get a Lua object list, non-filtered
    template<typename C, typename W>
    luabind::object getObjList(C count, const W &wrapFunc, lua_State* L)
    {
        return getObjList(count, wrapFunc, [](unsigned int i) { return true; }, L);
    }


    template<class T, char const* clsName>
    struct LuaBaseClassUtils 
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

};


// Original Posting: http://stackoverflow.com/questions/15037955/luabind-cant-return-shared-ptr
// We need to add this, in order to use std::shared_ptr
#include <boost/version.hpp>
#if BOOST_VERSION >= 105300
#include <boost/get_pointer.hpp>
namespace luabind {
    namespace detail {
        namespace has_get_pointer_ {
            template<class T>
            T * get_pointer(std::shared_ptr<T> const& p) { return p.get(); }
        }
    }
}
#else // if BOOST_VERSION < 105300
#include <memory>
// Not standard conforming: add function to ::std(::tr1)
namespace std {
#if defined(_MSC_VER) && _MSC_VER < 1700
    namespace tr1 {
#endif
        template<class T>
        T * get_pointer(shared_ptr<T> const& p) { return p.get(); }
#if defined(_MSC_VER) && _MSC_VER < 1700
    } // namespace tr1
#endif
} // namespace std
#endif // if BOOST_VERSION < 105300




#define LUAHELPER_DEF_CONST(luabindObj, defName) luabindObj [ #defName ] = defName

#define LUAHELPER_HELPCLASS_NAME(name) HelperClass_ ## name
#define LUAHELPER_HELPCLASS_STR_NAME(name) _cls_ ## name

#define LUAHELPER_DEF_CLASS_HELPER(classType, name) \
    extern const char LUAHELPER_HELPCLASS_STR_NAME(name) [] = #name ; \
    typedef LuaHelper::LuaBaseClassUtils< classType , LUAHELPER_HELPCLASS_STR_NAME(name) > LUAHELPER_HELPCLASS_NAME(name) ;

#define LUAHELPER_DEF_CLASS(name) \
    luabind::class_< LUAHELPER_HELPCLASS_NAME(name) ::cls>( LUAHELPER_HELPCLASS_NAME(name) ::getRawName()) \
        .property("__type", & LUAHELPER_HELPCLASS_NAME(name) ::getName)

#define LUAHELPER_DEF_CLASS_SMART_PTR_SHARED(name, smartPtrClass) \
    luabind::class_< LUAHELPER_HELPCLASS_NAME(name) ::cls, smartPtrClass ## < LUAHELPER_HELPCLASS_NAME(name) ::cls > >( LUAHELPER_HELPCLASS_NAME(name) ::getRawName()) \
        .property("__type", & LUAHELPER_HELPCLASS_NAME(name) ::getName)

#define LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(tableObj, elemKey) \
    try { \
        elemKey = luabind::object_cast< decltype(elemKey) >( tableObj [ #elemKey ] ); \
    } catch (luabind::cast_failed& e) { \
        luaL_error(L, "Value '" #elemKey "' is not set or has the wrong type!"); \
        return; \
    }


#define LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(tableObj, elemKey, defValue) \
    try { \
        elemKey = luabind::object_cast< decltype(elemKey) >( tableObj [ #elemKey ] ); \
    } catch (luabind::cast_failed& /*e*/) { \
        if(!tableObj [ #elemKey ]) { \
            elemKey = defValue; \
        } else { \
            luaL_error(L, "Value '" #elemKey "' has the wrong type!"); \
            return; \
        } \
    }




#endif
