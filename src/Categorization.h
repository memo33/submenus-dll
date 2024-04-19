#pragma once
#include "cISCPropertyHolder.h"
#include <unordered_set>

class Categorization
{
	public:
		enum TriState : uint8_t { No, Yes, Maybe };

		Categorization(std::unordered_set<uint32_t>* reachableSubmenus);

		bool belongsToSubmenu(cISCPropertyHolder* propHolder, uint32_t submenuId);

		TriState belongsToMenu(cISCPropertyHolder* propHolder, uint32_t menuId);

		static const std::unordered_set<uint32_t> toplevelMenuButtons;

		static const std::unordered_set<uint32_t> autoPrefilledSubmenus;

	private:
		const std::unordered_set<uint32_t>* reachableSubmenus;
};
