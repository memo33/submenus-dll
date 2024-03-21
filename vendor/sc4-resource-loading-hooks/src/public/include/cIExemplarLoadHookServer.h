///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-resource-loading-hooks, a DLL Plugin for SimCity 4
// that allows other DLLs to modify resources as the game loads them.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "cIGZUnknown.h"

class cIExemplarLoadHookTarget;
class cIExemplarLoadErrorHookTarget;

static const uint32_t GZCLSID_cIExemplarLoadHookServer = 0xFD7BD8C4;
static const uint32_t GZIID_cIExemplarLoadHookServer = 0xFC1A55E1;

/**
 * @brief The class used to register for the exemplar load callbacks.
 */
class cIExemplarLoadHookServer : public cIGZUnknown
{
public:

	/**
	 * @brief Registers to receive a callback on all successful exemplar loads.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadNotification(cIExemplarLoadHookTarget* target) = 0;

	/**
	 * @brief Registers to receive a callback on all successful exemplar loads
	 * with the specified group ID.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @param requiredGroupID The group ID to watch for.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadNotification(
		cIExemplarLoadHookTarget* target,
		uint32_t requiredGroupID) = 0;

	/**
	 * @brief Registers to receive a callback on successful exemplar loads
	 * with the specified group and instance IDs.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @param requiredGroupID The group ID to watch for.
	 * @param requiredInstanceID The instance ID to watch for.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadNotification(
		cIExemplarLoadHookTarget* target,
		uint32_t requiredGroupID,
		uint32_t requiredInstanceID) = 0;

	/**
	 * @brief Unregisters an object from the exemplar load callbacks.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool RemoveLoadNotification(cIExemplarLoadHookTarget* target) = 0;

	/**
	 * @brief Registers an object for the exemplar load error callbacks.
	 * @param target The object that implements cIExemplarLoadErrorHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadErrorNotification(cIExemplarLoadErrorHookTarget* target) = 0;

	/**
	 * @brief Unregisters an object from the exemplar load error callbacks.
	 * @param target The object that implements cIExemplarLoadErrorHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool RemoveLoadErrorNotification(cIExemplarLoadErrorHookTarget* target) = 0;
};