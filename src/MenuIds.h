#pragma once

// static menu button IDs
#define FLORA_BUTTON_ID 0x4a22ea06
#define ZONE_R_BUTTON_ID 0x29920899
#define ZONE_C_BUTTON_ID 0xa998af42
#define ZONE_I_BUTTON_ID 0xc998af00
#define ROADWAY_BUTTON_ID 0x6999bf56
#define HIGHWAY_BUTTON_ID 0x31
#define RAIL_BUTTON_ID 0x29
#define MISCTRANSP_BUTTON_ID 0x299237bf
#define AIRPORT_BUTTON_ID 0xe99234b3
#define SEAPORT_BUTTON_ID 0xa99234a6
#define POWER_BUTTON_ID 0x35
#define WATER_BUTTON_ID 0x39
#define GARBAGE_BUTTON_ID 0x40
#define POLICE_BUTTON_ID 0x37
#define FIRE_BUTTON_ID 0x38
#define EDUCATION_BUTTON_ID 0x42
#define HEALTH_BUTTON_ID 0x89dd5405
#define LANDMARK_BUTTON_ID 0x09930709
#define REWARD_BUTTON_ID 0x34
#define PARK_BUTTON_ID 0x3

#define NAM_CONTROLLER_MARKER_BUTTON_ID 0x6a47ffff


static constexpr uint32_t exemplarNamePropId = 0x20;
static constexpr uint32_t occupantGroupsPropId = 0xaa1dd396;
static constexpr uint32_t itemSubmenuParentPropId = 0xaa1dd399;  // occupantGroupsAltPropId
static constexpr uint32_t capacitySatisfiedPropId = 0x27812834;
static constexpr uint32_t budgetItemDepartmentPropId = 0xea54d283;
static constexpr uint32_t budgetItemDepartmentProp_HealthCoverage = 0xaa538cb3;
static constexpr uint32_t budgetItemDepartmentProp_GovernmentBuildings = 0xea59717a;
static constexpr uint32_t occupantSizePropId = 0x27812810;
static constexpr uint32_t powerPlantTypePropId = 0x27812853;
enum PowerPlantType : uint32_t { Coal = 1, Hydrogen = 2, NaturalGas = 3, Nuclear = 5, Oil = 6, Solar = 7, Waste = 8, Wind = 9, Auxiliary = 0xA };
static constexpr uint32_t conditionalBuildingPropId = 0xea3209f8;

static constexpr uint32_t railButtonId = RAIL_BUTTON_ID;
static constexpr uint32_t miscTransitButtonId = MISCTRANSP_BUTTON_ID;
static constexpr uint32_t seaportButtonId = SEAPORT_BUTTON_ID;

static constexpr uint32_t powerButtonId = POWER_BUTTON_ID;

static constexpr uint32_t policeButtonId = POLICE_BUTTON_ID;
static constexpr uint32_t educationButtonId = EDUCATION_BUTTON_ID;
static constexpr uint32_t healthButtonId = HEALTH_BUTTON_ID;
static constexpr uint32_t landmarkButtonId = LANDMARK_BUTTON_ID;
static constexpr uint32_t rewardButtonId = REWARD_BUTTON_ID;
static constexpr uint32_t parkButtonId = PARK_BUTTON_ID;

static constexpr uint32_t OgRail = 0x1300;
static constexpr uint32_t OgBus = 0x1301;
static constexpr uint32_t OgSubway = 0x1302;
static constexpr uint32_t OgLightrail = 0x1303;
static constexpr uint32_t OgPassengerRail = 0x1305;
static constexpr uint32_t OgFreightRail = 0x1306;
static constexpr uint32_t OgMonorail = 0x1307;
static constexpr uint32_t OgCarFerry = 0x1308;
static constexpr uint32_t OgPassengerFerry = 0x1309;
static constexpr uint32_t OgMiscTransit = 0x130A;
static constexpr uint32_t OgAirport = 0x1508;
static constexpr uint32_t OgSeaport = 0x1509;
static constexpr uint32_t OgWaterTransit = 0x1519;

static constexpr uint32_t OgBteWaterfront = 0xB5C00DD6;
static constexpr uint32_t OgBteInlandWaterways = 0xB5C00DD8;
static constexpr uint32_t OgSgWaterway = 0xB5C00185;

static constexpr uint32_t OgPower = 0x1400;

static constexpr uint32_t OgPolice = 0x1500;
static constexpr uint32_t OgJail = 0x1501;
static constexpr uint32_t OgPoliceBig = 0x150D;
static constexpr uint32_t OgPoliceSmall = 0x150E;
static constexpr uint32_t OgPoliceDeluxe = 0x1515;
static constexpr uint32_t OgPoliceKiosk = 0x1516;

static constexpr uint32_t OgSchool = 0x1503;  // there is no dedicated education OG
static constexpr uint32_t OgCollege = 0x1504;
static constexpr uint32_t OgLibrary = 0x1505;
static constexpr uint32_t OgMuseum = 0x1506;
static constexpr uint32_t OgSchoolElementary = 0x150F;
static constexpr uint32_t OgSchoolHigh = 0x1510;
static constexpr uint32_t OgSchoolPrivate = 0x1514;

static constexpr uint32_t OgHealth = 0x1507;
static constexpr uint32_t OgHospital = 0x1513;
static constexpr uint32_t OgHealthLarge = 0x151a;
static constexpr uint32_t OgHealthOther = 0x151c;

