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
#include "cISCResExemplar.h"
#include <cstdint>

static const uint32_t GZIID_cIExemplarLoadHookTarget = 0x80590E91;

/**
 * @brief Defines the callback that is executed on a successful exemplar load.
 */
class cIExemplarLoadHookTarget : public cIGZUnknown
{
public:

	/**
	 * @brief The callback that is executed on a successful exemplar load.,
	 * @param originalFunctionName The name of the resource factory function that
	 *  loaded the exemplar, used for debugging.
	 * @param key The type, group, and instance (TGI) information for the exemplar.
	 * @param resExemplar The exemplar data.
	 */
	virtual void ExemplarLoaded(
		const char* const originalFunctionName,
		const cGZPersistResourceKey& key,
		cISCResExemplar* resExemplar) = 0;
};