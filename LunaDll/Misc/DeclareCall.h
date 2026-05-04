#ifndef DECLARECALL_H_
#define DECLARECALL_H_

#ifdef SYSCALLS_IMPL
    #include <Windows.h>
    #include <cstdint>

    #ifdef __clang__
        // Auxiliary macros to only expand __COUNTER__ once
        #define DECLARE_CALL_AUX(counter, ...) DECLARE_CALL_AUX2(counter, __VA_ARGS__)
        #define DECLARE_CALL_AUX2(counter, ...) DECLARE_CALL_AUX3(functionAddr ## counter, __VA_ARGS__)
        
        // NB: I'm using AT&T syntax because of a clang bug: https://github.com/llvm/llvm-project/issues/60893
        #define DECLARE_CALL_AUX3(functionAddr, ...)        \
        static std::uintptr_t functionAddr = 0;             \
        __declspec(naked) __VA_ARGS__ {                     \
            __asm__ volatile (                              \
                "movl %[FunctionAddr], %%eax\n"             \
                "testl %%eax, %%eax\n"                      \
                "jnz 1f\n"                                  \
                "movl %[ModuleHandle], %%eax\n"             \
                "testl %%eax, %%eax\n"                      \
                "jnz 2f\n"                                  \
                "push %[DllName]\n"                         \
                "call %P[GetModuleHandleW]\n"               \
                "movl %%eax, %[ModuleHandle]\n"             \
                "2: push %[FunctionName]\n"                 \
                "push %%eax\n"                              \
                "call %P[GetProcAddress]\n"                 \
                "movl %%eax, %[FunctionAddr]\n"             \
                "1: jmp *%%eax\n"                           \
                :                                           \
                : [DllName] "i" (dllName),                  \
                [FunctionName] "i" (&__func__),             \
                [GetModuleHandleW] "s" (&GetModuleHandleW), \
                [GetProcAddress] "s" (&GetProcAddress),     \
                [FunctionAddr] "m" (functionAddr),          \
                [ModuleHandle] "m" (moduleHandle)           \
            );                                              \
        }

        #define DECLARE_CALL(...) DECLARE_CALL_AUX(__COUNTER__, __VA_ARGS__)
        
    #else
        #define DECLARE_CALL(...)                               \
        __declspec(naked) __VA_ARGS__ {                         \
            static char functionName[] = __func__;              \
            static std::uintptr_t functionAddr = 0;             \
            __asm {                                             \
                __asm mov eax, functionAddr                     \
                __asm test eax, eax                             \
                __asm jnz funcExists                            \
                __asm mov eax, moduleHandle                     \
                __asm test eax, eax                             \
                __asm jnz moduleExists                          \
                __asm push offset dllName                       \
                __asm call GetModuleHandleW                     \
                __asm mov moduleHandle, eax                     \
                __asm moduleExists: push offset functionName    \
                __asm push eax                                  \
                __asm call GetProcAddress                       \
                __asm mov functionAddr, eax                     \
                __asm funcExists: jmp eax                       \
            }                                                   \
        }
    #endif
    
    #define IMPORT_FROM(dllFilename)                                    \
    namespace dllFilename {                                             \
        static HMODULE moduleHandle = NULL;                             \
        static constexpr wchar_t dllName[] = L ## #dllFilename ".dll";  \
    }                                                                   \
    namespace dllFilename
        
#else
    #define DECLARE_CALL(...) __VA_ARGS__;

    #define IMPORT_FROM(dllFilename) namespace dllFilename
#endif

#endif