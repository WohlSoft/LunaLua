TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += dll
CONFIG += static

TARGET = LunaDLL

QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-write-strings -mwindows

DEFINES += LunaDLL_LIBRARY LunaDLL_EXPORTS
DEFINES += DX_EXPORT=Q_DECL_EXPORT _USRDLL LUNADLL_EXPORTS

INCLUDEPATH += $$PWD/LunaDLL
INCLUDEPATH += $$PWD/LunaDLL/libs/lua_mingw/include
INCLUDEPATH += $$PWD/LunaDll/libs/luabind-include
INCLUDEPATH += $$PWD/LunaDll/libs/sdl_mingw/include
LIBS += -L$$PWD/LunaDLL/libs/lua_mingw/lib
LIBS += -L$$PWD/LunaDLL/libs/sdl_mingw/lib
LIBS += -static -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lmsimg32 -ldsound -lwinspool -ladvapi32 -lole32 -loleaut32 -static -llua5.1
LIBS += -lwinmm -static -lSDL2main -lSDL2.dll -static -lSDL2_mixer.dll libversion -lDbghelp -lvorbisfile -lvorbis -lmad -lmikmod.dll -lflac -logg

SOURCES += \
    LunaDll/AC_HeartSystem.cpp \
    LunaDll/AC_LunaControl.cpp \
    LunaDll/Autocode.cpp \
    LunaDll/AutocodeManager.cpp \
    LunaDll/Blocks.cpp \
    LunaDll/BMPBox.cpp \
    LunaDll/CellManager.cpp \
    LunaDll/CSprite.cpp \
    LunaDll/CSpriteManager.cpp \
    LunaDll/DeathCounter.cpp \
    LunaDll/DeathRecord.cpp \
    LunaDll/dlltestlvlCode.cpp \
    LunaDll/Docopoper-AbstractAssault.lvl.cpp \
    LunaDll/Docopoper-Calleoca.cpp \
    LunaDll/Docopoper-TheFloorisLava.lvl..cpp \
    LunaDll/EnumCmd.cpp \
    LunaDll/EuroShellRandD.lvl.cpp \
    LunaDll/GenComp.cpp \
    LunaDll/Globals.cpp \
    LunaDll/Hitbox.cpp \
    LunaDll/Input.cpp \
    LunaDll/JosephStaleknight-CurtainTortoise.lvl.cpp \
    LunaDll/Keyboard.cpp \
    LunaDll/Kil-DemosBrain.cpp \
    LunaDll/Layer.cpp \
    LunaDll/Level.cpp \
    LunaDll/Logger.cpp \
    LunaDll/Main.cpp \
    LunaDll/MiscFuncs.cpp \
    LunaDll/NPCs.cpp \
    LunaDll/PlayerMOB.cpp \
    LunaDll/RenderBitmapOp.cpp \
    LunaDll/Rendering.cpp \
    LunaDll/RenderRectOp.cpp \
    LunaDll/SAJewers-QraestoliaCaverns.lvl.cpp \
    LunaDll/SAJewers-Snowboardin.cpp \
    LunaDll/SMBXEvent.cpp \
    LunaDll/Sound.cpp \
    LunaDll/SpriteBehaviorFuncs.cpp \
    LunaDll/SpriteComponent.cpp \
    LunaDll/SpriteDrawFuncs.cpp \
    LunaDll/Talkhaus-Science_Final_Battle.cpp \
    LunaDll/LuaMain/LunaLuaMain.cpp \
    LunaDll/LuaMain/LuaHelper.cpp \
    LunaDll/LuaMain/LuaProxy.cpp \
    LunaDll/CGUIContainer.cpp \
    LunaDll/GameboyRPG.cpp \
    LunaDll/KilArmoryCode.cpp \
    LunaDll/Minigames.cpp \
    LunaDll/PerformanceTimer.cpp \
    LunaDll/RenderEffectOp.cpp \
    LunaDll/SMBXEvents.cpp \
    LunaDll/UserSaving.cpp \
    LunaDll/LuaMain/LuaEvents.cpp \
    LunaDll/Animation.cpp \
    LunaDll/GlobalFuncs.cpp \
    LunaDll/RuntimeHook.cpp \
    LunaDll/mciEmulator.cpp \
    LunaDll/Overworld.cpp \
    LunaDll/WorldLevel.cpp \
    LunaDll/SdlMusic/MusicDefList.cpp \
    LunaDll/SdlMusic/MusicManager.cpp \
    LunaDll/SdlMusic/SdlMusPlayer.cpp \
    LunaDll/libs/ini-reader/INIReader.cpp \
    LunaDll/libs/ini-reader/ini.c \
    LunaDll/libs/luabind-src/class.cpp \
    LunaDll/libs/luabind-src/class_info.cpp \
    LunaDll/libs/luabind-src/class_registry.cpp \
    LunaDll/libs/luabind-src/class_rep.cpp \
    LunaDll/libs/luabind-src/create_class.cpp \
    LunaDll/libs/luabind-src/error.cpp \
    LunaDll/libs/luabind-src/exception_handler.cpp \
    LunaDll/libs/luabind-src/function.cpp \
    LunaDll/libs/luabind-src/inheritance.cpp \
    LunaDll/libs/luabind-src/link_compatibility.cpp \
    LunaDll/libs/luabind-src/object_rep.cpp \
    LunaDll/libs/luabind-src/open.cpp \
    LunaDll/libs/luabind-src/pcall.cpp \
    LunaDll/libs/luabind-src/scope.cpp \
    LunaDll/libs/luabind-src/stack_content_by_name.cpp \
    LunaDll/libs/luabind-src/weak_ref.cpp \
    LunaDll/libs/luabind-src/wrapper_base.cpp \
    LunaDll/PngRender.cpp

