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
LIBS += -static -lkernel32 -static -luser32 -static -lgdi32 -static -lcomdlg32 -static -lmsimg32 #-static -lcomsuppw
LIBS += -static -lmsimg32 -static -ldsound -static -lwinspool -static -ladvapi32 -static -lole32 -static -loleaut32 -static -llua5.1
LIBS += -static -lwinmm -static -lSDL2main -lSDL2.dll -static -lSDL2_mixer.dll libversion -lDbghelp -lvorbisfile -lvorbis -lmad -lmikmod.dll -lflac -logg

SOURCES += \
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
    LunaDll/BMPBox.cpp \
    LunaDll/CGUIContainer.cpp \
    LunaDll/dlltestlvlCode.cpp \
    LunaDll/Docopoper-AbstractAssault.lvl.cpp \
    LunaDll/Docopoper-Calleoca.cpp \
    LunaDll/Docopoper-TheFloorisLava.lvl..cpp \
    LunaDll/EuroShellRandD.lvl.cpp \
    LunaDll/GameboyRPG.cpp \
    LunaDll/GlobalFuncs.cpp \
    LunaDll/Globals.cpp \
    LunaDll/HardcodedGraphicsManager.cpp \
    LunaDll/Input.cpp \
    LunaDll/JosephStaleknight-CurtainTortoise.lvl.cpp \
    LunaDll/Keyboard.cpp \
    LunaDll/KilArmoryCode.cpp \
    LunaDll/Kil-DemosBrain.cpp \
    LunaDll/Layer.cpp \
    LunaDll/Level.cpp \
    LunaDll/Logger.cpp \
    LunaDll/Main.cpp \
    LunaDll/mciEmulator.cpp \
    LunaDll/Minigames.cpp \
    LunaDll/MiscFuncs.cpp \
    LunaDll/NPCs.cpp \
    LunaDll/Overworld.cpp \
    LunaDll/PerformanceTimer.cpp \
    LunaDll/PlayerMOB.cpp \
    LunaDll/PngRender.cpp \
    LunaDll/RenderBitmapOp.cpp \
    LunaDll/RenderEffectOp.cpp \
    LunaDll/Rendering.cpp \
    LunaDll/RenderRectOp.cpp \
    LunaDll/RuntimeHook.cpp \
    LunaDll/SAJewers-QraestoliaCaverns.lvl.cpp \
    LunaDll/SAJewers-Snowboardin.cpp \
    LunaDll/SMBXEvent.cpp \
    LunaDll/SMBXEvents.cpp \
    LunaDll/Sound.cpp \
    LunaDll/Talkhaus-Science_Final_Battle.cpp \
    LunaDll/UserSaving.cpp \
    LunaDll/WorldLevel.cpp \
    LunaDll/Animation/Animation.cpp \
    LunaDll/Autocode/Autocode.cpp \
    LunaDll/Autocode/AutocodeManager.cpp \
    LunaDll/Autocode/Commands/AC_HeartSystem.cpp \
    LunaDll/Autocode/Commands/AC_LunaControl.cpp \
    LunaDll/Autocode/Commands/EnumCmd.cpp \
    LunaDll/Autocode/Commands/GenComp.cpp \
    LunaDll/Blocks/Blocks.cpp \
    LunaDll/CellManager/CellManager.cpp \
    LunaDll/CustomSprites/CSprite.cpp \
    LunaDll/CustomSprites/CSpriteManager.cpp \
    LunaDll/CustomSprites/SpriteComponent.cpp \
    LunaDll/CustomSprites/SpritesFuncs/SpriteBehaviorFuncs.cpp \
    LunaDll/CustomSprites/SpritesFuncs/SpriteDrawFuncs.cpp \
    LunaDll/CustomSprites/Hitbox/Hitbox.cpp \
    LunaDll/DeathCounter/DeathCounter.cpp \
    LunaDll/DeathCounter/DeathRecord.cpp \
    LunaDll/LuaMain/LuaEvents.cpp \
    LunaDll/LuaMain/LuaHelper.cpp \
    LunaDll/LuaMain/LuaProxy.cpp \
    LunaDll/LuaMain/LunaLuaMain.cpp \
    LunaDll/SdlMusic/MusicDefList.cpp \
    LunaDll/SdlMusic/MusicManager.cpp \
    LunaDll/SdlMusic/SdlMusPlayer.cpp

HEADERS += \
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
    LunaDll/BMPBox.h \
    LunaDll/CGUI.h \
    LunaDll/CGUIContainer.h \
    LunaDll/Defines.h \
    LunaDll/GameboyRPG.h \
    LunaDll/GlobalFuncs.h \
    LunaDll/Globals.h \
    LunaDll/HardcodedGraphicsManager.h \
    LunaDll/Input.h \
    LunaDll/Keyboard.h \
    LunaDll/Layer.h \
    LunaDll/Level.h \
    LunaDll/LevelCodes.h \
    LunaDll/Logging.h \
    LunaDll/Main.h \
    LunaDll/mciEmulator.h \
    LunaDll/Minigames.h \
    LunaDll/MiscFuncs.h \
    LunaDll/NPCs.h \
    LunaDll/Overworld.h \
    LunaDll/PerformanceTimer.h \
    LunaDll/PlayerMOB.h \
    LunaDll/PngRender.h \
    LunaDll/RenderBitmapOp.h \
    LunaDll/RenderEffectOp.h \
    LunaDll/Rendering.h \
    LunaDll/RenderOp.h \
    LunaDll/RenderRectOp.h \
    LunaDll/RenderString.h \
    LunaDll/RuntimeHook.h \
    LunaDll/SMBXEvents.h \
    LunaDll/Sound.h \
    LunaDll/TriggerZone.h \
    LunaDll/UserSaving.h \
    LunaDll/WorldLevel.h \
    LunaDll/Animation/Animation.h \
    LunaDll/Autocode/Autocode.h \
    LunaDll/Autocode/AutocodeManager.h \
    LunaDll/Blocks/Blocks.h \
    LunaDll/CellManager/CellManager.h \
    LunaDll/CustomSprites/CSprite.h \
    LunaDll/CustomSprites/CSpriteManager.h \
    LunaDll/CustomSprites/SpriteComponent.h \
    LunaDll/CustomSprites/SpritesFuncs/SpriteFuncs.h \
    LunaDll/CustomSprites/Hitbox/Hitbox.h \
    LunaDll/DeathCounter/DeathCounter.h \
    LunaDll/DeathCounter/DeathRecord.h \
    LunaDll/LuaMain/LuaEvents.h \
    LunaDll/LuaMain/LuaHelper.h \
    LunaDll/LuaMain/LuaProxy.h \
    LunaDll/LuaMain/LunaLuaMain.h \
    LunaDll/SdlMusic/MusicManager.h \
    LunaDll/SdlMusic/SdlMusPlayer.h

OTHER_FILES += \
    LunaDll/scratch.txt

DISTFILES += \
    LunaDll/scrappedcode.txt

