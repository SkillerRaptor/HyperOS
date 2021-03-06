/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace Kernel
{
	struct Registers
	{
		uint64_t r15;
		uint64_t r14;
		uint64_t r13;
		uint64_t r12;
		uint64_t r11;
		uint64_t r10;
		uint64_t r9;
		uint64_t r8;

		uint64_t rsi;
		uint64_t rdi;
		uint64_t rbp;
		uint64_t rdx;
		uint64_t rcx;
		uint64_t rbx;
		uint64_t rax;

		uint64_t isr;
		uint64_t error;

		uint64_t rip;
		uint64_t cs;
		uint64_t flags;
		uint64_t rsp;
		uint64_t ss;
	};

	struct CpuId
	{
		uint32_t leaf;
		uint32_t sub_leaf;
		uint32_t eax;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
	};
} // namespace Kernel