HEADERS += \
    LunaDll/Autocode.h \
    LunaDll/AutocodeManager.h \
    LunaDll/Blocks.h \
    LunaDll/BMPBox.h \
    LunaDll/CellManager.h \
    LunaDll/CSprite.h \
    LunaDll/CSpriteManager.h \
    LunaDll/DeathCounter.h \
    LunaDll/DeathRecord.h \
    LunaDll/Defines.h \
    LunaDll/Globals.h \
    LunaDll/Hitbox.h \
    LunaDll/Input.h \
    LunaDll/Keyboard.h \
    LunaDll/Layer.h \
    LunaDll/Level.h \
    LunaDll/LevelCodes.h \
    LunaDll/Logging.h \
    LunaDll/Main.h \
    LunaDll/MiscFuncs.h \
    LunaDll/NPCs.h \
    LunaDll/PlayerMOB.h \
    LunaDll/RenderBitmapOp.h \
    LunaDll/Rendering.h \
    LunaDll/RenderOp.h \
    LunaDll/RenderRectOp.h \
    LunaDll/RenderString.h \
    LunaDll/SMBXEvents.h \
    LunaDll/Sound.h \
    LunaDll/SpriteComponent.h \
    LunaDll/SpriteFuncs.h \
    LunaDll/TriggerZone.h \
    LunaDll/LuaMain/LunaLuaMain.h \
    LunaDll/LuaMain/LuaHelper.h \
    LunaDll/LuaMain/LuaProxy.h \
    LunaDll/libs/luabind-include/luabind/detail/calc_arity.hpp \
    LunaDll/libs/luabind-include/luabind/detail/call.hpp \
    LunaDll/libs/luabind-include/luabind/detail/call_function.hpp \
    LunaDll/libs/luabind-include/luabind/detail/call_member.hpp \
    LunaDll/libs/luabind-include/luabind/detail/call_operator_iterate.hpp \
    LunaDll/libs/luabind-include/luabind/detail/class_cache.hpp \
    LunaDll/libs/luabind-include/luabind/detail/class_registry.hpp \
    LunaDll/libs/luabind-include/luabind/detail/class_rep.hpp \
    LunaDll/libs/luabind-include/luabind/detail/compute_score.hpp \
    LunaDll/libs/luabind-include/luabind/detail/constructor.hpp \
    LunaDll/libs/luabind-include/luabind/detail/conversion_storage.hpp \
    LunaDll/libs/luabind-include/luabind/detail/convert_to_lua.hpp \
    LunaDll/libs/luabind-include/luabind/detail/debug.hpp \
    LunaDll/libs/luabind-include/luabind/detail/decorate_type.hpp \
    LunaDll/libs/luabind-include/luabind/detail/deduce_signature.hpp \
    LunaDll/libs/luabind-include/luabind/detail/enum_maker.hpp \
    LunaDll/libs/luabind-include/luabind/detail/format_signature.hpp \
    LunaDll/libs/luabind-include/luabind/detail/garbage_collector.hpp \
    LunaDll/libs/luabind-include/luabind/detail/has_get_pointer.hpp \
    LunaDll/libs/luabind-include/luabind/detail/inheritance.hpp \
    LunaDll/libs/luabind-include/luabind/detail/instance_holder.hpp \
    LunaDll/libs/luabind-include/luabind/detail/is_indirect_const.hpp \
    LunaDll/libs/luabind-include/luabind/detail/link_compatibility.hpp \
    LunaDll/libs/luabind-include/luabind/detail/make_instance.hpp \
    LunaDll/libs/luabind-include/luabind/detail/most_derived.hpp \
    LunaDll/libs/luabind-include/luabind/detail/object_call.hpp \
    LunaDll/libs/luabind-include/luabind/detail/object_funs.hpp \
    LunaDll/libs/luabind-include/luabind/detail/object_rep.hpp \
    LunaDll/libs/luabind-include/luabind/detail/open.hpp \
    LunaDll/libs/luabind-include/luabind/detail/operator_id.hpp \
    LunaDll/libs/luabind-include/luabind/detail/other.hpp \
    LunaDll/libs/luabind-include/luabind/detail/pcall.hpp \
    LunaDll/libs/luabind-include/luabind/detail/pointee_sizeof.hpp \
    LunaDll/libs/luabind-include/luabind/detail/pointee_typeid.hpp \
    LunaDll/libs/luabind-include/luabind/detail/policy.hpp \
    LunaDll/libs/luabind-include/luabind/detail/primitives.hpp \
    LunaDll/libs/luabind-include/luabind/detail/property.hpp \
    LunaDll/libs/luabind-include/luabind/detail/ref.hpp \
    LunaDll/libs/luabind-include/luabind/detail/signature_match.hpp \
    LunaDll/libs/luabind-include/luabind/detail/stack_utils.hpp \
    LunaDll/libs/luabind-include/luabind/detail/typetraits.hpp \
    LunaDll/libs/luabind-include/luabind/detail/yes_no.hpp \
    LunaDll/libs/luabind-include/luabind/adopt_policy.hpp \
    LunaDll/libs/luabind-include/luabind/back_reference.hpp \
    LunaDll/libs/luabind-include/luabind/back_reference_fwd.hpp \
    LunaDll/libs/luabind-include/luabind/class.hpp \
    LunaDll/libs/luabind-include/luabind/class_info.hpp \
    LunaDll/libs/luabind-include/luabind/config.hpp \
    LunaDll/libs/luabind-include/luabind/container_policy.hpp \
    LunaDll/libs/luabind-include/luabind/copy_policy.hpp \
    LunaDll/libs/luabind-include/luabind/dependency_policy.hpp \
    LunaDll/libs/luabind-include/luabind/discard_result_policy.hpp \
    LunaDll/libs/luabind-include/luabind/error.hpp \
    LunaDll/libs/luabind-include/luabind/exception_handler.hpp \
    LunaDll/libs/luabind-include/luabind/from_stack.hpp \
    LunaDll/libs/luabind-include/luabind/function.hpp \
    LunaDll/libs/luabind-include/luabind/get_main_thread.hpp \
    LunaDll/libs/luabind-include/luabind/get_pointer.hpp \
    LunaDll/libs/luabind-include/luabind/handle.hpp \
    LunaDll/libs/luabind-include/luabind/iterator_policy.hpp \
    LunaDll/libs/luabind-include/luabind/lua_include.hpp \
    LunaDll/libs/luabind-include/luabind/luabind.hpp \
    LunaDll/libs/luabind-include/luabind/make_function.hpp \
    LunaDll/libs/luabind-include/luabind/nil.hpp \
    LunaDll/libs/luabind-include/luabind/object.hpp \
    LunaDll/libs/luabind-include/luabind/open.hpp \
    LunaDll/libs/luabind-include/luabind/operator.hpp \
    LunaDll/libs/luabind-include/luabind/out_value_policy.hpp \
    LunaDll/libs/luabind-include/luabind/prefix.hpp \
    LunaDll/libs/luabind-include/luabind/raw_policy.hpp \
    LunaDll/libs/luabind-include/luabind/return_reference_to_policy.hpp \
    LunaDll/libs/luabind-include/luabind/scope.hpp \
    LunaDll/libs/luabind-include/luabind/shared_ptr_converter.hpp \
    LunaDll/libs/luabind-include/luabind/tag_function.hpp \
    LunaDll/libs/luabind-include/luabind/typeid.hpp \
    LunaDll/libs/luabind-include/luabind/value_wrapper.hpp \
    LunaDll/libs/luabind-include/luabind/version.hpp \
    LunaDll/libs/luabind-include/luabind/weak_ref.hpp \
    LunaDll/libs/luabind-include/luabind/wrapper_base.hpp \
    LunaDll/libs/luabind-include/luabind/yield_policy.hpp \
    LunaDll/CGUI.h \
    LunaDll/CGUIContainer.h \
    LunaDll/GameboyRPG.h \
    LunaDll/Minigames.h \
    LunaDll/PerformanceTimer.h \
    LunaDll/PngRender.h \
    LunaDll/RenderEffectOp.h \
    LunaDll/UserSaving.h \
    LunaDll/LuaMain/LuaEvents.h \
    LunaDll/Animation.h \
    LunaDll/RuntimeHook.h \
    LunaDll/WorldLevel.h \
    LunaDll/GlobalFuncs.h \
    LunaDll/Overworld.h \
    LunaDll/mciEmulator.h \
    LunaDll/SdlMusic/MusicManager.h \
    LunaDll/SdlMusic/SdlMusPlayer.h \
    LunaDll/libs/ini-reader/ini.h \
    LunaDll/libs/ini-reader/INIReader.h

OTHER_FILES += \
    LunaDll/scratch.txt

DISTFILES += \
    LunaDll/scrappedcode.txt

