// Portions of this file have been adapted from the nam-dll project:
/*
 * Copyright (c) 2023 NAM Team contributors
 *
 * nam-dll is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * nam-dll is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with nam-dll.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Logger.h"
#include "SC4VersionDetection.h"
#include "version.h"
#include "cIGZCOM.h"
#include "cRZCOMDllDirector.h"
#include "cIGZMessage2.h"
#include "cIGZMessage2Standard.h"
#include "cRZMessage2COMDirector.h"
#include "cIGZMessageServer2.h"
#include "GZServPtrs.h"
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"
#include <unordered_set>
#include <stack>
/* #include <iostream> */

#include "cGZPersistResourceKey.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZPersistResourceKeyList.h"
#include "cISCResExemplar.h"
#include "cISCPropertyHolder.h"
#include "cISCProperty.h"
#include "cIGZVariant.h"
#include "cIGZPersistResourceKeyList.h"
#include "cISC4BuildingDevelopmentSimulator.h"
#include "cISC4City.h"
#include "cISC4App.h"
#include "PersistResourceKeyFilterByTypeAndGroup.h"
#include "cIExemplarLoadHookServer.h"
#include "ExemplarPatcher.h"
#include "PropertyUtil.h"
#include "cRZAutoRefCount.h"
#include "MenuIds.h"
#include "Categorization.h"


#ifdef __clang__
#define NAKED_FUN __attribute__((naked))
#else
#define NAKED_FUN __declspec(naked)
#endif

// an alternative occupant groups property used for menu placement
#define OCCUPANT_GROUPS_ALT_PROP 0xaa1dd399
#define OCCUPANT_GROUPS_PROP 0xaa1dd396
#define ITEM_BUTTON_ID_PROP 0x8a2602bb
#define ITEM_SUBMENU_PROP 0x8a2602ba
// the button ID that opens the submenu this item belongs to (replaces ITEM_SUBMENU_PROP)
#define ITEM_SUBMENU_PARENT_ID_PROP 0x8a2602ca
// this is a marker property for the submenu button exemplars as well as network items within nested submenus
#define ITEM_BUTTON_CLASS_PROP 0x8a2602cc
#define ITEM_BUTTON_CLASS_DISABLED 0x00000000
#define ITEM_BUTTON_CLASS_SUBMENU 0x00000001
#define ITEM_BUTTON_CLASS_NETWORK 0x00000002
#define ITEM_BUTTON_CLASS_FLORA 0x00000004  // currently not enforced (subject to change)

// the catalog item type of submenu buttons
#define SUBMENU_ITEM_TYPE 0x9ab38dac
// a replacement for the above item type for passing around in messages (might be removed in the future)
#define VIRTUAL_SUBMENU_ITEM_TYPE 0x9a6c1235

// a range of button IDs that could be added in the transport menu
#define TRANSPORT_EXTRA_MENU_MIN 0x9a6c1200
#define TRANSPORT_EXTRA_MENU_MAX 0x9a6c12ff

// secondary menu PNG IIDs that determine the color of the (top-level) menu frame
#define FLORA_MENU_ID 0x14215ed0
#define ZONE_MENU_ID 0x14215ed1
#define TRANSPORT_MENU_ID 0x14215ed2
#define UTILITY_MENU_ID 0x14215ed3
#define CIVIC_MENU_ID 0x14215ed4

// item types of menu items
#define TOOL_PLOP_NETWORK 0x4a2b2467
#define TOOL_PLOP_ZONE 0x4a2b2466
#define TOOL_PLOP_FLORA 0xea5ad08d

#define ITEM_EXEMPLAR_GID_MISC_CATALOG 0x2a3858e4
#define ITEM_EXEMPLAR_GID_FLORA 0xe83e0437
#define EXEMPLAR_TID 0x6534284a


static constexpr uint32_t kSubmenusDllDirectorID = 0xc0583f3b;

static constexpr std::string_view PluginLogFileName = "memo.submenus.log";

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePostCityShutdown = 0x26D31EC3;

static uint32_t cSCPropertyHelp_GetPropertyValue = 0x5fd480;
static uint32_t nSC4UI_CreateCatalogItemList = 0x78ef30;
static uint32_t DoTransportMenu_addOG = 0x41bfd0;

static uint32_t WinCatalogView_OnMouseUpL_InjectPoint = 0x79a984;
static uint32_t WinCatalogView_OnMouseUpL_ContinueJump = 0x79a98d;

static uint32_t HandleButtonActivated_InjectPoint = 0x7f46aa;
static uint32_t HandleButtonActivated_ContinueJump = 0x7f46b0;
static uint32_t HandleButtonActivated_ContinueJump_Transport = 0x7f4add;

static uint32_t HandleButtonActivated2_InjectPoint = 0x7f4eb5;
static uint32_t HandleButtonActivated2_ContinueJump = 0x7f4ebe;

static uint32_t DoUtilitiesMenu_InjectPoint = 0x7f3b5d;
static uint32_t DoUtilitiesMenu_ContinueJump = 0x7f3b64;

static uint32_t DoZoneCatalogView_InjectPoint = 0x7ec6e5;
static uint32_t DoZoneCatalogView_ContinueJump = 0x7ec6ea;

static uint32_t DoTransportMenuExtra_InjectPoint = 0x7f3d04;
static uint32_t DoTransportMenuExtra_ContinueJump_NoMatch = 0x7f3d09;
static uint32_t DoTransportMenuExtra_ContinueJump_Match = 0x7f3d50;

static uint32_t DoTransportMenuTarget_InjectPoint = 0x7f3d86;
static uint32_t DoTransportMenuTarget_ContinueJump = 0x7f3d97;

static uint32_t DoTransportMenuTarget2_InjectPoint = 0x7f3d68;
static uint32_t DoTransportMenuTarget2_ContinueJump = 0x7f3d70;

static uint32_t HandleButtonActivatedReopen_InjectPoint = 0x7f46a3;
static uint32_t HandleButtonActivatedReopen_ContinueJump = 0x7f46aa;

static constexpr uint32_t AddBuildingsToItemList_InjectPoint = 0x7f0073;
static constexpr uint32_t AddBuildingsToItemList_ContinueJump_Skip = 0x7f065b;
static constexpr uint32_t AddBuildingsToItemList_ContinueJump_Keep = 0x7f0194;
static constexpr uint32_t AddBuildingsToItemList_ContinueJump_UseOrigProp = 0x7f0078;

static uint32_t CreateBuildingMenu_InjectPoint = 0x7f074e;
static uint32_t CreateBuildingMenu_ContinueJump = 0x7f0756;

static uint32_t CreateCatalogView_InjectPoint = 0x78f16a;
static uint32_t CreateCatalogView_ContinueJump = 0x78f16f;

