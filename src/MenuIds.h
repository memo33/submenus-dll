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

static constexpr uint32_t educationButtonId = EDUCATION_BUTTON_ID;
static constexpr uint32_t landmarkButtonId = LANDMARK_BUTTON_ID;

static constexpr uint32_t schoolOG = 0x1503;  // there is no dedicated education OG
static constexpr uint32_t collegeOG = 0x1504;
static constexpr uint32_t libraryOG = 0x1505;
static constexpr uint32_t museumOG = 0x1506;
static constexpr uint32_t schoolElementaryOG = 0x150F;
static constexpr uint32_t schoolHighOG = 0x1510;
static constexpr uint32_t schoolPrivateOG = 0x1514;

static constexpr uint32_t landmarkOG = 0x150a;
static constexpr uint32_t cs1OG = 0x13110;
static constexpr uint32_t cs2OG = 0x13120;
static constexpr uint32_t cs3OG = 0x13130;
static constexpr uint32_t co2OG = 0x13320;
static constexpr uint32_t co3OG = 0x13330;

static constexpr uint32_t elementarySchoolSubmenuId = 0x9FE5C428;
static constexpr uint32_t highSchoolSubmenuId = 0xA08063D0;
static constexpr uint32_t collegeSubmenuId = 0xAC706063;
static constexpr uint32_t libraryMuseumSubmenuId = 0xAEDD9FAA;

static constexpr uint32_t cs1SubmenuId = 0x11BF1CA9;
static constexpr uint32_t cs2SubmenuId = 0x24C43253;
static constexpr uint32_t cs3SubmenuId = 0x9BDEFE2B;
static constexpr uint32_t co2SubmenuId = 0xA7FF7CF0;
static constexpr uint32_t co3SubmenuId = 0xE27B7EF6;
