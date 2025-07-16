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
	r1SubmenuId, r2SubmenuId, r3SubmenuId,
	cs1SubmenuId, cs2SubmenuId, cs3SubmenuId,
	co2SubmenuId, co3SubmenuId,
	iaSubmenuId, idSubmenuId, imSubmenuId, ihtSubmenuId,
	freightRailSubmenuId, passengerRailSubmenuId, monorailSubmenuId, hybridRailwaySubmenuId, yardsSubmenuId,
	busSubmenuId, subwaySubmenuId, elRailSubmenuId, glrSubmenuId, multiModalStationsSubmenuId,
	portFerrySubmenuId, canalSubmenuId, waterfrontSubmenuId,
	energyDirtySubmenuId, energyCleanSubmenuId, miscPowerSubmenuId,
	policeSmallSubmenuId, policeLargeSubmenuId, policeDeluxeSubmenuId,
	elementarySchoolSubmenuId, highSchoolSubmenuId, collegeSubmenuId, libraryMuseumSubmenuId,
	healthSmallSubmenuId, healthMediumSubmenuId, healthLargeSubmenuId,
	governmentSubmenuId, religionSubmenuId, entertainmentSubmenuId,
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

static bool isPatientCapacityGreaterThan(cISCPropertyHolder* propHolder, uint32_t threshold)
{
	if (propHolder->HasProperty(hospitalPatientCapacityPropId)) {
		auto property = propHolder->GetProperty(hospitalPatientCapacityPropId);
		uint32_t capacity = threshold;
		bool success = property->GetPropertyValue()->GetValUint32(capacity);
		if (success) {
			return capacity > threshold;
		}
	}
	return false;
}

