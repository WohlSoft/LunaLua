
#define QUOTE(str) #str
#define MACRO_TOSTRING(str) QUOTE(str)
#define TO_VER(num1, num2, num3, num4) num1 ## . ## num2 ## . ## num3 ## . ## num4
#define TO_VER_STR(num1, num2, num3, num4) MACRO_TOSTRING(TO_VER(num1, num2, num3, num4))

#define LUNA_VERNUM1                    0
#define LUNA_VERNUM2                    7
#define LUNA_VERNUM3                    2
#define LUNA_VERNUM4                    2

#define LUNALUA_VER TO_VER(LUNA_VERNUM1, LUNA_VERNUM2, LUNA_VERNUM3, LUNA_VERNUM4)
#define LUNALUA_VER_STR TO_VER_STR(LUNA_VERNUM1, LUNA_VERNUM2, LUNA_VERNUM3, LUNA_VERNUM4)

