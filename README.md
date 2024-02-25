# Submenus DLL

Submenus for SimCity 4.

## Features

- enables submenu functionality: This DLL is a dependency for other submenu-compatible plugins.
- adds submenus for Plazas, Green Spaces and Sports Grounds to the Parks menu
- fixes the game's duplicate menu icons bug that occurs when loading some plugins, such as the NAM, twice

## System requirements

- SimCity 4, version 1.1.641 (the digital release)
- Windows 10+ or Linux

## Installation

- Copy the DLL into the top-level directory of either Plugins folder.
- Copy the file `parks-submenus.dat` into your Plugins folder.

## Troubleshooting

The plugin should write a `.log` file in the folder containing the plugin.
The log contains status information for the most recent run of the plugin.

If the parks submenus appear empty in the game,
there is a conflicting mod in your Plugins that overwrites properties of the Maxis parks.
Check if a submenu-compatible version of that mod is available
or move the `parks-submenus.dat` to a folder loading later.
In any case, the DLL can still be used as a dependency for other plugins.

------------------------------------------------------------
# Information for modders

Submenus can contain buildings, flora, network items as well as buttons for nested submenus.
As the DLL makes use of new properties, plugin files can be made *submenu-compatible*
such that the same plugin file can be used with and without the DLL, making this DLL an *optional* dependency.

To make a building appear in a submenu, a building exemplar needs two properties:
- `OccupantGroups`: same as always, including the occupant group for a top-level menu
- `OccupantGroupsAlt`: a copy of the above, with the menu-placement occupant group replaced
  by the button ID of the corresponding submenu (see table below).

The `OccupantGroupsAlt` property only affects menu placement, so both properties are needed.
Without the DLL, the building still appears in the original menu as defined by `OccupantGroups`.

To create a new submenu, create an item exemplar with two additional properties:
- `Item Submenu Parent ID`: the parent Button ID that opens the submenu this item belongs to
- `Item Button Class`: set to `1` for Submenu Button.

To add network items or flora to a submenu, set `Item Button Class` to `2` or `4`.

The following table lists existing menus with their corresponding button IDs and occupant groups.

    Menu            , Submenu Button ID   , Occupant Group
    -------------------------------------------------------
    Flora           , 0x4a22ea06          , -
    Road            , 0x6999bf56          , -
    Highway         , 0x31                , -
    Rail            , 0x29                , 0x1300
    Misc. Transit   , 0x299237bf          , 0x130a
    Airport         , 0xe99234b3          , 0x1508
    Water Transit   , 0xa99234a6          , 0x1519
    Power           , 0x35                , 0x1400
    Water           , 0x39                , 0x1401
    Garbage         , 0x40                , 0x1402
    Police          , 0x37                , 0x1500
    Fire            , 0x38                , 0x1502
    Education       , 0x42                , 0x1503
    Health          , 0x89dd5405          , 0x1507
    Landmark        , 0x09930709          , 0x150a
    Reward          , 0x34                , 0x150b
    Park            , 0x3                 , 0x1006
    Plazas          , 0xeb75882c          , -
    Sports Grounds  , 0xce21dbeb          , -
    Green Spaces    , 0xbf776d40          , -

## Additional resources

- [new_properties.xml](https://github.com/memo33/submenus-dll/blob/main/vendor/new_properties.xml) [(diff)](https://github.com/memo33/submenus-dll/compare/2e0caee0278d5f4106eca76b797ed323505eef3e..HEAD#diff-95420378434ddd70a81640150126a3fc1e232bf860080961df62d888a2cc4d8b)
  adds names for the new properties to ILive's Reader 1.5.4 if you replace this file located in the Reader install directory
- submenu [icon template](https://github.com/memo33/submenus-dll/releases/tag/1.0.0) for use with GIMP

------------------------------------------------------------
# Information for developers

## Building the plugin

The DLL is compiled using `clang` as a cross-compiler.
Check the [Makefile](Makefile) for details.
```
make
```
The source code is mostly compatible with the MSVC compiler as well, but some tweaks may be needed for that.

## License

This project is licensed under the terms of the GNU Lesser General Public License version 3.0.
See [LICENSE.txt](LICENSE.txt) for more information.

### 3rd party code

- [gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) Located in the vendor folder, MIT License.
- [Windows Implementation Library](https://github.com/microsoft/wil) MIT License
- [SC4Fix](https://github.com/nsgomez/sc4fix) MIT License
- [NAM-dll](https://github.com/NAMTeam/nam-dll) LGPL 3.0