static bool isConditional(cISCPropertyHolder* propHolder)
{
	bool result = false;
	if (propHolder->HasProperty(conditionalBuildingPropId)) {
		auto property = propHolder->GetProperty(conditionalBuildingPropId);
		property->GetPropertyValue()->GetValBool(result);
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
			case cs1SubmenuId: return hasOg(OgLandmark) && hasOg(OgCs1) && propHolder->HasProperty(capacitySatisfiedPropId);
			case cs2SubmenuId: return hasOg(OgLandmark) && hasOg(OgCs2) && propHolder->HasProperty(capacitySatisfiedPropId);
			case cs3SubmenuId: return hasOg(OgLandmark) && hasOg(OgCs3) && propHolder->HasProperty(capacitySatisfiedPropId);
			case co2SubmenuId: return hasOg(OgLandmark) && hasOg(OgCo2) && propHolder->HasProperty(capacitySatisfiedPropId);
			case co3SubmenuId: return hasOg(OgLandmark) && hasOg(OgCo3) && propHolder->HasProperty(capacitySatisfiedPropId);
			case iaSubmenuId:  return hasOg(OgLandmark) && hasOg(OgIa)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case idSubmenuId:  return hasOg(OgLandmark) && hasOg(OgId)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case imSubmenuId:  return hasOg(OgLandmark) && hasOg(OgIm)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case ihtSubmenuId: return hasOg(OgLandmark) && hasOg(OgIht) && propHolder->HasProperty(capacitySatisfiedPropId);
			case r1SubmenuId:  return hasOg(OgLandmark) && hasOg(OgR1)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case r2SubmenuId:  return hasOg(OgLandmark) && hasOg(OgR2)  && propHolder->HasProperty(capacitySatisfiedPropId);
			case r3SubmenuId:  return hasOg(OgLandmark) && hasOg(OgR3)  && propHolder->HasProperty(capacitySatisfiedPropId);

			case freightRailSubmenuId:   return hasOg(OgRail) && hasOg(OgFreightRail)                         && !hasOg(OgAirport) && !hasOg(OgSeaport);
			case passengerRailSubmenuId: return hasOg(OgRail) && hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgBteHybridRailway) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !hasOg(OgLightrail);
			case monorailSubmenuId:      return hasOg(OgRail) && hasOg(OgMonorail) && !hasOg(OgPassengerRail) && !hasOg(OgBteHybridRailway) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !hasOg(OgLightrail);
			case hybridRailwaySubmenuId: return hasOg(OgRail) && (hasOg(OgMonorail) && hasOg(OgPassengerRail) || hasOg(OgBteHybridRailway)) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !hasOg(OgLightrail);
			case yardsSubmenuId: return hasOg(OgRail) && !hasOg(OgPassengerRail) && !hasOg(OgFreightRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport);

			case busSubmenuId: return hasOg(OgMiscTransit) && hasOg(OgBus) && !hasOg(OgSubway) && !hasOg(OgLightrail) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport);
			case subwaySubmenuId: return hasOg(OgMiscTransit) && hasOg(OgSubway) && !hasOg(OgLightrail) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport) && !propHolder->HasProperty(capacitySatisfiedPropId);
			case elRailSubmenuId: return hasOg(OgMiscTransit) && hasOg(OgLightrail) && !isHeightBelow(propHolder, 15.5) && !hasOg(OgBteGlr) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport);
			case glrSubmenuId: return hasOg(OgMiscTransit) && (hasOg(OgLightrail) && isHeightBelow(propHolder, 15.5) || hasOg(OgBteGlr)) && !hasOg(OgPassengerRail) && !hasOg(OgMonorail) && !hasOg(OgAirport) && !hasOg(OgSeaport);
			case multiModalStationsSubmenuId: return (hasOg(OgRail) || hasOg(OgMiscTransit)) && hasOg(OgLightrail) && (hasOg(OgPassengerRail) || hasOg(OgMonorail)) && !hasOg(OgAirport) && !hasOg(OgSeaport);
			// case parkingSubmenuId:  // no auto-categorization

			case portFerrySubmenuId: return hasOg(OgWaterTransit) && (hasOg(OgPassengerFerry) || hasOg(OgCarFerry) || hasOg(OgSeaport)) && !hasOg(OgAirport);
			case canalSubmenuId:  return (hasOg(OgWaterTransit) || hasOg(OgPark)) && (hasOg(OgBteInlandWaterways) || hasOg(OgSgWaterway));
			// case seawallSubmenuId:  // no auto-categorization
			case waterfrontSubmenuId: return hasOg(OgWaterTransit) && hasOg(OgBteWaterfront) && !hasOg(OgBteInlandWaterways) && !hasOg(OgSgWaterway) && !(hasOg(OgPassengerFerry) || hasOg(OgCarFerry) || hasOg(OgSeaport));
			// case watercraftSubmenuId:  // no auto-categorization

			case energyDirtySubmenuId:
				return hasOg(OgPower) && (
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Coal) ||
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::NaturalGas) ||
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Oil) ||
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Waste)
				);
			case energyCleanSubmenuId:
				return hasOg(OgPower) && (
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Hydrogen) ||
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Nuclear) ||
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Solar) ||
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Wind)
				);
			case miscPowerSubmenuId:
				return hasOg(OgPower) && (
					!propHolder->HasProperty(powerPlantTypePropId) ||
					PropertyUtil::arrayContains(propHolder, powerPlantTypePropId, nullptr, PowerPlantType::Auxiliary)
				);

			case policeSmallSubmenuId: return hasOg(OgPolice) && (hasOg(OgPoliceSmall) || hasOg(OgPoliceKiosk)) && !hasOg(OgPoliceBig) && !hasOg(OgPoliceDeluxe);
			case policeLargeSubmenuId: return hasOg(OgPolice) && hasOg(OgPoliceBig) && !hasOg(OgPoliceDeluxe);
			case policeDeluxeSubmenuId:  return hasOg(OgPolice) && hasOg(OgPoliceDeluxe);

			case elementarySchoolSubmenuId: return hasOg(OgSchool) && hasOg(OgSchoolElementary);
			case highSchoolSubmenuId:       return hasOg(OgSchool) && (hasOg(OgSchoolHigh) || hasOg(OgSchoolPrivate));
			case collegeSubmenuId:          return hasOg(OgCollege);
			case libraryMuseumSubmenuId:    return hasOg(OgLibrary) || hasOg(OgMuseum);

			case healthSmallSubmenuId:  return hasOg(OgHealth) && hasOg(OgHospital) && !hasOg(OgHealthOther) &&  (hasOg(OgClinic) || !hasOg(OgHealthLarge) && !hasOg(OgAmbulanceMaker));
			case healthMediumSubmenuId: return hasOg(OgHealth) && hasOg(OgHospital) && !hasOg(OgHealthOther) && !(hasOg(OgClinic) || !hasOg(OgHealthLarge) && !hasOg(OgAmbulanceMaker))
				&& !isPatientCapacityGreaterThan(propHolder, 20000);
			case healthLargeSubmenuId:  return hasOg(OgHealth) && (
					hasOg(OgHealthOther) && PropertyUtil::arrayContains(propHolder, budgetItemDepartmentPropId, nullptr, budgetItemDepartmentProp_HealthCoverage)
					|| isPatientCapacityGreaterThan(propHolder, 20000)
				);

			case governmentSubmenuId:
				return PropertyUtil::arrayContains(propHolder, budgetItemDepartmentPropId, nullptr, budgetItemDepartmentProp_GovernmentBuildings)
					|| hasOg(OgMayorHouse) || hasOg(OgBureaucracy) || hasOg(OgConventionCrowd) || hasOg(OgStockExchange)
					|| (hasOg(OgCourthouse) && !hasOg(OgLandmark));  // to exclude US Capitol
			case religionSubmenuId: return hasOg(OgWorship) || hasOg(OgCemetery) || hasOg(OgBteReligious);
			case entertainmentSubmenuId: return hasOg(OgStadium) || hasOg(OgOpera) || hasOg(OgNiteClub) || hasOg(OgZoo) || hasOg(OgStateFair)
					// || hasOg(OgCommercialCinema) || hasOg(OgCommercialMaxisSimTheatre) || (hasOg(OgCommercialMovie) && !hasOg(OgCommercialDrivein))
					|| hasOg(OgCasino) || hasOg(OgTvStation)
					|| PropertyUtil::arrayContains(propHolder, queryExemplarGuidPropId, nullptr, queryExemplarGuidProp_RadioStation)
					|| hasOg(OgSgEntertainment) || hasOg(OgBteCommEntertainment);

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
						&& !belongsToSubmenu(propHolder, r1SubmenuId)
						&& !belongsToSubmenu(propHolder, r2SubmenuId)
						&& !belongsToSubmenu(propHolder, r3SubmenuId)
						&& !belongsToSubmenu(propHolder, governmentSubmenuId)
						&& !belongsToSubmenu(propHolder, religionSubmenuId)
						&& !belongsToSubmenu(propHolder, entertainmentSubmenuId)
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

			case seaportButtonId:
				return bool2tri(hasOg(OgWaterTransit)
						&& !belongsToSubmenu(propHolder, portFerrySubmenuId)
						&& !belongsToSubmenu(propHolder, canalSubmenuId)
						// && !belongsToSubmenu(propHolder, seawallSubmenuId)
						&& !belongsToSubmenu(propHolder, waterfrontSubmenuId)
						// && !belongsToSubmenu(propHolder, watercraftSubmenuId)
					);

			case powerButtonId:
				return bool2tri(hasOg(OgPower)  // originally also excludes OgLandfill
						&& !belongsToSubmenu(propHolder, energyDirtySubmenuId)
						&& !belongsToSubmenu(propHolder, energyCleanSubmenuId)
						&& !belongsToSubmenu(propHolder, miscPowerSubmenuId)
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

			// for buildings in submenus (e.g. Religion), we keep a copy here
			// if they are *conditional* reward buildings (to make them easy to find when unlocked)
			case rewardButtonId:
				return bool2tri(hasOg(OgReward) && (
						isConditional(propHolder)
						|| !belongsToSubmenu(propHolder, governmentSubmenuId)
						&& !belongsToSubmenu(propHolder, religionSubmenuId)
						&& !belongsToSubmenu(propHolder, entertainmentSubmenuId)
					));

			case parkButtonId:
				return bool2tri(hasOg(OgPark)
						&& !belongsToSubmenu(propHolder, canalSubmenuId)
						&& !belongsToSubmenu(propHolder, religionSubmenuId)
						&& !belongsToSubmenu(propHolder, entertainmentSubmenuId)
					);

			default:
				return Categorization::TriState::Maybe; // other top level menus require evaluating the original inclusion/exclusion rules of their occupant groups
		}
	}
}
