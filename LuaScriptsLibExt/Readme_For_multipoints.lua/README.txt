Multipoints is a library that enhances SMBX midpoints. It allows for multiple checkpoints to be placed in a level, and for the player to spawn at a different location to the checkpoint itself. It can also expand checkpoints in other ways, though these are more advanced.

-------------------------------------------------------------------------------------------------------------------------

INSTALLATION

Drop the file "multipoints.lua" and the folder named "multipoints" into the LuaScriptsLib folder in your SMBX installation. Multipoints should now be installed!

-------------------------------------------------------------------------------------------------------------------------

USING MULTIPOINTS:

1. Find where you want to place your checkpoint, and keep track of the coordinates.

2. Import the Multipoints API using the code: multipoints = loadAPI("multipoints")

3. Use the function "addLuaCheckpoint" to create your checkpoint. Pass the coordinates you found in step 1 to place the checkpoints, and fill in the section to spawn in. Optionally, you can specify spawn coordinates that differ from the checkpoint's position, and add a list of Lua function to call when spawning to the checkpoint (this is particularly useful if your Lua program changes state as you progress through the level).

4. You should now have a working checkpoint! You can repeat steps 1-3 as many times as you like to create multiple checkpoints!

ALTERNATIVE METHOD:

The original setup of multipoints required the use of NPCs and events to create checkpoints. This has been kept for compatibility reasons but the newer method introduced in v2.1 is easier to implement and does not require NPCs or events. This alternative method also has some restricted functionality where multiple players are involved.

1. Re-sprite the Axe NPC as a checkpoint. Alternatively, make the Axe NPC invisible, and use a background object re-sprited as a checkpoint. Either way, make sure this object disappears when the player makes contact with the checkpoint.

2. Make the checkpoint Axe call a SMBX event on Death. This event does not need to do anything in SMBX, but make a note of the name. Here, it will be referred to as "GetCheckpoint", but any name will work.

3. Make a layer that hides the Multipoint checkpoint and its hitbox. This will be used when we spawn having already collected the checkpoint. Here, it will be referred to as "HideCheckpointLayer", but any name will work.

4. Jump into lunadll.lua, and import Multipoints as an API (see step 2 in the regular method).

5. Use the function "addCheckpoint" to create a new functioning checkpoint. You must specify the name of the "GetCheckpoint" event, the section containing the Multipoint checkpoint, the spawn location, and the name of the "HideCheckpointLayer" event. You can optionally also specify a list of Lua functions to call when spawning to that checkpoint (this is particularly useful if your Lua program changes state as you progress through the level).

6. You should now have a working checkpoint! You can repeat steps 1-5 as many times as you like to create multiple checkpoints!