# Submenus DLL

Submenus for SimCity 4.

## Features

- enables submenu functionality: This DLL is a dependency for other submenu-compatible plugins.
- adds a number of standard submenus such as Plazas, Green Spaces, Sports.
- fixes the game's menu rendering issues caused by duplicate or missing menu icons
- implements Exemplar Patching functionality

## System requirements

- SimCity 4, version 1.1.641 (the digital release)
  ([more info](https://community.simtropolis.com/forums/topic/762980-the-future-of-sc4-modding-the-matter-of-digital-vs-disc-and-windows-vs-macos-in-the-dll-era/))
- Windows 7+ or Linux

## Dependencies

- [SC4ResourceLoadingHooks DLL](https://community.simtropolis.com/files/file/36242-resource-loading-hooks-dll-for-simcity-4/)

## Installation

- Copy the DLLs into the top-level directory of either Plugins folder
  (place it directly in `<Documents>\SimCity 4\Plugins` or `<SC4 install folder>\Plugins`, not in a subfolder).
- Copy the file `submenu-essentials.dat` and optionally the individual submenu files anywhere into your Plugins folder.
  You may skip installing an optional submenu if it contains neither Flora nor any sub-submenus.
  In that case, the buildings of that submenu will appear in the default top-level menu instead.

Alternatively, the mod can be installed with Sc4pac: [memo:submenus-dll](https://memo33.github.io/sc4pac/channel/?pkg=memo:submenus-dll)

## Troubleshooting

The plugin should write a `.log` file in the folder containing the plugin.
The log contains status information for the most recent run of the plugin.

If no `.log` file is written, the DLL failed to load.
In this case, make sure the [Microsoft Visual C++ 2022+ x86 Redistributable](https://aka.ms/vs/17/release/vc_redist.x86.exe) is installed.

## Credits

- Submenu icons created by Panda
- Exemplar Patching implemented in collaboration with Null 45

## Standard Submenus

This mod adds the following submenus. Additional menus may be added by third-party mods.

Residential:
- Ploppable R$ (0x93DADFE9)*
- Ploppable R$$ (0x984E5034)*
- Ploppable R$$$ (0x9F83F133)*

Commercial:
- Ploppable CS$ (0x11BF1CA9)*
- Ploppable CS$$ (0x24C43253)*
- Ploppable CS$$$ (0x9BDEFE2B)*
- Ploppable CO$$ (0xA7FF7CF0)*
- Ploppable CO$$$ (0xE27B7EF6)*

Industrial:
- Ploppable Farms (0xC220B7D8)*
- Ploppable Dirty Industry (0x62D82695)*
- Ploppable Manufacturing Industry (0x68B3E5FD)*
- Ploppable High-Tech Industry (0x954E20FE)*

Rail:
- Passenger Rail (0x35380C75)*
- Freight Rail Stations (0x3557F0A1)*
- Yards (0x39BA25C7)* (non-functional lots, sidings, spurs)
- Hybrid Railway (0x2B294CC2)*
- Monorail (0x3A1D9854)*

Misc. Transit:
- Bus (0x1FDDE184)*
- GLR (0x26B51B28)*
- El-Rail (0x244F77E1)*
- Subway (0x231A97D3)*
- Multi-modal Stations (0x322C7959)*
- Parking (0x217B6C35)

Water Transit:
- Seaports and Ferry Terminals (0x07047B22)*
- Canals (0x03C6629C)*
- Seawalls (0x1CD18678)
- Waterfront (0x84D42CD6)*

Power:
- Dirty Energy (0x4B465151)*
- Clean Energy (0xCDE0316B)*
- Miscellaneous Power Utilities (0xD013F32D)* (substations, poles,…)

Police:
- Small (0x65D88585)*
- Large (0x7D6DC8BC)*
- Deluxe (0x8157CA0E)*
- Military (0x8BA49621)

Education:
- Elementary Schools (0x9FE5C428)*
- High Schools (0xA08063D0)* (including private schools)
- Higher Education (0xAC706063)* (colleges and universities)
- Libraries and Museums (0xAEDD9FAA)*

Health:
- Small (0xB1F7AC5B)*
- Medium (0xB7B594D6)*
- Large (0xBC251B69)*

Parks:
- Green Spaces (0xBF776D40)
- Plazas (0xEB75882C)
- Sports Grounds (0xCE21DBEB)
- Paths and Modular Parks (0xDEFFD960)
- Embankments and Retaining Walls (0xBB531946)
- Fillers (0xF034265C) (diagonal fillers, roundabout fillers, industrial fillers, etc.)

Many of these submenus are pre-filled automatically with suitable items from your plugins.
Others remain empty (and therefore hidden) until you install plugins that have been updated for submenu functionality.

Whether an item appears in a submenu can be explicit or implicit:
explicit if the item was modified to use a particular submenu;
implicit if the item belongs to a category associated with a submenu marked with `*`.

------------------------------------------------------------
## Information for modders

Submenus may contain
- buildings,
- flora,
- network items,
- buttons for nested submenus (no hard depth limit).

As the DLL makes use of new properties, plugin files can be made *submenu-compatible*
such that the same plugin file can be used with and without the DLL, making this DLL an *optional* dependency.

### Adding a building to a submenu

To make a building appear in a submenu, add the following property to its building exemplar:

- `Building Submenus` (0xaa1dd399): the Button IDs of the submenus this building appears in if the DLL is loaded.

Without the DLL or if the submenu does not exist or is not installed,
the building instead appears in the original menu as defined by the `OccupantGroups` property (see table below).

(Instead of modifying the building exemplars directly, you may use Exemplar patching to change Exemplar files (see below).
This is useful if you are not the owner of the Exemplar files you want to change, as it does not alter the original files,
so this can help avoid some conflicts.)

### Creating a new submenu button

To create a new submenu, create a copy of an existing submenu button exemplar and change its Instance ID to a new unique ID.
It must be different from any [Occupant Group](https://www.sc4devotion.com/forums/index.php?topic=2378.msg533652#msg533652) in use.
Update the properties `Exemplar Name`, `Item Icon`, `Item Order`, `Item Button ID`, `User Visible Name Key` and `Item Description Key` as needed.

Additionally, adjust the property

- `Item Submenu Parent ID` (0x8a2602ca): the parent Button ID that opens the submenu this item belongs to (see table below).

The following property is required, but does not usually need to be changed:

- `Item Button Class` (0x8a2602cc): set to `1 = Submenu Button`.

The following table lists the Button IDs for existing top-level menus.
The Occupant Groups are used as fallback to add buildings to a specific top-level menu if the DLL is not present.

    Menu            , Submenu Button ID   , Occupant Group
    -------------------------------------------------------
    Flora           , 0x4a22ea06          , -
    Residential     , 0x29920899          , -
    Commercial      , 0xa998af42          , -
    Industrial      , 0xc998af00          , -
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

Note that empty submenus are hidden from the menus.

### Adding flora or network items to a submenu

Add the properties `Item Submenu Parent ID` and `Item Button Class` to the catalog item Exemplar.
The latter property is set to `2 = Network Item in Submenu` or `4 = Flora Item in Submenu`.
(The `Building Submenus` property is not used for these items.)

### Exemplar patching

Exemplar patching is a technique to change the behavior of Exemplar files on-the-fly at runtime, without altering the original DBPF files.
An **Exemplar Patch** is a Cohort file with Group ID 0xb03697d1 (and arbitrary Instance ID) containing the property:

- `Exemplar Patch Targets` (0x0062e78a): list of Exemplar files this patch applies to
  (format: Group ID 1, Instance ID 1, Group ID 2, Instance ID 2,…).
  The list must contain an even number of IDs.

All the other properties of the Cohort file (except for `Exemplar Name`) are injected into these target Exemplar files
whenever the game loads an Exemplar file from this list.
This allows to add or overwrite specific properties of Exemplar files without affecting any unrelated properties.

You can think of an Exemplar Patch as a "Child Cohort" file, mirroring the usual Parent Cohort structure.
An Exemplar Patch can target multiple Exemplar files simultaneously.
Conversely, multiple Exemplar Patches can be applied to the same Exemplar file.


                Parent Cohort
                 /        \
                /          \
               /            \
              /              \
       Exemplar A          Exemplar B
           |   \_____       /   |
           |    _____\_____/    |
           |   /      \______   |
           |  /              \  |
    Exemplar Patch 1     Exemplar Patch 2


If two Cohort files have the same Instance ID, only the one loading last has an effect.
Therefore, you should usually change the Instance ID whenever you copy an existing Exemplar Patch Cohort file.


### Additional resources

- [new_properties.xml](vendor/new_properties.xml) ([diff](https://github.com/memo33/submenus-dll/compare/2e0caee0278d5f4106eca76b797ed323505eef3e..HEAD#diff-95420378434ddd70a81640150126a3fc1e232bf860080961df62d888a2cc4d8b))
  adds names for the new properties to ILive's Reader 1.5.4 if you replace this file located in the Reader install directory
- submenu [icon template](https://github.com/memo33/submenus-dll/releases/tag/1.0.0) for use with GIMP

------------------------------------------------------------
## Information for developers

### Building the plugin

The DLL is compiled using `clang` as a cross-compiler.
Check the [Makefile](Makefile) for details.
```
git submodule update --init
make
```
The source code is mostly compatible with the MSVC compiler as well, but some tweaks may be needed for that.

### License

This project is licensed under the terms of the GNU Lesser General Public License version 3.0.
See [LICENSE.txt](LICENSE.txt) for more information.

#### 3rd party code

- [gzcom-dll](https://github.com/nsgomez/gzcom-dll/tree/master) MIT License
- [Windows Implementation Library](https://github.com/microsoft/wil) MIT License
- [SC4Fix](https://github.com/nsgomez/sc4fix) MIT License
- [NAM-dll](https://github.com/NAMTeam/nam-dll) LGPL 3.0
- [SC4ResourceLoadingHooks](https://github.com/0xC0000054/sc4-resource-loading-hooks) MIT License