static uint32_t CreateCatalogItemList_InjectPoint = 0x78f030;
static uint32_t CreateCatalogItemList_ContinueJump_AddItem = 0x78f053;
static uint32_t CreateCatalogItemList_ContinueJump_Default = 0x78f036;
static uint32_t CreateCatalogItemList_ContinueJump_Skip = 0x78f038;
static uint32_t CreateCatalogItemList2_InjectPoint = 0x78efac;
static uint32_t CreateCatalogItemList2_ContinueJump = 0x78efb2;
static uint32_t CreateCatalogItemList3_InjectPoint = 0x78eff1;
static uint32_t CreateCatalogItemList3_ContinueJump = 0x78eff6;

static uint32_t InvokeTertiaryMenu_InjectPoint = 0x7eb009;
static uint32_t InvokeTertiaryMenu_ContinueJump = 0x7eb00e;


static uint32_t pLastPhysicalButtonNotificationTarget;  // notification target of last button used for opening tertiary menu
static uint32_t lastPhysicalButtonId = 0;  // ID of last button used for opening tertiary menu
static uint32_t lastVirtualButtonId;  // virtual or physical button

uint32_t occupantGroup = 0;
uint32_t submenuPropValueBuffer = 0;

// used as an additional argument for CreateBuildingMenu and CreateCatalogView
uint32_t buttonIdForCreateBuildingMenu = 0;

// we keep track of the catalog item IIDs in order to avoid adding icons more than once in case the Plugins contain duplicates
std::unordered_set<uint32_t> itemListIIDs = {};

bool shouldDiscardItem = 0;


static Categorization* spCategorization = nullptr;

