#ifndef LunaGenHelperUtils_hhhhh
#define LunaGenHelperUtils_hhhhh

#include <string>

struct Momentum
{
    double x;
    double y;
    double height;
    double width;
    double speedX;
    double speedY;
};

#define COMBOOL(b) (b ? -1 : 0)


// Macro Utils
// Found from http://stackoverflow.com/a/1872506/5082374
#define STRINGIZE(arg)  STRINGIZE1(arg)
#define STRINGIZE1(arg) STRINGIZE2(arg)
#define STRINGIZE2(arg) #arg

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define FOR_EACH_1(what, x, ...) what(x)
#define FOR_EACH_2(what, x, ...)\
  what(x),\
  FOR_EACH_1(what,  __VA_ARGS__)
#define FOR_EACH_3(what, x, ...)\
  what(x),\
  FOR_EACH_2(what,  __VA_ARGS__)
#define FOR_EACH_4(what, x, ...)\
  what(x),\
  FOR_EACH_3(what,  __VA_ARGS__)
#define FOR_EACH_5(what, x, ...)\
  what(x),\
  FOR_EACH_4(what,  __VA_ARGS__)
#define FOR_EACH_6(what, x, ...)\
  what(x),\
  FOR_EACH_5(what,  __VA_ARGS__)
#define FOR_EACH_7(what, x, ...)\
  what(x),\
  FOR_EACH_6(what,  __VA_ARGS__)
#define FOR_EACH_8(what, x, ...)\
  what(x),\
  FOR_EACH_7(what,  __VA_ARGS__)

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__) 
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N 
#define FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_(N, what, x, ...) CONCATENATE(FOR_EACH_, N)(what, x, __VA_ARGS__)
#define FOR_EACH(what, x, ...) FOR_EACH_(FOR_EACH_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)


#include <string>
#include <luabind/luabind.hpp>

namespace LunaGen
{
    namespace detail {
        template<typename ClassT>
        struct LunaGenTraits;
    }

    template<typename ConvertedType>
    struct vb6_converter {
        typedef ConvertedType from_type; // vb6
        typedef ConvertedType to_type; // C++

        static to_type ConvertForGetter(const from_type& data) {
            return data;
        }

        static from_type ConvertForSetter(const to_type& data) {
            return data;
        }
    };

    template<>
    struct vb6_converter<bool> {
        typedef short from_type; // vb6
        typedef bool to_type; // C++

        static to_type ConvertForGetter(const from_type& data) {
            return data == -1;
        }

        static from_type ConvertForSetter(const to_type& data) {
            return COMBOOL(data);
        }
    };

    // NOTE: This class is currently not actually used.
    //       m_index must be accessed somehow properly. --> Indexable Subclass ?
    //                                                  --> Move m_index to THIS subclass?
    template<class WrapperClass, class InternalClass>
    class CommonVB6Wrapper {
    public:
        typedef InternalClass internal_class;

        template<typename DataType, DataType InternalClass::* Ptr, typename InputType = DataType>
        void Setter(InputType data)
        {
            typedef vb6_converter<InputType> converter;

            InternalClass::Get(static_cast<WrapperClass*>(this)->m_index)->*Ptr = converter::ConvertForSetter(data);
        }

        template<typename DataType, DataType InternalClass::* Ptr, typename OutputType = DataType>
        OutputType Getter() const
        {
            typedef vb6_converter<OutputType> converter;

            return converter::ConvertForGetter(InternalClass::Get(static_cast<const WrapperClass*>(this)->m_index)->*Ptr);
        }

        template<Momentum InternalClass::* Ptr, double Momentum::* SubPtr>
        void MomentumSetter(double data)
        {
            (&(InternalClass::Get(static_cast<WrapperClass*>(this)->m_index)->*Ptr))->*SubPtr = data;
        }

        template<Momentum InternalClass::* Ptr, double Momentum::* SubPtr>
        double MomentumGetter() const
        {
            return (&(InternalClass::Get(static_cast<const WrapperClass*>(this)->m_index)->*Ptr))->*SubPtr;
        }

    };
    
    template<typename ClassT>
    struct LunaGenHelper {
        using internal_class_type = ClassT;
        using internal_class_info = detail::LunaGenTraits<ClassT>;

        // enable_if, with SMBX_FullBaseArray, ...
        // TODO: Get, Count, ... methods in CommonVB6Wrapper
        static auto defClass() {
            return luabind::class_<internal_class_type>(internal_class_info::clsName)
                .property("__type", &getName);
        }

        static auto defClassSharedPtr() {
            return luabind::class_<internal_class_type, std::shared_ptr<internal_class_type>>(internal_class_info::clsName)
                .property("__type", &getName);
        }

        static std::string getName(internal_class_type& cls) {
            return std::string(internal_class_info::clsName);
        }

        static const char *getRawName() {
            return internal_class_info::clsName;
        }
    };
}


#ifdef __LUNA_CODE_GENERATOR__
#define LUNAGEN_ANNOTATE(elem) annotate(elem)
#define LUNAGEN_ANNOTATE_ELEMS(...) FOR_EACH(LUNAGEN_ANNOTATE, __VA_ARGS__)
#define LUNAGEN_ATTR_ANNOTATE(...) __attribute__((LUNAGEN_ANNOTATE_ELEMS(__VA_ARGS__)))

// 1. LunaGen Option --> LUNAGEN_ATTR_NAMES
//    Alternative names 
#define LUNAGEN_ATTR_NAMES_ELEM(elem) "lunagen_alt_name:" ## elem
#define LUNAGEN_ATTR_NAMES_ELEMS(...) FOR_EACH(LUNAGEN_ATTR_NAMES_ELEM, __VA_ARGS__)
#define LUNAGEN_ATTR_NAMES(...) LUNAGEN_ATTR_ANNOTATE(LUNAGEN_ATTR_NAMES_ELEMS(__VA_ARGS__))

// 2. LunaGEN Option --> LUNAGEN_ATTR_HIDDEN
//    Hide fields and do not generate bindings for it
#define LUNAGEN_ATTR_HIDDEN LUNAGEN_ATTR_ANNOTATE("lunagen_hidden")

#else
#define LUNAGEN_ATTR_NAMES(...)
#define LUNAGEN_ATTR_HIDDEN
#endif

#define LUNAGEN_DEF_CLASS(classType, name) \
    namespace LunaGen \
    { \
        namespace detail { \
            template<> \
            struct LunaGenTraits<classType> \
            { \
                static constexpr const char clsName[] = #name; \
            }; \
        } \
    }




#endif