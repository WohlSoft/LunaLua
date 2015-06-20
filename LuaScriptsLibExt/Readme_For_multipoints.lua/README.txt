Multipoints is a library that enhances SMBX midpoints. It allows for multiple midpoints to be placed in a level, and for the player to spawn at a different location to the midpoint itself. It can also expand midpoints in other ways, though these are more advanced. It requires a little bit of setup, but the steps involved are listed here.

--------------------------------------------------------------------------------------------------------------------------
THIS LIBRARY REQUIRES THE LUA TRIGGER LIBRARY (NOT INCLUDED, DOWNLOAD HERE: http://engine.wohlnet.ru/pgewiki/Triggers.lua)
--------------------------------------------------------------------------------------------------------------------------

0. Install the Trigger library, and place the "multipoints.lua" file in your Lua directory.

1. Place a single SMBX midpoint on the ground in a location separate from the rest of the level. This can be in its own section, or in any point unreachable from playing the level. It is recommended to re-sprite the midpoint so that it is invisible.

2. Create a Lua Trigger using the Trigger library (see the Trigger library "how to use" document). Make sure you know the ID of the NPC used to create the Trigger (an Axe is recommended). You do not need to do steps 6-9 in the "how to use" document, as these are done by the Multipoint library.

3. Re-sprite the Axe NPC as a midpoint. Alternatively, make the Axe NPC invisible, and use a background object re-sprited as a midpoint. Either way, make sure this object disappears when the player makes contact with the midpoint.

4. Make the midpoint Axe call the Trigger (step 2) on Death.

5. Make a layer that hides the Multipoint midpoint and its hitbox. This will be used when we spawn having already collected the midpoint. Here, it will be referred to as "HideMidpointLayer", but any name will work.

6. Jump into lunadll.lua, and import Multipoints as an API. If you used an NPC other than the Axe as your Trigger, you will need to tell Multipoints using the triggerID value.

7. Use the function addMidpoint to create a new functioning midpoint. You must specify the coordinates of the Trigger NPC, the section containing the Multipoint midpoint, the spawn location, and the name of the "HideMidpointLayer" event. You can optionally also specify a list of Lua functions to call when spawning to that midpoint.

8. You should now have a working midpoint! You can repeat steps 2-7 as many times as you like to create multiple midpoints!
