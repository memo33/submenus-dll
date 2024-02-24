#pragma once

#include "cISCProperty.h"
#include "cRZBaseVariant.h"

class cSCBaseProperty : public cISCProperty
{
public:

	cSCBaseProperty();
	cSCBaseProperty(uint32_t propertyID, cIGZVariant const& propertyValue);

	cSCBaseProperty(cSCBaseProperty const& value);

	bool QueryInterface(uint32_t riid, void** ppvObj);
	uint32_t AddRef();
	uint32_t Release();

	uint32_t GetPropertyID() const;
	void SetPropertyID(uint32_t value);

	cIGZVariant* GetPropertyValue();
	const cIGZVariant* GetPropertyValue() const;

	bool SetPropertyValue(const cIGZVariant& value);
	bool Write(cIGZOStream& stream);
	bool Read(cIGZIStream& stream) const;

private:

	uint32_t propertyID;
	cRZBaseVariant propertyValue;
	uint32_t refCount;
};