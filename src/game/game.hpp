#pragma once

#define WEAK __declspec(selectany)
#define NAKED __declspec(naked)

// t6 no sp...
#define SELECT(mp_, notmp) (game::is_mp() ? mp_ : notmp)
#define ASSIGN(type, mp, notmp) reinterpret_cast<type>(SELECT(mp, notmp))
#define CALL_ADDR(mp, notmp) ASSIGN(void*, mp, notmp)

#define ARRAY_COUNT(arrayn) \
	((sizeof(arrayn)) / (sizeof(arrayn[0])))

#define ASSERT_STRUCT_SIZE(structure, size) static_assert(sizeof(structure) == size, "sizeof(" #structure ") != " #size);
#define ASSERT_STRUCT_OFFSET(structure, member, offset) static_assert(offsetof(structure, member) == offset, "offsetof(" #structure ", " #member ") != " #offset);

ASSERT_STRUCT_SIZE(int, 4);
ASSERT_STRUCT_SIZE(char, 1);
ASSERT_STRUCT_SIZE(short, 2);
ASSERT_STRUCT_SIZE(float, 4);
ASSERT_STRUCT_SIZE(double, 8);
ASSERT_STRUCT_SIZE(long, 4);
ASSERT_STRUCT_SIZE(long long, 8);
ASSERT_STRUCT_SIZE(void*, 4);
ASSERT_STRUCT_SIZE(jmp_buf, 4 * 16);
ASSERT_STRUCT_SIZE(CRITICAL_SECTION, 0x18);

namespace game
{
	enum gamemode
	{
		iw5mp,
		t4mp,
		t4sp,
		t5mp,
		t5sp,
		t6mp,
		t6zm
	};

	gamemode get_gamemode();

	bool is_iw5();
	bool is_t4();
	bool is_t5();
	bool is_t6();

	bool is_mp();
	bool is_sp();
	bool is_zm();
	
	bool is_dedi();
	bool is_lan();

	const std::filesystem::path& get_storage_location();

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t mp, const size_t notmp)
			: mp_(reinterpret_cast<T*>(mp))
			, notmp_(reinterpret_cast<T*>(notmp))
		{
		}

		T* get() const
		{
			if (is_mp())
			{
				return mp_;
			}

			return notmp_;
		}

		void set(const size_t ptr)
		{
			this->mp_ = reinterpret_cast<T*>(ptr);
			this->notmp_ = reinterpret_cast<T*>(ptr);
		}

		operator T* () const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

	private:
		T* mp_;
		T* notmp_;
	};
}

#include "iw5/structs.hpp"
#include "iw5/symbols.hpp"

#include "t4/structs.hpp"
#include "t4/symbols.hpp"

#include "t5/structs.hpp"
#include "t5/symbols.hpp"

#include "t6/structs.hpp"
#include "t6/symbols.hpp"