namespace
{
	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}

	void OverwriteMemory(void* address, uint8_t newValue)
	{
		DWORD oldProtect;
		// Allow the executable memory to be written to.
		THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(
			address,
			sizeof(newValue),
			PAGE_EXECUTE_READWRITE,
			&oldProtect));

		// Patch the memory at the specified address.
		*((uint8_t*)address) = newValue;
	}

	void InstallHook(uint32_t address, void (*pfnFunc)(void))
	{
		DWORD oldProtect;
		THROW_IF_WIN32_BOOL_FALSE(VirtualProtect((void *)address, 5, PAGE_EXECUTE_READWRITE, &oldProtect));

		*((uint8_t*)address) = 0xE9;
		*((uint32_t*)(address+1)) = ((uint32_t)pfnFunc) - address - 5;
	}

	std::unordered_map<uint32_t, std::stack<uint32_t>*> menuLayerStacks = {};

	// store the last physical button press with associated occupantGroup on menu layer stack, when pressing submenu button
	void storeMenuLayer(const uint32_t physicalButtonId, const uint32_t occupantGroup)
	{
		if (menuLayerStacks.contains(physicalButtonId)) {
			auto stack = menuLayerStacks[physicalButtonId];
			if (stack->empty() || stack->top() != occupantGroup) {
				stack->push(occupantGroup);
			} else {
				// occupantGroup is already top of stack, so do nothing
			}
		} else {
			auto stack = new std::stack<uint32_t>();
			stack->push(occupantGroup);
			menuLayerStacks[physicalButtonId] = stack;
		}
	}

	/* void printStack(std::stack<uint32_t> stack)  // copies stack */
	/* { */
	/* 	std::cout << "stack("; */
	/* 	while (!stack.empty()) { */
	/* 		std::cout << std::hex << stack.top() << ", "; */
	/* 		stack.pop(); */
	/* 	} */
	/* 	std::cout << ")\n"; */
	/* } */

	// Returns which menu layer ID to open, or 0 if menu should be closed, when pressing top-level menu button.
	// Here, physicalButtonId corresponds to the current top-level menu click and
	// openMenuId may be a physical button ID or an occupant group of a submenu.
	uint32_t getMenuLayer(const uint32_t openMenuId, const uint32_t physicalButtonId)
	{
		uint32_t result;
		if (openMenuId == physicalButtonId) {
			result = 0;  // corresponding top-level menu is open, so close the menu
		} else if (openMenuId == 0) {
			// menu is closed, so re-open last layer
			if (menuLayerStacks.contains(physicalButtonId)) {
				auto stack = menuLayerStacks[physicalButtonId];
				/* printStack(*stack); */
				result = stack->empty() ? physicalButtonId : stack->top();
			} else {
				result = physicalButtonId;
			}
		} else {
			// openMenuId is
			// - a submenu in the same menu tree (corresponding to physicalButtonId)
			// - a submenu or physicalButtonId in a different menu tree (not corresponding to physicalButtonId)
			if (menuLayerStacks.contains(physicalButtonId)) {
				auto stack = menuLayerStacks[physicalButtonId];
				/* printStack(*stack); */
				if (stack->empty()) {
					result = physicalButtonId;
				} else {
					// if openMenuId is top of stack, open parent layer (otherwise openMenuId belongs to different top-level menu)
					if (openMenuId == stack->top()) {
						stack->pop();
						result = stack->empty() ? physicalButtonId : stack->top();
					} else {
						// re-open last layer of other top-level menu (TODO consider clearing stack and re-opening top-level menu)
						result = stack->top();
					}
				}
			} else {
				result = physicalButtonId;
			}
		}
		return result;
	}

	std::unordered_set<uint32_t> emptySubmenus = {};
	std::unordered_set<uint32_t> reachableSubmenus(Categorization::toplevelMenuButtons);  // initially just the top-level menus, submenus are added later

	// returns false if item is not a submenu
	bool isEmptySubmenu(const uint32_t buttonId)
	{
		return emptySubmenus.contains(buttonId);
	}

	// If catalog item is a submenu button, this triggers a static button click event to open a new catalog.
	// Otherwise, this sends the default message for clicking a catalog item.
	void NAKED_FUN Hook_WinCatalogView_OnMouseUpL(void)
	{
		__asm {
			// itemId has already been pushed beforehand
			cmp eax, SUBMENU_ITEM_TYPE;
			je submenuButton;

			// ordinary message
			push eax;  // item type
			push 0x49d993d4;  // catalog item click
			push 0x3;  // message type
			push ecx; // notification target
			push WinCatalogView_OnMouseUpL_ContinueJump;
			ret;

submenuButton:
			// item button ID ITEM_BUTTON_ID_PROP has already been pushed, but only we seem to use it
			push VIRTUAL_SUBMENU_ITEM_TYPE;
			push 0x287259f6;  // static button click
			push 0x3;  // message type
			push dword ptr [pLastPhysicalButtonNotificationTarget];  // notification target
			push WinCatalogView_OnMouseUpL_ContinueJump;
			ret;
		}
	}

	Categorization::TriState call_belongsToMenu(cISCPropertyHolder* propHolder, uint32_t menuId)
	{
		return spCategorization->belongsToMenu(propHolder, menuId);
	}

	// Determine whether the current building should be added to the menu or not, based on its building exemplar data.
	// This takes into account whether menus are non-reachable or empty and also auto-categorizes some buildings without submenu.
	void NAKED_FUN Hook_AddBuildingsToItemList(void)
	{
		__asm {
			// edi contains cISCPropertyHolder*
			push eax;  // store
			push ecx;  // store
			push edx;  // store

			// choose between top-level menuId and submenuId argument
			cmp dword ptr [lastVirtualButtonId], VIRTUAL_SUBMENU_ITEM_TYPE;
			je pushSubmenuId;
			push dword ptr [lastVirtualButtonId];  // top-level menuId
			jmp afterMenuId;
pushSubmenuId:
			push dword ptr [occupantGroup];  // submenuId
afterMenuId:
			push edi;  // cISCPropertyHolder*
			call call_belongsToMenu; // (cdecl)
			add esp, 0x8;
			pop edx;  // restore
			pop ecx;  // restore

			cmp al, Categorization::TriState::No;
			je skip;
			cmp al, Categorization::TriState::Yes;
			je keep;
			// else Categorization::TriState::Maybe, so use original occupant groups evaluation code

			pop eax;  // restore
			push OCCUPANT_GROUPS_PROP;
			push AddBuildingsToItemList_ContinueJump_UseOrigProp;
			ret;

skip:
			pop eax;  // restore
			push AddBuildingsToItemList_ContinueJump_Skip;
			ret;

keep:
			pop eax;  // restore
			push AddBuildingsToItemList_ContinueJump_Keep;
			ret;
		}
	}

	uint32_t menuFramePngFlora = FLORA_MENU_ID;
	uint32_t menuFramePngZone = ZONE_MENU_ID;
	uint32_t menuFramePngTransport = TRANSPORT_MENU_ID;
	uint32_t menuFramePngUtility = UTILITY_MENU_ID;
	uint32_t menuFramePngCivic = CIVIC_MENU_ID;

	void initializeMenuFrames()
	{
		cIGZPersistResourceManagerPtr pResMan;
		if (!pResMan) {
			return;
		}
		auto key = cGZPersistResourceKey(0x856DDBAC, 0x46A006B0, 0xAC581B70);
		if (pResMan->TestForKey(key)) {
			menuFramePngFlora = key.instance;
		}
		key.instance = 0xAC581B71;
		if (pResMan->TestForKey(key)) {
			menuFramePngZone = key.instance;
		}
		key.instance = 0xAC581B72;
		if (pResMan->TestForKey(key)) {
			menuFramePngTransport = key.instance;
		}
		key.instance = 0xAC581B73;
		if (pResMan->TestForKey(key)) {
			menuFramePngUtility = key.instance;
		}
		key.instance = 0xAC581B74;
		if (pResMan->TestForKey(key)) {
			menuFramePngCivic = key.instance;
		}
	}

	// Alters the call of InvokeTertiaryMenu to change which button the displayed catalog menu is attached to.
	void NAKED_FUN Hook_DoTransportMenuTarget(void)
	{
		__asm {
			mov ebp, eax;
			mov eax, dword ptr [esp + 0x3c];
			cmp eax, VIRTUAL_SUBMENU_ITEM_TYPE;
			je nested;

			// regular menu, no overwrite
			push TRANSPORT_MENU_ID;
			push ebp;
			push eax;  // physical button ID
			push ecx;  // notification target
			push eax;  // item type (virtual button ID)
			jmp rest;

nested:
			// for simplicity, we just check for all the buttons that need another color
			mov eax, dword ptr [lastPhysicalButtonId];
			cmp eax, ROADWAY_BUTTON_ID;
			je transportColor;
			cmp eax, HIGHWAY_BUTTON_ID;
			je transportColor;
			cmp eax, RAIL_BUTTON_ID;
			je transportColor;
			cmp eax, MISCTRANSP_BUTTON_ID;
			je transportColor;
			cmp eax, AIRPORT_BUTTON_ID;
			je transportColor;
			cmp eax, SEAPORT_BUTTON_ID;
			je transportColor;
			cmp eax, POWER_BUTTON_ID;
			je utilityColor;
			cmp eax, WATER_BUTTON_ID;
			je utilityColor;
			cmp eax, GARBAGE_BUTTON_ID;
			je utilityColor;
			cmp eax, FLORA_BUTTON_ID;
			je floraColor;
			cmp eax, ZONE_R_BUTTON_ID;
			je zoneColor;
			cmp eax, ZONE_C_BUTTON_ID;
			je zoneColor;
			cmp eax, ZONE_I_BUTTON_ID;
			je zoneColor;
			// else default to civic menu color
			push dword ptr [menuFramePngCivic];
			jmp afterColor;
floraColor:
			push dword ptr [menuFramePngFlora];
			jmp afterColor;
zoneColor:
			push dword ptr [menuFramePngZone];
			jmp afterColor;
transportColor:
			push dword ptr [menuFramePngTransport];
			jmp afterColor;
utilityColor:
			push dword ptr [menuFramePngUtility];

afterColor:
			mov eax, dword ptr [esp + 0x40];  // restore
			push ebp;
			push dword ptr [lastPhysicalButtonId];
			push ecx;  // notification target
			// Overwriting this item type by the OG (submenu button ID) is
			// necessary for making nested submenus work, since otherwise two
			// clicks of VIRTUAL_SUBMENU_ITEM_TYPE would close the menu.
			push dword ptr [occupantGroup];  // is only initialized in nested case

rest:
			mov ecx, esi;
			push DoTransportMenuTarget_ContinueJump;
			ret;
		}
	}

	// When pressing a top-level/physical menu button, determine if we should re-open a submenu.
	// If so, overwrite the current parameters accordingly, so that that submenu is opened.
	void NAKED_FUN Hook_HandleButtonActivatedReopen(void)
	{
		__asm {
			mov edi, dword ptr [esp + 0x38];  // param_2
			mov esi, dword ptr [edi + 0x8];  // item type from dwData2 (physical or virtual button ID)

			cmp esi, VIRTUAL_SUBMENU_ITEM_TYPE;
			je skip;  // we've clicked a submenu button, so do not alter anything

			// else we've pressed a physical button, which should act as
			// - back, if a submenu is currently open
			// - close, if the top-level menu of the button is open
			// - re-open, if none or a different menu are open
			push eax;  // store
			push ecx;  // store
			push edx;  // store
			push esi;  // physicalButtonId
			push dword ptr [ebp + 0x204];  // openMenuId
			call getMenuLayer;  // (cdecl)
			add esp, 0x8;
			cmp eax, 0;  // menu should be closed; this only happens in case the top-level menu is open, so can be ignored
			je skip2;
			cmp eax, esi;  // check if result is equal to original physicalButtonId (otherwise it is a submenu ID/occupantGroup)
			je skip2;

			// we overwrite a physical button press and all its state in order to re-open a submenu
			mov dword ptr [lastPhysicalButtonId], esi;  // physicalButtonId
			mov dword ptr [edi + 0xc], eax;  // overwrite occupantGroup in dwData3
			mov dword ptr [occupantGroup], eax;  // occupantGroup (submenu ID) (might be redundant)
			mov dword ptr [esp + 0x38 + 0xc], VIRTUAL_SUBMENU_ITEM_TYPE;  // overwrite param_2/itemType
			mov dword ptr [edi + 0x8], VIRTUAL_SUBMENU_ITEM_TYPE;  // overwrite dwData2
			mov esi, VIRTUAL_SUBMENU_ITEM_TYPE;  // overwrite itemType/dwData2
			mov dword ptr [lastVirtualButtonId], VIRTUAL_SUBMENU_ITEM_TYPE;  // (might be redundant)

skip2:
			pop edx;  // restore
			pop ecx;  // restore
			pop eax;  // restore

skip:
			push HandleButtonActivatedReopen_ContinueJump;
			ret;
		}
	}

	// stores buttonId for use in CreateBuildingMenu for transport menu
	void NAKED_FUN Hook_DoTransportMenuTarget2(void)
	{
		__asm {
			// edx contains button id/item type
			mov dword ptr [lastVirtualButtonId], edx;
			mov dword ptr [buttonIdForCreateBuildingMenu], edx;
			push -1;  // zone
			push ecx;  // network menu group
			lea ecx, [esp + 0x2c];
			push ecx;
			push DoTransportMenuTarget2_ContinueJump;
			ret;
		}
	}

	// stores buttonId for use in CreateBuildingMenu for civic menu
	void NAKED_FUN Hook_HandleButtonActivated2(void)
	{
		__asm {
			// esi contains button id
			mov eax, esi;
			mov dword ptr [lastVirtualButtonId], eax;
			mov dword ptr [buttonIdForCreateBuildingMenu], eax;
			push -1;  // zone
			push 0;  // network menu group
			lea eax, [esp + 0x2c];
			push eax;
			push HandleButtonActivated2_ContinueJump;
			ret;
		}
	}

	// stores buttonId for use in CreateBuildingMenu for utilities menu
	void NAKED_FUN Hook_DoUtilitiesMenu(void)
	{
		__asm {
			// edx contains button id
			mov dword ptr [lastVirtualButtonId], edx;
			mov dword ptr [buttonIdForCreateBuildingMenu], edx;
			push ebp;  // zone
			push edi;  // network menu group
			lea ecx, [esp + 0x24];
			push ecx;
			push DoUtilitiesMenu_ContinueJump;
			ret;
		}
	}

	// stores buttonId for use in CreateCatalogView for zoning menu
	void NAKED_FUN Hook_DoZoneCatalogView(void)
	{
		__asm {
			mov ebx, dword ptr [esp + 0xc];  // param_2 (buttonId)
			mov dword ptr [lastVirtualButtonId], ebx;
			mov dword ptr [buttonIdForCreateBuildingMenu], ebx;  // also used for CreateCatalogView
			mov ebx, dword ptr [esp + 0x10];  // param_3
			push esi;
			push DoZoneCatalogView_ContinueJump;
			ret;
		}
	}

	// adds additional calls to CreateCatalogItemList in order to add submenu buttons and submenu items for flora and networks
	void NAKED_FUN Hook_CreateBuildingMenu(void)
	{
		__asm {
			mov eax, dword ptr [buttonIdForCreateBuildingMenu];
			cmp eax, 0;
			je skipSubmenu;

			// third call to add submenu buttons
			mov dword ptr [buttonIdForCreateBuildingMenu], 0;  // mark argument as used until next invocation
			push ITEM_BUTTON_CLASS_SUBMENU;
			cmp eax, VIRTUAL_SUBMENU_ITEM_TYPE;
			je nested;
			push eax;  // virtual button id, matches against ITEM_SUBMENU_PARENT_ID_PROP
			jmp commonCall;
nested:
			push dword ptr [occupantGroup];  // is only initialized in nested case
commonCall:
			push SUBMENU_ITEM_TYPE;
			push ITEM_EXEMPLAR_GID_MISC_CATALOG;
			lea ecx, [esp + 0x1c];
			push ebp;
			push ecx;
			mov eax, dword ptr [nSC4UI_CreateCatalogItemList];
			call eax;  // (cdecl)
			add esp, 0x18;

skipSubmenu:
			cmp dword ptr [lastVirtualButtonId], VIRTUAL_SUBMENU_ITEM_TYPE;
			jne skipFlora;  // only add flora items in nested case
			cmp dword ptr [lastPhysicalButtonId], FLORA_BUTTON_ID;  // lastPhysicalButtonId is only up-to-date in nested case
			jne skipFlora;

			// fourth call for flora items
			push ITEM_BUTTON_ID_PROP;
			push dword ptr [occupantGroup]; // is initialized as we are in nested transport menu opened from flora menu
			push TOOL_PLOP_FLORA;
			push ITEM_EXEMPLAR_GID_FLORA;
			lea ecx, [esp + 0x1c];
			push ebp;
			push ecx;
			mov eax, dword ptr [nSC4UI_CreateCatalogItemList];
			call eax;  // (cdecl)
			add esp, 0x18;

skipFlora:
			cmp dword ptr [lastVirtualButtonId], VIRTUAL_SUBMENU_ITEM_TYPE;
			jne skipNetworkItems;  // only add additional network items in nested case
			// for simplicity, we compare against relevant transport menu button IDs
			mov eax, dword ptr [lastPhysicalButtonId]; // is initialized as we are in nested submenu
			cmp eax, ROADWAY_BUTTON_ID;
			je addNetworkItems;
			cmp eax, HIGHWAY_BUTTON_ID;
			je addNetworkItems;
			cmp eax, RAIL_BUTTON_ID;
			je addNetworkItems;
			cmp eax, MISCTRANSP_BUTTON_ID;
			je addNetworkItems;
			cmp eax, AIRPORT_BUTTON_ID;
			je addNetworkItems;
			cmp eax, SEAPORT_BUTTON_ID;
			je addNetworkItems;
			jmp skipNetworkItems;

addNetworkItems:
			// fifth call for network items
			push ITEM_BUTTON_CLASS_NETWORK;
			push dword ptr [occupantGroup]; // is initialized as we are in nested submenu
			push TOOL_PLOP_NETWORK;
			push ITEM_EXEMPLAR_GID_MISC_CATALOG;
			lea ecx, [esp + 0x1c];
			push ebp;
			push ecx;
			mov eax,dword ptr [nSC4UI_CreateCatalogItemList];
			call eax;  // (cdecl)
			add esp, 0x18;

skipNetworkItems:
			mov eax, dword ptr [esp + 0x24];
			mov ecx, dword ptr [esp + 0x20];
			push CreateBuildingMenu_ContinueJump;
			ret;
		}
	}

	// adds an additional call to CreateCatalogItemList in order to add submenu buttons
	void NAKED_FUN Hook_CreateCatalogView(void)
	{
		__asm {
			mov eax, dword ptr [nSC4UI_CreateCatalogItemList];
			call eax;  // (cdecl)

			// check if we come from flora menu
			mov eax, dword ptr [esp + 0x3c];
			cmp eax, TOOL_PLOP_FLORA;
			je flora;

			// check if we come from zoning menu
			mov eax, dword ptr [buttonIdForCreateBuildingMenu];
			cmp eax, 0;
			je skipSubmenu;

			// second call to add submenu buttons for zoning menu and flora menu
			mov dword ptr [buttonIdForCreateBuildingMenu], 0;  // mark argument as used until next invocation
			jmp addSubmenu;
flora:
			mov eax, FLORA_BUTTON_ID;
addSubmenu:
			add esp, 0x18;
			push ITEM_BUTTON_CLASS_SUBMENU;
			push eax;  // virtual button id, matches against ITEM_SUBMENU_PARENT_ID_PROP
			push SUBMENU_ITEM_TYPE;
			push ITEM_EXEMPLAR_GID_MISC_CATALOG;
			lea edx, [esp + 0x20];
			push ebp;
			push edx;
			mov eax, dword ptr [nSC4UI_CreateCatalogItemList];
			call eax;  // (cdecl)

skipSubmenu:
			push CreateCatalogView_ContinueJump;
			ret;
		}
	}

	void clearItemListIIDs(void)
	{
		itemListIIDs.clear();
	}

	// returns true if duplicate item should be discarded
	bool addToItemList(const uint32_t iid)
	{
		if (itemListIIDs.contains(iid)) {
			if (iid == NAM_CONTROLLER_MARKER_BUTTON_ID) {
				return false;  // for ease of debugging, keep duplicates of NAM Controller Marker button
			} else {
				return true;
			}
		} else {
			itemListIIDs.insert(iid);
			return false;
		}
	}

	// clears the IID cache prior to filling a new catalog item list
	void NAKED_FUN Hook_CreateCatalogItemList2(void)
	{
		__asm {
			push eax;  // store
			call clearItemListIIDs;  // (cdecl)
			pop eax;  // restore
			mov ecx, dword ptr [esp + 0x18];
			mov edx, dword ptr [ecx];
			push CreateCatalogItemList2_ContinueJump;
			ret;
		}
	}

	// stores whether the current item exemplar should be discarded due to duplicate IID or when it is an empty submenu
	void NAKED_FUN Hook_CreateCatalogItemList3(void)
	{
		__asm {
			// eax contains resource key (TGI)
			mov ecx, dword ptr [eax + 0x8];  // instance ID

			push eax;  // store
			push edx;  // store
			push ecx;  // argument for isEmptySubmenu
			push ecx;
			call addToItemList;  // (cdecl)
			add esp, 0x4;
			test al, al;
			jnz skipEmptinessCheck;
			call isEmptySubmenu;  // (cdecl) note that here we assume that IID and Button ID are the same
skipEmptinessCheck:
			mov byte ptr [shouldDiscardItem], al;  // for later use
			add esp, 0x4;
			pop edx;  // restore
			pop eax;  // restore

			mov ecx, esi;  // (this)
			call dword ptr [edi + 0xc];  // (thiscall)
			push CreateCatalogItemList3_ContinueJump;
			ret;
		}
	}

	// Alters the handling of catalog item exemplars in case of submenu buttons as well as network buttons contained in submenus.
	// Also fixes the duplicate network item icon bug.
	void NAKED_FUN Hook_CreateCatalogItemList(void)
	{
		__asm {
			// first check whether the exemplar is a duplicate or an empty submenu
			cmp byte ptr [shouldDiscardItem], 0;
			jne discardItem;

			// next determine whether to check for submenu or network item property
			cmp dword ptr [esp + 0x5c], SUBMENU_ITEM_TYPE;  // param_4
			je checkSubmenuOrNetworkInSubmenu;
			cmp dword ptr [lastVirtualButtonId], VIRTUAL_SUBMENU_ITEM_TYPE;  // check if nested
			jne checkRegularNetworkItem;  // only add additional network items in nested case
			cmp dword ptr [esp + 0x5c], TOOL_PLOP_NETWORK;  // param_4
			jne checkRegularNetworkItem;
			// else add network items for nested submenus

checkSubmenuOrNetworkInSubmenu:
			// check if has ITEM_BUTTON_CLASS_PROP
			mov edx, dword ptr [edi];
			mov eax, ITEM_BUTTON_CLASS_PROP;
			push eax;
			mov ecx, edi;  // (this)
			call dword ptr [edx + 0xc];  // HasProperty (thiscall)
			test al, al;
			jz discardItem;  // if it does not have the property

			// else has submenu, so get the value of the property
			mov ecx, offset submenuPropValueBuffer;
			push ecx;
			push ITEM_BUTTON_CLASS_PROP;
			push edi;
			mov eax, dword ptr [cSCPropertyHelp_GetPropertyValue];  // result is stored in submenuPropValueBuffer
			call eax;  // (cdecl)
			add esp, 0xc;

			mov eax, dword ptr [esp + 0x64];  // param_6: desired property value
			cmp eax, dword ptr [submenuPropValueBuffer];
			jne checkRegularNetworkItem;

			// else directly add item
			push CreateCatalogItemList_ContinueJump_AddItem;
			ret;

checkRegularNetworkItem:
			// check network item property as usual
			mov eax, dword ptr [esp + 0x64];
			cmp eax, ebp;
			push CreateCatalogItemList_ContinueJump_Default;
			ret;

discardItem:
			// do not add this item at all
			push CreateCatalogItemList_ContinueJump_Skip;
			ret;
		}
	}

	// stores physical button and notification target for later use
	void NAKED_FUN Hook_InvokeTertiaryMenu(void)
	{
		__asm {
			// ebx contains notification target, ecx contains physical button ID
			mov dword ptr [pLastPhysicalButtonNotificationTarget], ebx;  // store notification target for later use
			mov dword ptr [lastPhysicalButtonId], ecx;  // store physical button ID for later use
			mov eax, dword ptr [ebx];
			push ecx;
			mov ecx, ebx;
			push InvokeTertiaryMenu_ContinueJump;
			ret;
		}
	}

	// redirects a range of extra button IDs to DoTransportMenu
	void NAKED_FUN Hook_HandleButtonActivated(void)
	{
		__asm {
			cmp esi, TRANSPORT_EXTRA_MENU_MAX;
			ja noTransportExtraMatch;
			cmp esi, TRANSPORT_EXTRA_MENU_MIN;
			jb noTransportExtraMatch;
			push HandleButtonActivated_ContinueJump_Transport;
			ret;
noTransportExtraMatch:
			cmp esi, 0x6991ee42;
			push HandleButtonActivated_ContinueJump;
			ret;
		}
	}

	struct CatalogState {
		uint32_t scrollPosition;
		uint32_t selectedItemId;
	};

	std::unordered_map<uint32_t, CatalogState*> catalogStates = {};

	CatalogState* getVirtualButtonCatalogState(const uint32_t virtualButtonId)
	{
		if (catalogStates.contains(virtualButtonId))
		{
			return catalogStates[virtualButtonId];
		}
		else
		{
			auto state = new CatalogState();
			catalogStates[virtualButtonId] = state;
			return state;
		}
	}

	// adds a virtual button to the transport menu that is invoked when a submenu button is clicked
	void NAKED_FUN Hook_DoTransportMenuExtra(void)
	{
		__asm {
			cmp eax, TRANSPORT_EXTRA_MENU_MAX;
			ja noTransportExtraMatch;
			cmp eax, TRANSPORT_EXTRA_MENU_MIN;
			jb noTransportExtraMatch;
			// if in range, choose occupant group and fill the menu

			// first get button ID from dwData3 and store it as OG
			mov eax, dword ptr [ebp + 0xc]; // dwData3
			mov dword ptr [occupantGroup], eax;

			mov eax, offset occupantGroup;
			push eax;
			lea eax, [esp + 0x14];
			push eax;
			lea ecx, [esp + 0x20];  // (this)
			mov eax, dword ptr [DoTransportMenu_addOG];
			call eax;  // (thiscall)
			xor ecx, ecx;  // network menu group = 0, so no network tools or puzzle pieces in this menu

			// store new menu layer on stack
			push ecx;  // store
			push edx;  // store
			push dword ptr [occupantGroup];
			push dword ptr [lastPhysicalButtonId];
			call storeMenuLayer;  // (cdecl)
			add esp, 0x8;
			// get cached catalog state or allocate it
			push dword ptr [occupantGroup];
			call getVirtualButtonCatalogState;  // (cdecl)
			add esp, 0x4;
			pop edx;  // restore
			pop ecx;  // restore

			mov edi, eax;  // catalog state
			push DoTransportMenuExtra_ContinueJump_Match;
			ret;
noTransportExtraMatch:  // continue regularly with airport menu branch
			cmp eax, AIRPORT_BUTTON_ID;
			push DoTransportMenuExtra_ContinueJump_NoMatch;
			ret;
		}
	}

	void InstallDuplicateIconsPatch(const uint16_t gameVersion)
	{
		Logger& logger = Logger::GetInstance();

		try
		{
			if (gameVersion != 641)
			{
				return;
			}
			InstallHook(CreateCatalogItemList_InjectPoint, Hook_CreateCatalogItemList);
			InstallHook(CreateCatalogItemList2_InjectPoint, Hook_CreateCatalogItemList2);
			InstallHook(CreateCatalogItemList3_InjectPoint, Hook_CreateCatalogItemList3);

			logger.WriteLine(
				LogLevel::Info,
				"Installed the Duplicate Menu Icons fix.");
		}
		catch (const wil::ResultException& e)
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to install the Duplicate Menu Icons fix.\n%s",
				e.what());
		}
	}

	void InstallSubmenusPatch(const uint16_t gameVersion)
	{
		Logger& logger = Logger::GetInstance();

		try
		{
			if (gameVersion != 641)
			{
				return;
			}
			InstallHook(WinCatalogView_OnMouseUpL_InjectPoint, Hook_WinCatalogView_OnMouseUpL);
			InstallHook(HandleButtonActivated_InjectPoint, Hook_HandleButtonActivated);
			InstallHook(DoTransportMenuTarget_InjectPoint, Hook_DoTransportMenuTarget);
			InstallHook(HandleButtonActivatedReopen_InjectPoint, Hook_HandleButtonActivatedReopen);
			InstallHook(DoTransportMenuTarget2_InjectPoint, Hook_DoTransportMenuTarget2);
			InstallHook(HandleButtonActivated2_InjectPoint, Hook_HandleButtonActivated2);
			InstallHook(DoUtilitiesMenu_InjectPoint, Hook_DoUtilitiesMenu);
			InstallHook(DoZoneCatalogView_InjectPoint, Hook_DoZoneCatalogView);
			InstallHook(DoTransportMenuExtra_InjectPoint, Hook_DoTransportMenuExtra);
			InstallHook(CreateBuildingMenu_InjectPoint, Hook_CreateBuildingMenu);
			InstallHook(CreateCatalogView_InjectPoint, Hook_CreateCatalogView);
			InstallHook(AddBuildingsToItemList_InjectPoint, Hook_AddBuildingsToItemList);
			InstallHook(InvokeTertiaryMenu_InjectPoint, Hook_InvokeTertiaryMenu);

			// With the following replacement, plugins can be designed for use with and without the DLL.
			// The property ITEM_SUBMENU_PARENT_ID_PROP is not loaded without the DLL, but otherwise behaves like ITEM_SUBMENU_PROP.
			OverwriteMemory((void*)0x78f00f, (ITEM_SUBMENU_PARENT_ID_PROP & 0xff));  // replaces ITEM_SUBMENU_PROP by ITEM_SUBMENU_PARENT_ID_PROP

			logger.WriteLine(
				LogLevel::Info,
				"Installed the Submenus patch.");
		}
		catch (const wil::ResultException& e)
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to install the Submenus patch.\n%s",
				e.what());
		}
	}

	void InstallPatches(const uint16_t gameVersion)
	{
		InstallDuplicateIconsPatch(gameVersion);
		InstallSubmenusPatch(gameVersion);
	}
}

