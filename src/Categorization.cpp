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
	freightRailSubmenuId, passengerRailSubmenuId, monorailSubmenuId, hybridRailwaySubmenuId, yardsSubmenuId,
	busSubmenuId, subwaySubmenuId, elRailSubmenuId, glrSubmenuId, multiModalStationsSubmenuId,
	policeSmallSubmenuId, policeLargeSubmenuId, policeDeluxeSubmenuId,
	elementarySchoolSubmenuId, highSchoolSubmenuId, collegeSubmenuId, libraryMuseumSubmenuId,
	healthSmallSubmenuId, healthMediumSubmenuId, healthLargeSubmenuId,
};

Categorization::Categorization(std::unordered_set<uint32_t>* reachableSubmenus) : reachableSubmenus(reachableSubmenus)
{
}

static bool isHeightBelow(cISCPropertyHolder* propHolder, float_t height)
{
	bool result = false;
	if (propHolder->HasProperty((uint32_t)occupantSizePropId)) {
		auto property = propHolder->GetProperty((uint32_t)occupantSizePropId);
		property->AddRef();
		const auto variant = property->GetPropertyValue();
		variant->AddRef();
		if (variant->GetType() == cIGZVariant::Type::Float32Array) {
			uint32_t reps = variant->GetCount();
			if (reps == 3) {
				float_t* values = variant->RefFloat32();
				result |= (values[1] <= height);
			}
		}
		variant->Release();
		property->Release();
	}
	return result;
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
		auto hasOg = [&propHolder](uint32_t og) { return PropertyUtil::arrayContains(propHolder, occupantGroupsPropId, nullptr, og); };
		switch (submenuId)
		{
			// TODO Check that higher-wealth OG not present?
			case cs1SubmenuId: return hasOg(OgLandmark) && hasOg(OgCs1) && propHolder->HasProperty(capacitySatisfiedPropId);
			case cs2SubmenuId: return hasOg(OgLandmark) && hasOg(OgCs2) && propHolder->HasProperty(capacitySatisfiedPropId);
			case cs3SubmenuId: return hasOg(OgLandmark) && hasOg(OgCs3) && propHolder->HasProperty(capacitySatisfiedPropId);
			case co2SubmenuId: return hasOg(OgLandmark) && hasOg(OgCo2) && propHolder->HasProperty(capacitySatisfiedPropId);
			case co3SubmenuId: return hasOg(OgLandmark) && hasOg(OgCo3) && propHolder->HasProperty(capacitySatisfiedPropId);
			case iaSubmenuId:  return hasOg(OgLandmark) && hasOg(OgIa)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case idSubmenuId:  return hasOg(OgLandmark) && hasOg(OgId)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case imSubmenuId:  return hasOg(OgLandmark) && hasOg(OgIm)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case ihtSubmenuId: return hasOg(OgLandmark) && hasOg(OgIht) && propHolder->HasProperty(capacitySatisfiedPropId);

			case freightRailSubmenuId:   return hasOg(OgRail) && hasOg(OgFreightRail)                         && !hasOg(OgAirport) && !hasOg(OgSeaport);
			case passengerRailSubmenuId: return hasOg(OgRail) && hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !hasOg(OgLightrail);
			case monorailSubmenuId:      return hasOg(OgRail) && hasOg(OgMonorail) && !hasOg(OgPassengerRail) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !hasOg(OgLightrail);
			case hybridRailwaySubmenuId: return hasOg(OgRail) && hasOg(OgMonorail) &&  hasOg(OgPassengerRail) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !hasOg(OgLightrail);
			case yardsSubmenuId: return hasOg(OgRail) && !hasOg(OgPassengerRail) && !hasOg(OgFreightRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport);

			case busSubmenuId: return hasOg(OgMiscTransit) && hasOg(OgBus) && !hasOg(OgSubway) && !hasOg(OgLightrail) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport);
			case subwaySubmenuId: return hasOg(OgMiscTransit) && hasOg(OgSubway) && !hasOg(OgLightrail) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !propHolder->HasProperty(capacitySatisfiedPropId);
			case elRailSubmenuId: return hasOg(OgMiscTransit) && hasOg(OgLightrail) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !isHeightBelow(propHolder, 15.5);
			case glrSubmenuId: return hasOg(OgMiscTransit) && hasOg(OgLightrail) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport) && isHeightBelow(propHolder, 15.5);
			case multiModalStationsSubmenuId: return (hasOg(OgRail) || hasOg(OgMiscTransit)) && hasOg(OgLightrail) && (hasOg(OgPassengerRail) || hasOg(OgMonorail)) && !hasOg(OgAirport) && !hasOg(OgSeaport);
			// case parkingSubmenuId:

			case policeSmallSubmenuId:  return hasOg(OgPolice) && (hasOg(OgPoliceSmall) || hasOg(OgPoliceKiosk));
			case policeLargeSubmenuId: return hasOg(OgPolice) && hasOg(OgPoliceBig) && !hasOg(OgPoliceDeluxe);
			case policeDeluxeSubmenuId:  return hasOg(OgPolice) && hasOg(OgPoliceDeluxe);

			case elementarySchoolSubmenuId: return hasOg(OgSchool) && hasOg(OgSchoolElementary);
			case highSchoolSubmenuId:       return hasOg(OgSchool) && (hasOg(OgSchoolHigh) || hasOg(OgSchoolPrivate));
			case collegeSubmenuId:          return hasOg(OgCollege);
			case libraryMuseumSubmenuId:    return hasOg(OgLibrary) || hasOg(OgMuseum);

			case healthSmallSubmenuId:  return hasOg(OgHealth) && hasOg(OgHospital) && !hasOg(OgHealthLarge) && !hasOg(OgHealthOther);
			case healthMediumSubmenuId: return hasOg(OgHealth) && hasOg(OgHospital) && hasOg(OgHealthLarge) && !hasOg(OgHealthOther);
			case healthLargeSubmenuId:  return hasOg(OgHealth) && hasOg(OgHealthOther)
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
	else if (propHolder->HasProperty(itemSubmenuParentPropId) &&
			PropertyUtil::arrayExists(propHolder, itemSubmenuParentPropId, nullptr, [this](uint32_t id) {
				return this->reachableSubmenus->contains(id);
			}))
	{
		return bool2tri(PropertyUtil::arrayContains(propHolder, itemSubmenuParentPropId, nullptr, menuId));
	}
	else
	{
		auto hasOg = [&propHolder](uint32_t og) { return PropertyUtil::arrayContains(propHolder, occupantGroupsPropId, nullptr, og); };
		switch (menuId)
		{
			case landmarkButtonId:
				return bool2tri(hasOg(OgLandmark)
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

			case railButtonId:
				return bool2tri(hasOg(OgRail)
						&& !belongsToSubmenu(propHolder, freightRailSubmenuId)
						&& !belongsToSubmenu(propHolder, passengerRailSubmenuId)
						&& !belongsToSubmenu(propHolder, monorailSubmenuId)
						&& !belongsToSubmenu(propHolder, hybridRailwaySubmenuId)
						&& !belongsToSubmenu(propHolder, multiModalStationsSubmenuId)
						&& !belongsToSubmenu(propHolder, yardsSubmenuId)
					);

			case miscTransitButtonId:
				return bool2tri(hasOg(OgMiscTransit)
						&& !belongsToSubmenu(propHolder, busSubmenuId)
						&& !belongsToSubmenu(propHolder, subwaySubmenuId)
						&& !belongsToSubmenu(propHolder, elRailSubmenuId)
						&& !belongsToSubmenu(propHolder, glrSubmenuId)
						&& !belongsToSubmenu(propHolder, multiModalStationsSubmenuId)
						// && !belongsToSubmenu(propHolder, parkingSubmenuId)
					);

			case policeButtonId:
				return bool2tri((hasOg(OgPolice) || hasOg(OgJail))
						&& !belongsToSubmenu(propHolder, policeSmallSubmenuId)
						&& !belongsToSubmenu(propHolder, policeLargeSubmenuId)
						&& !belongsToSubmenu(propHolder, policeDeluxeSubmenuId)
					);

			case educationButtonId:
				// this implementation looks redundant, but ensures that menu placement is correct even if some submenus are not installed
				return bool2tri((hasOg(OgSchool) || hasOg(OgCollege) || hasOg(OgLibrary) || hasOg(OgMuseum))
						&& !belongsToSubmenu(propHolder, elementarySchoolSubmenuId)
						&& !belongsToSubmenu(propHolder, highSchoolSubmenuId)
						&& !belongsToSubmenu(propHolder, collegeSubmenuId)
						&& !belongsToSubmenu(propHolder, libraryMuseumSubmenuId)
					);

			case healthButtonId:
				return bool2tri(hasOg(OgHealth)
						&& !belongsToSubmenu(propHolder, healthSmallSubmenuId)
						&& !belongsToSubmenu(propHolder, healthMediumSubmenuId)
						&& !belongsToSubmenu(propHolder, healthLargeSubmenuId)
					);

			default:
				return Categorization::TriState::Maybe; // other top level menus require evaluating the original inclusion/exclusion rules of their occupant groups
		}
	}
}
