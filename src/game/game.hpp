#pragma once

#define NAKED __declspec(naked)

// t6 no sp...
#define SELECT(mp_, notmp) (game::is_mp() ? mp_ : notmp)
#define ASSIGN(type, mp, notmp) reinterpret_cast<type>(SELECT(mp, notmp))
#define CALL_ADDR(mp, notmp) ASSIGN(void*, mp, notmp)

#define ARRAY_COUNT(arrayn) \
	((sizeof(arrayn)) / (sizeof(arrayn[0])))

struct pushfd_s
{
    volatile unsigned int CF : 1; // Carry
    volatile unsigned int : 1;
    volatile unsigned int PF : 1; // Parity
    volatile unsigned int : 1;
    volatile unsigned int AF : 1; // Auxiliary Carry 
    volatile unsigned int : 1;
    volatile unsigned int ZF : 1; // Zero
    volatile unsigned int SF : 1; // Sign
    volatile unsigned int TF : 1; // Trap
    volatile unsigned int IF : 1; // Interrupt enable
    volatile unsigned int DF : 1; // Direction
    volatile unsigned int OF : 1; // Overflow
    volatile unsigned int pad : 20;
};

struct pushad_s
{
    volatile size_t EDI;
    volatile size_t ESI;
    volatile size_t EBP;
    volatile size_t ESP;
    volatile size_t EBX;
    volatile size_t EDX;
    volatile size_t ECX;
    volatile size_t EAX;
};

typedef volatile size_t retaddr_t;

#define popad_esp   \
        popad       \
  __asm mov esp, [esp - 0x14]

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
		none,
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

	template<typename T>
	constexpr T& pointer(std::uintptr_t addr)
	{
		return *reinterpret_cast<T*>(addr);
	}
}

#include "iw5/structs.hpp"
#include "iw5/symbols.hpp"

#include "t4/structs.hpp"
#include "t4/symbols.hpp"

#include "t5/structs.hpp"
#include "t5/symbols.hpp"

#include "t6/structs.hpp"
#include "t6/symbols.hpp"
