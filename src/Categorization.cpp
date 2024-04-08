#include "Categorization.h"
#include "MenuIds.h"
#include "PropertyUtil.h"
#include "cGZPersistResourceKey.h"


const std::unordered_set<uint32_t> Categorization::toplevelMenuButtons = {
	FLORA_BUTTON_ID,
	ZONE_R_BUTTON_ID, ZONE_C_BUTTON_ID, ZONE_I_BUTTON_ID,
	ROADWAY_BUTTON_ID, HIGHWAY_BUTTON_ID, RAIL_BUTTON_ID, MISCTRANSP_BUTTON_ID, AIRPORT_BUTTON_ID, SEAPORT_BUTTON_ID,
	POWER_BUTTON_ID, WATER_BUTTON_ID, GARBAGE_BUTTON_ID,
	POLICE_BUTTON_ID, FIRE_BUTTON_ID, EDUCATION_BUTTON_ID, HEALTH_BUTTON_ID, LANDMARK_BUTTON_ID, REWARD_BUTTON_ID, PARK_BUTTON_ID
};

const std::unordered_set<uint32_t> Categorization::autoPrefilledSubmenus = {
	cs1SubmenuId, cs2SubmenuId, cs3SubmenuId,
	co2SubmenuId, co3SubmenuId,
	iaSubmenuId, idSubmenuId, imSubmenuId, ihtSubmenuId,
	policeSmallSubmenuId, policeMediumSubmenuId, policeLargeSubmenuId,
	elementarySchoolSubmenuId, highSchoolSubmenuId, collegeSubmenuId, libraryMuseumSubmenuId,
	healthSmallSubmenuId, healthMediumSubmenuId, healthLargeSubmenuId,
};

Categorization::Categorization(std::unordered_set<uint32_t>* reachableSubmenus) : reachableSubmenus(reachableSubmenus)
{
}

static inline Categorization::TriState bool2tri(bool b)
{
	return b ? Categorization::TriState::Yes : Categorization::TriState::No;
}

// Determines whether building exemplar belongs to a given submenu, taking into account that some submenus might not be reachable.
bool Categorization::belongsToSubmenu(cISCPropertyHolder* propHolder, uint32_t submenuId)
{
	if (!reachableSubmenus->contains(submenuId))
	{
		return false;
	}
	else if (propHolder->HasProperty(itemSubmenuParentPropId) &&
			PropertyUtil::arrayExists(propHolder, itemSubmenuParentPropId, nullptr, [this](uint32_t id) {
				return this->reachableSubmenus->contains(id);
			}))
	{
		return PropertyUtil::arrayContains(propHolder, itemSubmenuParentPropId, nullptr, submenuId);
	}
	else  // no reachable submenu configured, so try to automatically put building into a submenu using OG or other available data
	{
		auto hasOG = [&propHolder](uint32_t og) { return PropertyUtil::arrayContains(propHolder, occupantGroupsPropId, nullptr, og); };
		switch (submenuId)
		{
			// TODO Check that higher-wealth OG not present?
			case cs1SubmenuId: return hasOG(landmarkOG) && hasOG(cs1OG) && propHolder->HasProperty(capacitySatisfiedPropId);
			case cs2SubmenuId: return hasOG(landmarkOG) && hasOG(cs2OG) && propHolder->HasProperty(capacitySatisfiedPropId);
			case cs3SubmenuId: return hasOG(landmarkOG) && hasOG(cs3OG) && propHolder->HasProperty(capacitySatisfiedPropId);
			case co2SubmenuId: return hasOG(landmarkOG) && hasOG(co2OG) && propHolder->HasProperty(capacitySatisfiedPropId);
			case co3SubmenuId: return hasOG(landmarkOG) && hasOG(co3OG) && propHolder->HasProperty(capacitySatisfiedPropId);
			case iaSubmenuId:  return hasOG(landmarkOG) && hasOG(iaOG)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case idSubmenuId:  return hasOG(landmarkOG) && hasOG(idOG)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case imSubmenuId:  return hasOG(landmarkOG) && hasOG(imOG)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case ihtSubmenuId: return hasOG(landmarkOG) && hasOG(ihtOG) && propHolder->HasProperty(capacitySatisfiedPropId);

			case policeSmallSubmenuId:  return hasOG(policeOG) && (hasOG(policeSmallOG) || hasOG(policeKioskOG));
			case policeMediumSubmenuId: return hasOG(policeOG) && hasOG(policeBigOG) && !hasOG(policeDeluxeOG);
			case policeLargeSubmenuId:  return hasOG(policeOG) && hasOG(policeDeluxeOG);

			case elementarySchoolSubmenuId: return hasOG(schoolOG) && hasOG(schoolElementaryOG);
			case highSchoolSubmenuId:       return hasOG(schoolOG) && (hasOG(schoolHighOG) || hasOG(schoolPrivateOG));
			case collegeSubmenuId:          return hasOG(collegeOG);
			case libraryMuseumSubmenuId:    return hasOG(libraryOG) || hasOG(museumOG);

			case healthSmallSubmenuId:  return hasOG(healthOG) && hasOG(hospitalOG) && !hasOG(largeHealthOG) && !hasOG(healthOtherOG);
			case healthMediumSubmenuId: return hasOG(healthOG) && hasOG(hospitalOG) && hasOG(largeHealthOG) && !hasOG(healthOtherOG);
			case healthLargeSubmenuId:  return hasOG(healthOG) && hasOG(healthOtherOG)
			                                && PropertyUtil::arrayContains(propHolder, budgetItemDepartmentPropId, nullptr, budgetItemDepartmentProp_HealthCoverage);

			default: // generic submenu
				return false;
		}
	}
}

