#pragma once
#include "cGZPersistResourceKey.h"
#include <functional>

// a hash implementation of cGZPersistResourceKey for use in std collections
template<> struct std::hash<const cGZPersistResourceKey>
{
	std::size_t operator()(const cGZPersistResourceKey& key) const noexcept
	{
		std::size_t prime = 4229;
		std::size_t result = 1;
		result = prime * result + std::hash<std::uint32_t>{}(key.type);
		result = prime * result + std::hash<std::uint32_t>{}(key.instance);
		result = prime * result + std::hash<std::uint32_t>{}(key.group);
		return result;
	}
};

// an equality implementation of cGZPersistResourceKey for use in std collections
template<> struct std::equal_to<const cGZPersistResourceKey>
{
	constexpr bool operator()(const cGZPersistResourceKey& lhs, const cGZPersistResourceKey& rhs) const
	{
		return lhs.instance == rhs.instance && lhs.group == rhs.group && lhs.type == rhs.type;
	}
};

