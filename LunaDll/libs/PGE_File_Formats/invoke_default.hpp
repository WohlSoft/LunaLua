#ifndef INVOKE_DEFAULT_HHHHH
#define INVOKE_DEFAULT_HHHHH
#include <type_traits>
#include <cstddef>
#include <utility>

namespace idef
{
    namespace detail
    {
        template<typename T>
        struct remove_const_ref
        {
            using type = typename std::remove_reference<typename std::remove_const<T>::type>::type;
        };

        template<typename PossibleFunc, typename DefaultFunc>
        struct default_if_nullptr_impl
        {
            using func_type = PossibleFunc;
            static const func_type &Get(const PossibleFunc &pf, const DefaultFunc & /*df*/)
            {
                return pf;
            }
        };

        template<typename DefaultFunc>
        struct default_if_nullptr_impl<std::nullptr_t, DefaultFunc>
        {
            using func_type = DefaultFunc;
            static const func_type &Get(std::nullptr_t /*pf*/, const DefaultFunc &df)
            {
                return df;
            }
        };


        template<typename Func>
        struct invoke_or_noop_impl
        {
            template<typename Ret, typename... Args>
            static Ret invoke(Func &&f, Args &&... args)
            {
                return f(std::forward<Args>(args)...);
            }
        };

        template<>
        struct invoke_or_noop_impl<std::nullptr_t>
        {
            template<typename Ret, typename Func, typename... Args>
            static Ret invoke(Func && /*f*/, Args &&... /*args*/)
            {
                return Ret();
            }
        };
    }

    template<typename PossibleFunc, typename DefaultFunc>
    auto default_if_nullptr(const PossibleFunc &pf, const DefaultFunc &df) ->
    const typename detail::default_if_nullptr_impl<typename detail::remove_const_ref<PossibleFunc>::type, DefaultFunc>::func_type &
    {
        using impl_type = detail::default_if_nullptr_impl<typename detail::remove_const_ref<PossibleFunc>::type, DefaultFunc>;
        return impl_type::Get(pf, df);
    }

    template<typename Ret, typename Func, typename... Args>
    Ret invoke_or_noop(Func f, Args &&... args)
    {
        static_assert(std::is_default_constructible<Ret>::value || std::is_same<Ret, void>::value, "Return value must be default constructible!");
        return detail::invoke_or_noop_impl<Func>::template invoke<Ret>(std::forward<Func>(f), std::forward<Args>(args)...);
    }
}

#endif
