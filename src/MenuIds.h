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


static constexpr uint32_t occupantGroupsPropId = 0xaa1dd396;
static constexpr uint32_t itemSubmenuParentPropId = 0xaa1dd399;  // occupantGroupsAltPropId
static constexpr uint32_t capacitySatisfiedPropId = 0x27812834;
static constexpr uint32_t budgetItemDepartmentPropId = 0xea54d283;
static constexpr uint32_t budgetItemDepartmentProp_HealthCoverage = 0xaa538cb3;

static constexpr uint32_t policeButtonId = POLICE_BUTTON_ID;
static constexpr uint32_t educationButtonId = EDUCATION_BUTTON_ID;
static constexpr uint32_t healthButtonId = HEALTH_BUTTON_ID;
static constexpr uint32_t landmarkButtonId = LANDMARK_BUTTON_ID;

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
static constexpr uint32_t OgCs1 = 0x13110;
static constexpr uint32_t OgCs2 = 0x13120;
static constexpr uint32_t OgCs3 = 0x13130;
static constexpr uint32_t OgCo2 = 0x13320;
static constexpr uint32_t OgCo3 = 0x13330;
static constexpr uint32_t OgIa = 0x14100;
static constexpr uint32_t OgId = 0x14200;
static constexpr uint32_t OgIm = 0x14300;
static constexpr uint32_t OgIht = 0x14400;

static constexpr uint32_t policeSmallSubmenuId = 0x65D88585;
static constexpr uint32_t policeMediumSubmenuId = 0x7D6DC8BC;
static constexpr uint32_t policeLargeSubmenuId = 0x8157CA0E;

static constexpr uint32_t elementarySchoolSubmenuId = 0x9FE5C428;
static constexpr uint32_t highSchoolSubmenuId = 0xA08063D0;
static constexpr uint32_t collegeSubmenuId = 0xAC706063;
static constexpr uint32_t libraryMuseumSubmenuId = 0xAEDD9FAA;

static constexpr uint32_t healthSmallSubmenuId = 0xB1F7AC5B;  // medical clinic
static constexpr uint32_t healthMediumSubmenuId = 0xB7B594D6;  // hospital with helicopter
static constexpr uint32_t healthLargeSubmenuId = 0xBC251B69;  // medical center with helicopter

static constexpr uint32_t cs1SubmenuId = 0x11BF1CA9;
static constexpr uint32_t cs2SubmenuId = 0x24C43253;
static constexpr uint32_t cs3SubmenuId = 0x9BDEFE2B;
static constexpr uint32_t co2SubmenuId = 0xA7FF7CF0;
static constexpr uint32_t co3SubmenuId = 0xE27B7EF6;
static constexpr uint32_t iaSubmenuId = 0xC220B7D8;
static constexpr uint32_t idSubmenuId = 0x62D82695;
static constexpr uint32_t imSubmenuId = 0x68B3E5FD;
static constexpr uint32_t ihtSubmenuId = 0x954E20FE;
