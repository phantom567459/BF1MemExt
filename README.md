Tool for patching SWBF1 (2004) to extend it's memory limits.

## Current Patches

- DLC Mission Limit Extension. Increased from 50 to 4096 (theoretically). This lets more mods be installed at once, provided that don't conflict and no other limits are hit.
- Spawn Screen Fix for BF1.  Allows up to 10 units on the Spawn Screen simultaneously when combined with updates to ifs_pc_spawnselect Lua file in common.lvl

## Supported Versions

- [SWBFspy Patched Version](https://info.swbfgamers.com/downloads.html)
> Using the link above, the disk version of the game or the Steam version of the game can be patched.
- SPTest (The version of the game used to debug mods. Found in the modtools.)
> Not completely supported yet

If you're interested in seeing another version of the game supported feel free to open an Issue (or +1 an Issue if someone else has already asked for your version to be supported).

## Usage

The tool itself is a simple Win32 GUI app. Launch it, click "Patch Executable", browse to your game's executable (the one named `Battlefront.exe` and is in the same folder as your `Addon` folder) and click Open. The tool will then patch the executable, if it recognizes the executable and is able to patch it you'll get a success message.

If it fails the executable will left unmodified. Replacing it is the final step it does after everything else has succeeded.