// Determines whether building exemplar belongs to a given menu, taking into account that some submenus might not be reachable.
// `menuId` is a top-level button ID or a submenu ID
Categorization::TriState Categorization::belongsToMenu(cISCPropertyHolder* propHolder, uint32_t menuId)
{
	if (!toplevelMenuButtons.contains(menuId))
	{
		return bool2tri(belongsToSubmenu(propHolder, menuId));
	}
	else  // TODO consider also checking itemSubmenuParentPropId for top-level menus
	{
		auto hasOG = [&propHolder](uint32_t og) { return PropertyUtil::arrayContains(propHolder, occupantGroupsPropId, nullptr, og); };
		switch (menuId)
		{
			case landmarkButtonId:
				return bool2tri(hasOG(landmarkOG)
						&& !belongsToSubmenu(propHolder, cs1SubmenuId)
						&& !belongsToSubmenu(propHolder, cs2SubmenuId)
						&& !belongsToSubmenu(propHolder, cs3SubmenuId)
						&& !belongsToSubmenu(propHolder, co2SubmenuId)
						&& !belongsToSubmenu(propHolder, co3SubmenuId)
						&& !belongsToSubmenu(propHolder, iaSubmenuId)
						&& !belongsToSubmenu(propHolder, idSubmenuId)
						&& !belongsToSubmenu(propHolder, imSubmenuId)
						&& !belongsToSubmenu(propHolder, ihtSubmenuId)
					);

			case policeButtonId:
				return bool2tri((hasOG(policeOG) || hasOG(jailOG))
						&& !belongsToSubmenu(propHolder, policeSmallSubmenuId)
						&& !belongsToSubmenu(propHolder, policeMediumSubmenuId)
						&& !belongsToSubmenu(propHolder, policeLargeSubmenuId)
					);

			case educationButtonId:
				// this implementation looks redundant, but ensures that menu placement is correct even if some submenus are not installed
				return bool2tri((hasOG(schoolOG) || hasOG(collegeOG) || hasOG(libraryOG) || hasOG(museumOG))
						&& !belongsToSubmenu(propHolder, elementarySchoolSubmenuId)
						&& !belongsToSubmenu(propHolder, highSchoolSubmenuId)
						&& !belongsToSubmenu(propHolder, collegeSubmenuId)
						&& !belongsToSubmenu(propHolder, libraryMuseumSubmenuId)
					);

			case healthButtonId:
				return bool2tri(hasOG(healthOG)
						&& !belongsToSubmenu(propHolder, healthSmallSubmenuId)
						&& !belongsToSubmenu(propHolder, healthMediumSubmenuId)
						&& !belongsToSubmenu(propHolder, healthLargeSubmenuId)
					);

			default:
				return Categorization::TriState::Maybe; // other top level menus require evaluating the original inclusion/exclusion rules of their occupant groups
		}
	}
}