static constexpr uint32_t OgLandmark = 0x150a;
static constexpr uint32_t OgR1 = 0x11010;
static constexpr uint32_t OgR2 = 0x11020;
static constexpr uint32_t OgR3 = 0x11030;
static constexpr uint32_t OgCs1 = 0x13110;
static constexpr uint32_t OgCs2 = 0x13120;
static constexpr uint32_t OgCs3 = 0x13130;
static constexpr uint32_t OgCo2 = 0x13320;
static constexpr uint32_t OgCo3 = 0x13330;
static constexpr uint32_t OgIa = 0x14100;
static constexpr uint32_t OgId = 0x14200;
static constexpr uint32_t OgIm = 0x14300;
static constexpr uint32_t OgIht = 0x14400;

static constexpr uint32_t OgReward = 0x150B;
static constexpr uint32_t OgMayorHouse = 0x1938;
static constexpr uint32_t OgCourthouse = 0x1511;  // or city hall
static constexpr uint32_t OgBureaucracy = 0x1905;  // DMV
static constexpr uint32_t OgStockExchange = 0x1913;  // Biz Lawyer Attack
static constexpr uint32_t OgConventionCrowd = 0x1921;
static constexpr uint32_t OgWorship = 0x1907;
static constexpr uint32_t OgCemetery = 0x1700;
static constexpr uint32_t OgBteReligious = 0xB5C00DDF;
static constexpr uint32_t OgStadium = 0x1906;
static constexpr uint32_t OgOpera = 0x1909;
static constexpr uint32_t OgNiteClub = 0x1908;
static constexpr uint32_t OgZoo = 0x1702;
static constexpr uint32_t OgStateFair = 0x1925;
static constexpr uint32_t OgCasino = 0x1940;
static constexpr uint32_t OgCommercialCinema = 0x1112;
static constexpr uint32_t OgCommercialMaxisSimTheatre = 0x1113;
static constexpr uint32_t OgCommercialMovie = 0x21003;
static constexpr uint32_t OgCommercialDrivein = 0x1103;
static constexpr uint32_t OgSgEntertainment = 0xB5C00157;
static constexpr uint32_t OgBteCommEntertainment = 0xB5C00A0A;

static constexpr uint32_t OgPark = 0x1006;

static constexpr uint32_t passengerRailSubmenuId = 0x35380C75;
static constexpr uint32_t freightRailSubmenuId = 0x3557F0A1;
static constexpr uint32_t yardsSubmenuId = 0x39BA25C7;
static constexpr uint32_t hybridRailwaySubmenuId = 0x2B294CC2;
static constexpr uint32_t monorailSubmenuId = 0x3A1D9854;

static constexpr uint32_t busSubmenuId = 0x1FDDE184;
static constexpr uint32_t glrSubmenuId = 0x26B51B28;
static constexpr uint32_t elRailSubmenuId = 0x244F77E1;
static constexpr uint32_t subwaySubmenuId = 0x231A97D3;
static constexpr uint32_t multiModalStationsSubmenuId = 0x322C7959;
static constexpr uint32_t parkingSubmenuId = 0x217B6C35;

static constexpr uint32_t portFerrySubmenuId = 0x07047B22;
static constexpr uint32_t canalSubmenuId = 0x03C6629C;
static constexpr uint32_t seawallSubmenuId = 0x1CD18678;
static constexpr uint32_t waterfrontSubmenuId = 0x84D42CD6;

static constexpr uint32_t energyDirtySubmenuId = 0x4B465151;
static constexpr uint32_t energyCleanSubmenuId =0xCDE0316B;
static constexpr uint32_t miscPowerSubmenuId = 0xD013F32D;

static constexpr uint32_t policeSmallSubmenuId = 0x65D88585;
static constexpr uint32_t policeLargeSubmenuId = 0x7D6DC8BC;
static constexpr uint32_t policeDeluxeSubmenuId = 0x8157CA0E;

static constexpr uint32_t elementarySchoolSubmenuId = 0x9FE5C428;
static constexpr uint32_t highSchoolSubmenuId = 0xA08063D0;
static constexpr uint32_t collegeSubmenuId = 0xAC706063;
static constexpr uint32_t libraryMuseumSubmenuId = 0xAEDD9FAA;

static constexpr uint32_t healthSmallSubmenuId = 0xB1F7AC5B;  // medical clinic
static constexpr uint32_t healthMediumSubmenuId = 0xB7B594D6;  // hospital with helicopter
static constexpr uint32_t healthLargeSubmenuId = 0xBC251B69;  // medical center with helicopter

static constexpr uint32_t governmentSubmenuId = 0x9FAF7A3B;
static constexpr uint32_t religionSubmenuId = 0x26EB3057;
static constexpr uint32_t entertainmentSubmenuId = 0xBE9FDA0C;

static constexpr uint32_t r1SubmenuId = 0x93DADFE9;
static constexpr uint32_t r2SubmenuId = 0x984E5034;
static constexpr uint32_t r3SubmenuId = 0x9F83F133;
static constexpr uint32_t cs1SubmenuId = 0x11BF1CA9;
static constexpr uint32_t cs2SubmenuId = 0x24C43253;
static constexpr uint32_t cs3SubmenuId = 0x9BDEFE2B;
static constexpr uint32_t co2SubmenuId = 0xA7FF7CF0;
static constexpr uint32_t co3SubmenuId = 0xE27B7EF6;
static constexpr uint32_t iaSubmenuId = 0xC220B7D8;
static constexpr uint32_t idSubmenuId = 0x62D82695;
static constexpr uint32_t imSubmenuId = 0x68B3E5FD;
static constexpr uint32_t ihtSubmenuId = 0x954E20FE;
