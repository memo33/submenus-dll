#pragma once
#include "cIExemplarLoadHookTarget.h"
#include "cGZPersistResourceKey.h"
#include "cISCResExemplar.h"
#include "cISCPropertyHolder.h"
#include "PersistResourceKeyHash.h"
#include <unordered_map>
#include <deque>


class ExemplarPatcher : public cIExemplarLoadHookTarget
{
public:

	ExemplarPatcher();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	uint32_t AddRef() override;

	uint32_t Release() override;

	void LoadPatches();
private:
	uint32_t refCount;
	std::unordered_map<const cGZPersistResourceKey, std::deque<cISCPropertyHolder*>*> patches;

	void ExemplarLoaded(
		const char* const originalFunctionName,
		const cGZPersistResourceKey& key,
		cISCResExemplar* resExemplar) override;
};
