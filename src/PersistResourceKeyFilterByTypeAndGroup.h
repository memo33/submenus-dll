#pragma once
#include "cIGZPersistResourceKeyFilter.h"

static constexpr uint32_t GZCLSID_PersistResourceKeyFilterByTypeAndGroup = 0x40BF53BC;

class PersistResourceKeyFilterByTypeAndGroup : public cIGZPersistResourceKeyFilter
{
	public:

	PersistResourceKeyFilterByTypeAndGroup(const uint32_t typeId, const uint32_t groupId);

	virtual ~PersistResourceKeyFilterByTypeAndGroup();

	bool QueryInterface(uint32_t riid, void** ppvObj);
	uint32_t AddRef();
	uint32_t Release();

	bool IsKeyIncluded(cGZPersistResourceKey const& key);

	private:
	uint32_t refCount;
	uint32_t type;
	uint32_t group;
};
