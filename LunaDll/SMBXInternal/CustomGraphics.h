#ifndef CustomGraphics_hhhhh
#define CustomGraphics_hhhhh

#include "../Defines.h"
#include "../Misc/RuntimeHook.h"

class SMBX_CustomGraphics
{
public:
    SMBX_CustomGraphics() = delete;
    SMBX_CustomGraphics(SMBX_CustomGraphics& other) = delete;

    static inline void setPlayerHitboxWidth(PowerupID powerupID, Characters characterID, int value) {
        if (characterID >= 1 && characterID <= 5 && powerupID >= 1 && powerupID <= 7) { // overwrite 1.3 memory
            (&GM_HITBOX_W_PTR)[((int)powerupID - 1) * 5 + ((int)characterID - 1)] = value;
        }
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) hitbox->hitbox_w = value;
    }
    static inline void setPlayerHitboxHeight(PowerupID powerupID, Characters characterID, int value) {
        if (characterID >= 1 && characterID <= 5 && powerupID >= 1 && powerupID <= 7) { // overwrite 1.3 memory
            (&GM_HITBOX_H_PTR)[((int)powerupID - 1) * 5 + ((int)characterID - 1)] = value;
        }
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) hitbox->hitbox_h = value;
    }
    static inline void setPlayerHitboxDuckHeight(PowerupID powerupID, Characters characterID, int value) {
        if (characterID >= 1 && characterID <= 5 && powerupID >= 1 && powerupID <= 7) { // overwrite 1.3 memory
            (&GM_HITBOX_H_D_PTR)[((int)powerupID - 1) * 5 + ((int)characterID - 1)] = value;
        }
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) hitbox->hitbox_h_d = value;
    }
    static inline void setPlayerGrabOffsetX(PowerupID powerupID, Characters characterID, int value) {
        if (characterID >= 1 && characterID <= 5 && powerupID >= 1 && powerupID <= 7) { // overwrite 1.3 memory
            (&GM_HITBOX_GRABOFF_X)[((int)powerupID - 1) * 5 + ((int)characterID - 1)] = value;
        }
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) hitbox->hitbox_graboff_x = value;
    }
    static inline void setPlayerGrabOffsetY(PowerupID powerupID, Characters characterID, int value) {
        if (characterID >= 1 && characterID <= 5 && powerupID >= 1 && powerupID <= 7) { // overwrite 1.3 memory
            (&GM_HITBOX_GRABOFF_Y)[((int)powerupID - 1) * 5 + ((int)characterID - 1)] = value;
        }
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) hitbox->hitbox_graboff_y = value;
    }

    static inline int getPlayerHitboxWidth(PowerupID powerupID, Characters characterID) {
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) return hitbox->hitbox_w;
        return 0;
    }
    static inline int getPlayerHitboxHeight(PowerupID powerupID, Characters characterID) {
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) return hitbox->hitbox_h;
        return 0;
    }
    static inline int getPlayerHitboxDuckHeight(PowerupID powerupID, Characters characterID) {
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) return hitbox->hitbox_h_d;
        return 0;
    }
    static inline int getPlayerGrabOffsetX(PowerupID powerupID, Characters characterID) {
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) return hitbox->hitbox_graboff_x;
        return 0;
    }
    static inline int getPlayerGrabOffsetY(PowerupID powerupID, Characters characterID) {
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox) return hitbox->hitbox_graboff_y;
        return 0;
    }

    /*
        This function will return the x coordinates of the specific spriteIndex.
        The spriteIndex is calculated from direction [-1 or 1] * playerSpriteIndex.
    */
    static inline int getXFromSpriteIndex(int spriteIndex) {
        int spIndexX = spriteIndex + 100;
        return native_spritesheetX(&spIndexX);
    }

    /*
        see getXFromSpriteIndex
    */
    static inline int getYFromSpriteIndex(int spriteIndex) {
        int spIndexY = spriteIndex + 100;
        return native_spritesheetY(&spIndexY);
    }

    static inline int convIndexCoorToSpriteIndex(short x, short y){
        return (y + 10 * x) - 49;
    }

    static inline void convSpriteIndexToCoor(short index, short& x, short& y) {
        y = (index + 49) % 10;
        x = ((index + 49) - (index + 49) % 10) / 10;
    }

    static int getOffsetX(Characters characterID, int spriteIndex, PowerupID powerupID, int* retSuccess = 0) {
        if (spriteIndex < -49 || spriteIndex > 50) {
            if (retSuccess) {
                *retSuccess = -1;
            }
            return 0;
        }

        int offsetVal = 0;
        if (retSuccess)
            *retSuccess = 0;

        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox)
        {
            offsetVal = hitbox->gfxoffset_x[spriteIndex+49];
        }
        return offsetVal;
    }

    static int getOffsetY(Characters characterID, int spriteIndex, PowerupID powerupID, int* retSuccess = 0) {
        if (spriteIndex < -49 || spriteIndex > 50) {
            if (retSuccess) {
                *retSuccess = -1;
            }
            return 0;
        }

        int offsetVal = 0;
        if (retSuccess)
            *retSuccess = 0;
        
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox)
        {
            offsetVal = hitbox->gfxoffset_y[spriteIndex + 49];
        }
        return offsetVal;
    }

    static void setOffsetX(Characters characterID, int spriteIndex, PowerupID powerupID, int value, int* retSuccess = 0) {
        if (spriteIndex < -49 || spriteIndex > 50) {
            if (retSuccess) {
                *retSuccess = -1;
            }
            return;
        }

        if (retSuccess)
            *retSuccess = 0;

        switch (characterID)
        {
        case CHARACTER_MARIO:
            GM_GFXOFFSET_MARIO_X[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_LUIGI:
            GM_GFXOFFSET_LUIGI_X[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_PEACH:
            GM_GFXOFFSET_PEACH_X[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_TOAD:
            GM_GFXOFFSET_TOAD_X[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_LINK:
            GM_GFXOFFSET_LINK_X[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        }
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox)
        {
            hitbox->gfxoffset_x[spriteIndex + 49] = value;
        }
    }

    static void setOffsetY(Characters characterID, int spriteIndex, PowerupID powerupID, int value, int* retSuccess = 0) {
        if (spriteIndex < -49 || spriteIndex > 50) {
            if (retSuccess) {
                *retSuccess = -1;
            }
            return;
        }

        if (retSuccess)
            *retSuccess = 0;

        switch (characterID)
        {
        case CHARACTER_MARIO:
            GM_GFXOFFSET_MARIO_Y[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_LUIGI:
            GM_GFXOFFSET_LUIGI_Y[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_PEACH:
            GM_GFXOFFSET_PEACH_Y[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_TOAD:
            GM_GFXOFFSET_TOAD_Y[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        case CHARACTER_LINK:
            GM_GFXOFFSET_LINK_Y[spriteIndex + ((int)powerupID * 100)] = value;
            break;
        }
        CharacterHitBoxData* hitbox = runtimeHookGetCharacterHitBoxData(characterID, powerupID);
        if (hitbox)
        {
            hitbox->gfxoffset_y[spriteIndex + 49] = value;
        }
    }
};

#endif