class SubmenusDllDirector final : public cRZMessage2COMDirector
{
public:

	SubmenusDllDirector() : categorization(Categorization(&reachableSubmenus))
	{
		std::filesystem::path dllFolderPath = GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error);
		logger.WriteLogFileHeader("Submenus DLL " PLUGIN_VERSION_STR);

		spCategorization = &categorization;
	}

	uint32_t GetDirectorID() const
	{
		return kSubmenusDllDirectorID;
	}

	void clearState(void)
	{
		for (auto&& p : catalogStates)
		{
			delete p.second;
		}
		catalogStates.clear();
		for (auto&& p : menuLayerStacks)
		{
			delete p.second;
		}
		menuLayerStacks.clear();
		clearItemListIIDs();
	}

	std::unordered_map<uint32_t, uint32_t> submenuLinks = {};  // maps child to parent
	std::unordered_map<uint32_t, std::unordered_set<uint32_t>*> submenuChildren = {};  // maps parent to children
	std::unordered_set<uint32_t> nonemptySubmenus = {};
	std::unordered_map<uint32_t, cIGZString*> submenuNames = {};  // for debugging

	// Reads a single building exemplar and updates nonemptySubmenus if building belongs to a submenu.
	void initializeSubmenusFromBuilding(cISC4BuildingDevelopmentSimulator* buildingDevSim, uint32_t buildingType)
	{
		Logger& logger = Logger::GetInstance();
		bool success = true;
		cIGZPersistResourceManagerPtr pResMan;

		// TODO ignore GetBuildingProfile check which has unknown use
		cGZPersistResourceKey key;
		success = buildingDevSim->GetBuildingKeyFromType(buildingType, key);

		cISCResExemplar* exemplar = nullptr;
		// GetResource automatically adds a reference to the exemplar
		pResMan->GetResource(key, GZIID_cISCResExemplar, reinterpret_cast<void**>(&exemplar), 0, nullptr);

		auto propHolder = exemplar->AsISCPropertyHolder();
		PropertyUtil::arrayForeach(propHolder, OCCUPANT_GROUPS_ALT_PROP, &key, [this](uint32_t submenuId) { nonemptySubmenus.insert(submenuId); });

		// Auto-filled submenus can be non-empty, despite not having buildings with the corresponding submenu parent property.
		// TODO Here we could save some cycles by iterating only over the autoPrefilledSubmenus that have not been inserted yet.
		for (auto&& submenuId : Categorization::autoPrefilledSubmenus) {
			if (!nonemptySubmenus.contains(submenuId) && categorization.belongsToSubmenu(propHolder, submenuId)) {
				nonemptySubmenus.insert(submenuId);
			}
		}

		exemplar->Release();
	}

	bool isSubmenuEmptyUpdate(uint32_t buttonId)
	{
		if (nonemptySubmenus.contains(buttonId)) {
			return false;
		} else if (submenuChildren.contains(buttonId)) {
			// If the menu contains only submenu buttons, we need to check recursively whether the nested menus are empty.
			for (auto&& childId : *(submenuChildren[buttonId])) {
				if (!isSubmenuEmptyUpdate(childId)) {
					nonemptySubmenus.insert(buttonId);
					return false;
				}
			}
			return true;
		} else {
			// submenu does not have child menus
			return true;
		}
	}

	bool isSubmenuReachableUpdate(uint32_t buttonId)
	{
		if (reachableSubmenus.contains(buttonId)) {
			return true;
		} else if (submenuLinks.contains(buttonId)) {
			const uint32_t parentId = submenuLinks[buttonId];
			if (isSubmenuReachableUpdate(parentId)) {
				reachableSubmenus.insert(buttonId);
				return true;
			} else {
				return false;
			}
		} else {
			return false;  // submenu without parent is not possible, so this buttonId is not a submenu
		}
	}

	bool submenusInitialized = false;
	void initializeSubmenus()
	{
		if (submenusInitialized) {  // TODO reinitialize for each city that is opened?
			return;
		}
		cIGZPersistResourceManagerPtr pResMan;
		cISC4AppPtr pApp;
		if (!pResMan || !pApp)
		{
			return;
		}
		bool success = true;
		Logger& logger = Logger::GetInstance();

		// Parse all the item exemplars for submenu buttons, network items and flora items
		// in order to determine which submenus are empty or are unreachable due to missing button exemplars.
		for (const uint32_t itemGid : {(uint32_t)ITEM_EXEMPLAR_GID_MISC_CATALOG, (uint32_t)ITEM_EXEMPLAR_GID_FLORA})
		{
			cIGZPersistResourceKeyList* pResourceList = nullptr;
			PersistResourceKeyFilterByTypeAndGroup* filter = new PersistResourceKeyFilterByTypeAndGroup(EXEMPLAR_TID, itemGid);
			filter->AddRef();
			pResMan->GetAvailableResourceList(&pResourceList, filter);
			filter->Release();

			pResourceList->AddRef();
			if (pResourceList->Size() > 0)
			{
				pResourceList->EnumKeys([](const auto& key, void* pContext) {
					auto this1 = static_cast<SubmenusDllDirector*>(pContext);
					uint32_t itemGid = key.group;
					cIGZPersistResourceManagerPtr pResMan;
					cISCResExemplar* exemplar = nullptr;
					// GetResource automatically adds a reference to the exemplar
					pResMan->GetResource(key, GZIID_cISCResExemplar, reinterpret_cast<void**>(&exemplar), 0, nullptr);
					auto propHolder = exemplar->AsISCPropertyHolder();

					// check if item exemplar is a submenu button, network item or flora item
					uint32_t propValueBuffer;
					propHolder->GetProperty((uint32_t)ITEM_BUTTON_CLASS_PROP, propValueBuffer);
					if (propValueBuffer == ITEM_BUTTON_CLASS_SUBMENU && itemGid == ITEM_EXEMPLAR_GID_MISC_CATALOG)
					{
						propHolder->GetProperty((uint32_t)ITEM_BUTTON_ID_PROP, propValueBuffer);
						uint32_t buttonId = propValueBuffer;  // copy
						propHolder->GetProperty((uint32_t)ITEM_SUBMENU_PARENT_ID_PROP, propValueBuffer);
						uint32_t parentId = propValueBuffer;  // copy
						if (buttonId != 0 && parentId != 0)
						{
							this1->submenuLinks[buttonId] = parentId;  // found valid submenu button, store it for later use
							if (this1->submenuChildren.contains(parentId)) {
								this1->submenuChildren[parentId]->insert(buttonId);
							} else {
								this1->submenuChildren[parentId] = new std::unordered_set<uint32_t>({buttonId});
							}
							if (!this1->submenuNames.contains(buttonId) && propHolder->HasProperty(exemplarNamePropId)) {
								auto nameProperty = propHolder->GetProperty(exemplarNamePropId);
								const auto variant = nameProperty->GetPropertyValue();
								variant->AddRef();
								cIGZString* exemplarName = nullptr;
								if (variant->CreateValString(&exemplarName)) {
									this1->submenuNames[buttonId] = exemplarName;  // transfer of ownership
								}
								variant->Release();
							}
						}
					}
					else if (propValueBuffer == ITEM_BUTTON_CLASS_NETWORK && itemGid == ITEM_EXEMPLAR_GID_MISC_CATALOG
							|| propValueBuffer == ITEM_BUTTON_CLASS_FLORA && itemGid == ITEM_EXEMPLAR_GID_FLORA)
					{
						propHolder->GetProperty((uint32_t)ITEM_SUBMENU_PARENT_ID_PROP, propValueBuffer);
						if (propValueBuffer != 0)
						{
							uint32_t parentId = propValueBuffer;  // copy
							this1->nonemptySubmenus.insert(parentId);  // found network or flora item
						}
					}

					exemplar->Release();
				}, this);
			}
			pResourceList->Release();
		}

		// Now that we know all submenu IDs, we recursively update reachableSubmenus to determine which buildings must appear in their default menu instead
		uint32_t numNonReachable = 0;
		for (auto&& p : submenuLinks)
		{
			auto buttonId = p.first;
			bool isReachable = isSubmenuReachableUpdate(buttonId);  // updates reachableSubmenus
			if (!isReachable) {
				numNonReachable++;
				logger.WriteLineFormatted(LogLevel::Info, "Submenu with Button ID 0x%08X does not exist or is not reachable from top-level menus.", buttonId);
			}
		}

		// Now that we know which submenus are installed and reachable,
		// we parse all building exemplars to check which submenus are empty in order to hide them.
		// Auto-filled submenus can be non-empty, despite not having buildings with the corresponding submenu parent property.
		cISC4City* pCity = pApp->GetCity();
		if (pCity)
		{
			pCity->AddRef();
			cISC4BuildingDevelopmentSimulator* buildingDevSim = pCity->GetBuildingDevelopmentSimulator();
			if (buildingDevSim)
			{
				buildingDevSim->AddRef();

				uint32_t nCount;
				success = buildingDevSim->GetAllBuildingTypes(nullptr, nCount);

				if (success && nCount > 0)
				{
					uint32_t* buildingTypes{ new uint32_t[nCount]{} };  // TODO consider using eastl::vector
					uint32_t nCount2;
					success = buildingDevSim->GetAllBuildingTypes(buildingTypes, nCount2);
					if (nCount2 != nCount || !success)  // should not happen
					{
						logger.WriteLineFormatted(LogLevel::Error, "Length mismatch of building-type arrays: %d vs %d.", nCount, nCount2);
					}
					else
					{
						for (auto i = 0; i < nCount; i++)
						{
							initializeSubmenusFromBuilding(buildingDevSim, buildingTypes[i]);
						}

					}

					delete[] buildingTypes;
					buildingTypes = nullptr;
				}

				buildingDevSim->Release();
			}
			pCity->Release();
		}

		// We recursively update nonemptySubmenus to cover the case in which a menu contains only empty submenus.
		for (auto&& p : submenuLinks)
		{
			auto buttonId = p.first;
			bool isEmpty = isSubmenuEmptyUpdate(buttonId);  // updates nonemptySubmenus if non-empty
			if (isEmpty) {
				emptySubmenus.insert(buttonId);
				if (submenuNames.contains(buttonId)) {
					logger.WriteLineFormatted(LogLevel::Info, "Hiding empty submenu with Button ID 0x%08X: %s", buttonId, submenuNames[buttonId]->ToChar());
				} else {
					logger.WriteLineFormatted(LogLevel::Info, "Hiding empty submenu with Button ID 0x%08X.", buttonId);
				}
			}
		}
		// Now after initialization of submenus, there is no need for the submenu names anymore.
		for (auto&& p : submenuNames)
		{
			p.second->Release();
		}
		submenuNames.clear();

		initializeMenuFrames();

		logger.WriteLineFormatted(LogLevel::Info,
			"Initialized %d submenus (%d empty, %d not reachable).",
			submenuLinks.size(), emptySubmenus.size(), numNonReachable);
		submenusInitialized = true;
	}

	bool DoMessage(cIGZMessage2* pMsg)
	{
		cIGZMessage2Standard* pStandardMessage = static_cast<cIGZMessage2Standard*>(pMsg);
		uint32_t msgType = pStandardMessage->GetType();

		switch (msgType)
		{
		case kSC4MessagePostCityInit:
			initializeSubmenus();
			clearState();
			break;
		case kSC4MessagePostCityShutdown:
			clearState();
			break;
		}

		return true;
	}

	bool PreAppInit()
	{
		if (exemplarPatcher) {
			return false;
		} else {
			cIGZCOM* const pCOM = GZCOM();
			cRZAutoRefCount<cIExemplarLoadHookServer> exemplarLoadHookServer;
			Logger& logger = Logger::GetInstance();

			// Get an instance of the cIExemplarLoadHookServer class from GZCOM.
			if (pCOM->GetClassObject(
				GZCLSID_cIExemplarLoadHookServer,
				GZIID_cIExemplarLoadHookServer,
				exemplarLoadHookServer.AsPPVoid()))
			{
				exemplarPatcher = new ExemplarPatcher();
				exemplarLoadHookServer->AddLoadNotification(exemplarPatcher);
				logger.WriteLine(LogLevel::Info, "Installed the Exemplar Patching feature.");
			}
			else
			{
				logger.WriteLine(LogLevel::Error, "Failed to reach the Exemplar-load-hook server. Please install the SC4ResourceLoadingHooks DLL:\n\n  https://github.com/0xC0000054/sc4-resource-loading-hooks/releases\n");
			}

			return true;
		}
	}

	bool PostAppInit()
	{
		Logger& logger = Logger::GetInstance();

		cIGZMessageServer2Ptr pMsgServ;
		if (pMsgServ)
		{
			std::vector<uint32_t> requiredNotifications;
			requiredNotifications.push_back(kSC4MessagePostCityInit);
			requiredNotifications.push_back(kSC4MessagePostCityShutdown);

			for (uint32_t messageID : requiredNotifications)
			{
				if (!pMsgServ->AddNotification(this, messageID))
				{
					logger.WriteLine(LogLevel::Error, "Failed to subscribe to the required notifications.");
					return false;
				}
			}
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "Failed to subscribe to the required notifications.");
			return false;
		}

		if (exemplarPatcher)
		{
			exemplarPatcher->LoadPatches();
		}

		return true;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		const uint16_t gameVersion = versionDetection.GetGameVersion();

		if (gameVersion == 641)
		{
			InstallPatches(gameVersion);
		}
		else
		{
			Logger& logger = Logger::GetInstance();
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Requires game version 641, found game version %d.",
				gameVersion);
		}

		cIGZFrameWork* const pFramework = RZGetFrameWork();

		if (pFramework->GetState() < cIGZFrameWork::kStatePreAppInit)
		{
			pFramework->AddHook(this);
		}
		else
		{
			PreAppInit();
		}

		return true;
	}

private:

	const SC4VersionDetection versionDetection;

	ExemplarPatcher* exemplarPatcher = nullptr;

	Categorization categorization;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static SubmenusDllDirector sDirector;
	return &sDirector;
}