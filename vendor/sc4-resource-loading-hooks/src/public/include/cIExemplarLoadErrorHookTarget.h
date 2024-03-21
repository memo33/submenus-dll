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
#include "cGZPersistResourceKey.h"
#include "cIGZUnknown.h"

static const uint32_t GZIID_cIExemplarLoadErrorHookTarget = 0x12C54BC2;

/**
 * @brief Defines callbacks that are executed on a failed exemplar load.
 */
class cIExemplarLoadErrorHookTarget : public cIGZUnknown
{
public:

	/**
	 * @brief A callback that is executed on a failed exemplar load.,
	 * @param originalFunctionName The name of the resource factory function that
	 *  loaded the exemplar, used for debugging.
	 * @param riid The requested interface ID for the type.
	 */
	virtual void LoadError(
		const char* const originalFunctionName,
		uint32_t riid) = 0;

	/**
	 * @brief A callback that is executed on a failed exemplar load.,
	 * @param originalFunctionName The name of the resource factory function that
	 *  loaded the exemplar, used for debugging.
	 * @param riid The requested interface ID for the type.
	 * @param key The type, group, and instance (TGI) information for the exemplar.
	 */
	virtual void LoadError(
		const char* const originalFunctionName,
		uint32_t riid,
		const cGZPersistResourceKey& key) = 0;
};
