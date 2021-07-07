/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <Kernel/Common/Logger.hpp>
#include <Kernel/Common/Memory.hpp>
#include <Kernel/Common/MMIO.hpp>
#include <Kernel/Drivers/HPET.hpp>
#include <Kernel/Interrupts/APIC.hpp>
#include <Kernel/Interrupts/IDT.hpp>
#include <Kernel/Interrupts/PIC.hpp>
#include <Kernel/System/CPU.hpp>

namespace Kernel
{
	void APIC::initialize()
	{
		Logger::info("APIC: Initializing...");

		//PIC::disable();

		IDT::set_handler(0xFF, IDT::HandlerType::Present | IDT::HandlerType::InterruptGate, APIC::lapic_spur_handler);

		uint32_t spur = lapic_read(0xF0);
		lapic_write(0xF0, spur | (1 << 8) | 0xFF);

		Logger::info("APIC: Initializing finished!");
	}
	
	void APIC::calibrate(uint64_t ms)
	{
		lapic_write(0x3E0, 0x3);
		lapic_write(0x380, ~0);
		
		HPET::sleep(ms);
		
		uint32_t ticks = ~0 - lapic_read(0x390);
		
		lapic_write(0x320, 32 | 0x20000);
		lapic_write(0x3E0, 0x3);
		lapic_write(0x380, ticks);
	}

	void APIC::lapic_end_of_interrupt()
	{
		APIC::lapic_write(0xB0, 0);
	}

	void APIC::lapic_write(uint32_t reg, uint32_t data)
	{
		uintptr_t lapic_mmio_base = lapic_get_mmio_base();
		MMIO::outd(reinterpret_cast<void*>(lapic_mmio_base + reg), data);
	}

	uint32_t APIC::lapic_read(uint32_t reg)
	{
		uintptr_t lapic_mmio_base = lapic_get_mmio_base();
		return MMIO::ind(reinterpret_cast<void*>(lapic_mmio_base + reg));
	}

	void APIC::lapic_spur_handler(Registers*)
	{
		lapic_end_of_interrupt();
	}

	uintptr_t APIC::lapic_get_mmio_base()
	{
		return reinterpret_cast<uintptr_t>((CPU::read_msr(0x1B) & 0xFFFFF000) + s_physical_memory_offset);
	}
} // namespace Kernel
