#include "PersistResourceKeyFilterByTypeAndGroup.h"
#include "cGZPersistResourceKey.h"

PersistResourceKeyFilterByTypeAndGroup::PersistResourceKeyFilterByTypeAndGroup(const uint32_t typeId, const uint32_t groupId)
	: refCount(0), type(typeId), group(groupId)
{
}

PersistResourceKeyFilterByTypeAndGroup::~PersistResourceKeyFilterByTypeAndGroup()
{
}

bool PersistResourceKeyFilterByTypeAndGroup::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIGZPersistResourceKeyFilter)
	{
		*ppvObj = static_cast<cIGZPersistResourceKeyFilter*>(this);
		AddRef();
		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	*ppvObj = nullptr;
	return false;
}

uint32_t PersistResourceKeyFilterByTypeAndGroup::AddRef()
{
	return ++refCount;
}

uint32_t PersistResourceKeyFilterByTypeAndGroup::Release()
{
	if (refCount == 1)
	{
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

bool PersistResourceKeyFilterByTypeAndGroup::IsKeyIncluded(cGZPersistResourceKey const& key)
{
	return this->group == key.group && this->type == key.type;
}
