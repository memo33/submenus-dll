#pragma once
#include "cGZPersistResourceKey.h"
#include "cISCProperty.h"
#include "cIGZVariant.h"
#include "Logger.h"


namespace PropertyUtil {

	template <typename F>
	void arrayForeach(cISCPropertyHolder* propHolder, const uint32_t propertyId, const cGZPersistResourceKey* keyOrNull, F&& callback)
	{
		if (propHolder->HasProperty((uint32_t)propertyId))
		{
			auto property = propHolder->GetProperty((uint32_t)propertyId);
			property->AddRef();
			const auto variant = property->GetPropertyValue();
			variant->AddRef();
			switch (variant->GetType())
			{
				case cIGZVariant::Type::Uint32:
					uint32_t value;
					variant->GetValUint32(value);
					callback(value);
					break;
				case cIGZVariant::Type::Uint32Array:
					uint32_t reps;
					reps = variant->GetCount();
					if (reps > 0) {
						uint32_t* values = variant->RefUint32();
						for (uint32_t j = 0; j < reps; j++) {
							uint32_t value = values[j];  // it's important to create a copy
							callback(value);
						}
					}
					break;
				default:
					if (keyOrNull) {
						Logger::GetInstance().WriteLineFormatted(
								LogLevel::Error,
								"Property 0x%08X requires type Uint32Array or Uint32: T:0x%08X G:0x%08X I:0x%08X.",
								propertyId, keyOrNull->type, keyOrNull->group, keyOrNull->instance);
					} else {
						Logger::GetInstance().WriteLineFormatted(
								LogLevel::Error,
								"Property 0x%08X requires type Uint32Array or Uint32.",
								propertyId);
					}
					break;
			}
			variant->Release();
			property->Release();
		}
	}

	static bool arrayContains(cISCPropertyHolder* propHolder, const uint32_t propertyId, const cGZPersistResourceKey* keyOrNull, const uint32_t value)
	{
		bool found = false;
		arrayForeach(propHolder, propertyId, keyOrNull, [&found, &value](uint32_t v) {
			found |= (v == value);
		});  // TODO return early
		return found;
	}

	template <typename F>
	static bool arrayExists(cISCPropertyHolder* propHolder, const uint32_t propertyId, const cGZPersistResourceKey* keyOrNull, F&& predicate)
	{
		bool found = false;
		arrayForeach(propHolder, propertyId, keyOrNull, [&found, &predicate](uint32_t v) {
			found |= predicate(v);
		});
		return found;
	}

};
