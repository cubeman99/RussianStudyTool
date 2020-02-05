#pragma once

#include <cmgCore/cmg_core.h>


#define EXPAND_TO_STR(_enum, _name, _, __) #_name,
#define EXPAND_TO_ENUM(_enum, _name, _value, _) k_##_name _value,
#define EXPAND_TO_STR_TO_ENUM(_enum, _name, _value, _short) if (str == #_name) { outValue = _enum::k_##_name; return true; }
#define EXPAND_TO_SHORT_STR_TO_ENUM(_enum, _name, _value, _short) if (str == #_short) { outValue = _enum::k_##_name; return true; }
#define EXPAND_TO_ENUM_TO_STR(_enum, _name, _value, _short) if (value == _enum::k_##_name) return #_name;
#define EXPAND_TO_ENUM_TO_SHORT_STR(_enum, _name, _value, _short) if (value == _enum::k_##_name) return #_short;

#define DECLARE_ENUM(_enum, _enum_macro) \
	enum class _enum \
	{ \
		_enum_macro(_enum, EXPAND_TO_ENUM) \
	}; \
	DECLARE_STR_TO_ENUM(_enum)

#define DECLARE_ENUM_WITH_COUNT(_enum, _enum_macro) \
	enum class _enum \
	{ \
		_enum_macro(_enum, EXPAND_TO_ENUM) \
		k_count,\
	}; \
	DECLARE_STR_TO_ENUM(_enum)

#define DECLARE_STR_TO_ENUM(_enum) \
	template <> \
	bool TryStringToEnum<_enum>(const String& str, _enum& outValue, bool shortName); \
	template <> \
	String EnumToString<_enum>(_enum value, bool shortName);

#define DEFINE_ENUM(_enum, _enum_macro) \
	template <> \
	bool TryStringToEnum<_enum>(const String& str, _enum& outValue, bool shortName) \
	{ \
		if (shortName) \
		{ \
			_enum_macro(_enum, EXPAND_TO_SHORT_STR_TO_ENUM) \
		} \
		else \
		{ \
			_enum_macro(_enum, EXPAND_TO_STR_TO_ENUM) \
		} \
		return false; \
	} \
	template <> \
	String EnumToString<_enum>(_enum value, bool shortName) \
	{ \
		if (shortName) \
		{ \
			_enum_macro(_enum, EXPAND_TO_ENUM_TO_SHORT_STR) \
		} \
		else \
		{ \
			_enum_macro(_enum, EXPAND_TO_ENUM_TO_STR) \
		} \
		return ""; \
	}

template <typename T_Enum>
String EnumToShortString(T_Enum value)
{
	return EnumToString(value, true);
}

template <typename T_Enum>
String EnumToString(T_Enum value, bool shortName = false)
{
	return std::to_string((uint32) value);
}

template <typename T_Enum>
T_Enum StringToEnum(const String& str, bool shortName = false)
{
	T_Enum value = T_Enum(0);
	TryStringToEnum(str, value, shortName);
	return value;
}

template <typename T_Enum>
bool TryStringToEnum(const String& str, T_Enum& outValue, bool shortName = false)
{
	return false;
}


template
<
	class T_Flags,
	typename T_Enum,
	uint32 T_Count = (uint32) T_Enum::k_count,
	typename T_Value = uint32
>
class EnumFlagsIterator
{
public:
	using value_type = std::pair<T_Enum, bool>;

	EnumFlagsIterator(const T_Flags& flags, uint32 index) :
		m_flags(flags),
		m_index(index)
	{
	}

	value_type operator *() const
	{
		T_Enum item = (T_Enum) m_index;
		return value_type(item, m_flags[item]);
	}

	bool operator !=(const EnumFlagsIterator& other) const
	{
		return (m_index != other.m_index);
	}


	const EnumFlagsIterator& operator ++()
	{
		++m_index;
		return *this;
	}

private:
	uint32 m_index;
	const T_Flags& m_flags;
};

template
<
	typename T_Enum,
	uint32 T_Count = (uint32) T_Enum::k_count,
	typename T_Value = uint32
>
class EnumFlags
{
public:
	using this_type = EnumFlags<T_Enum, T_Count, T_Value>;
	using iterator = EnumFlagsIterator<this_type, T_Enum, T_Count, T_Value>;

	EnumFlags() {}
	EnumFlags(T_Enum value)
	{
		Set(value, true);
	}

	static constexpr uint32 GetElementCount()
	{
		uint32 count = T_Count / s_bitsPerElement;
		if (count % 8 != 0)
			count++;
		return count;
	}

	static constexpr uint32 s_bitsPerElement = sizeof(T_Value) * 8;
	static constexpr uint32 s_arrayCount = GetElementCount();


	iterator begin()
	{
		return iterator(*this, 0);
	}

	iterator end()
	{
		return iterator(*this, T_Count);
	}

	bool IsZero() const
	{
		for (uint32 i = 0; i < s_arrayCount; i++)
		{
			if (m_array[i] != T_Value(0))
				return false;
		}
		return true;
	}

	uint32 Size() const
	{
		return s_arrayCount;
	}

	T_Value GetValue(uint32 index) const
	{
		return m_array[index];
	}

	uint32 GetCount() const
	{
		return T_Count;
	}

	bool operator[](T_Enum item) const
	{
		T_Value index = (T_Value) item / s_bitsPerElement;
		T_Value bit = (T_Value) item % s_bitsPerElement;
		return (m_array[index] & (T_Value(1) << bit)) != T_Value(0);
	}

	EnumFlags operator |(const EnumFlags& other) const
	{
		EnumFlags result;
		for (uint32 i = 0; i < s_arrayCount; i++)
			result.m_array[i] = m_array[i] | other.m_array[i];
		return result;
	}

	EnumFlags operator &(const EnumFlags& other) const
	{
		EnumFlags result;
		for (uint32 i = 0; i < s_arrayCount; i++)
			result.m_array[i] = m_array[i] & other.m_array[i];
		return result;
	}

	bool operator <(const EnumFlags& other) const
	{
		return m_value < other.m_value;
	}

	bool operator >(const EnumFlags& other) const
	{
		return m_value > other.m_value;
	}

	bool operator==(const EnumFlags& other) const
	{
		for (uint32 i = 0; i < s_arrayCount; i++)
		{
			if (m_array[i] != other.m_array[i])
				return false;
		}
		return true;
	}

	bool operator!=(const EnumFlags& other) const
	{
		return !(*this == other);
	}

	void Set(T_Enum item, bool enabled)
	{
		T_Value index = (T_Value) item / s_bitsPerElement;
		T_Value bit = (T_Value) item % s_bitsPerElement;
		if (enabled)
			m_array[index] |= (T_Value(1) << bit);
		else
			m_array[index] &= ~(T_Value(1) << bit);
	}

	void Toggle(T_Enum item)
	{
		Set(item, !(*this)[item]);
	}

	EnumFlags& operator |=(const EnumFlags& other)
	{
		for (uint32 i = 0; i < s_arrayCount; i++)
			m_array[i] |= other.m_array[i];
		return *this;
	}

	EnumFlags& operator &=(const EnumFlags& other)
	{
		for (uint32 i = 0; i < s_arrayCount; i++)
			m_array[i] &= other.m_array[i];
		return *this;
	}

private:
	static constexpr T_Value ToValue(T_Enum item)
	{
		return (T_Value(1) << (T_Value) item);
	}

	T_Value m_array[s_arrayCount] = { 0 };
};

