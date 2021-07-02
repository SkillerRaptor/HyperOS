/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <Kernel/main.hpp>
#include <Kernel/Common/Logger.hpp>
#include <Kernel/Common/Serial.hpp>
#include <Kernel/Memory/PhysicalMemoryManager.hpp>
#include <Kernel/Memory/VirtualMemoryManager.hpp>
#include <Kernel/System/IDT.hpp>
#include <Kernel/System/GDT.hpp>
#include <Kernel/System/PIC.hpp>

#if defined(__x86_64__)
#	include <Kernel/Arch/x86_64/Stivale.hpp>
#elif defined(__i386__)
#	error x86_32 is not implemented yet!
#endif

namespace Kernel
{
	void main(stivale2_struct* bootloader_data)
	{
		Serial::initialize();

		auto* firmware_tag = reinterpret_cast<stivale2_struct_tag_firmware*>(
			stivale2_get_tag(bootloader_data, STIVALE2_STRUCT_TAG_FIRMWARE_ID));

		Logger::info(" _  _                     ___  ___    ");
		Logger::info("| || |_  _ _ __  ___ _ _ / _ \\/ __|  ");
		Logger::info("| __ | || | '_ \\/ -_) '_| (_) \\__ \\");
		Logger::info("|_||_|\\_, | .__/\\___|_|  \\___/|___/");
		Logger::info("      |__/|_|                         ");
		Logger::info("");
		Logger::info("Firmware: %s", (firmware_tag->flags & STIVALE2_FIRMWARE_BIOS) ? "BIOS" : "UEFI");
		Logger::info("Bootloader: %s %s", bootloader_data->bootloader_brand, bootloader_data->bootloader_version);
		Logger::info("");
		Logger::info("------------------------------------");
		Logger::info("");

		Logger::info("HyperOS is booting...");
		
		GDT::initialize();
		PIC::remap(0x20, 0x28);
		IDT::initialize();
		
		auto* memory_map_tag = reinterpret_cast<stivale2_struct_tag_memmap*>(
			stivale2_get_tag(bootloader_data, STIVALE2_STRUCT_TAG_MEMMAP_ID));
		PhysicalMemoryManager::initialize(memory_map_tag->memmap, memory_map_tag->entries);
		VirtualMemoryManager::initialize(memory_map_tag->memmap, memory_map_tag->entries);
		
		Logger::info("HyperOS booted successfully!");

		__asm__ __volatile__("sti");

		while (true)
		{
			__asm__ __volatile__("hlt");
		}
	}
} // namespace Kernel

/*

#include <AK/Logger.hpp>
#include <AK/Memory.hpp>
#include <AK/Serial.hpp>
#include <Kernel/System/ACPI.hpp>
#include <Kernel/System/APIC.hpp>
#include <Kernel/System/GDT.hpp>
#include <Kernel/System/HPET.hpp>
#include <Kernel/System/IDT.hpp>
#include <Kernel/System/PIC.hpp>
#include <Kernel/System/SMP.hpp>
#include <stddef.h>

namespace Kernel
{
	using ConstructorFunction = void (*)();
	extern "C" ConstructorFunction constructors_start[];
	extern "C" ConstructorFunction constructors_end[];



		for (ConstructorFunction* constructor = constructors_start; constructor < constructors_end; ++constructor)
		{
			(*constructor)();
		}

		auto* rsdp_tag =
			reinterpret_cast<stivale2_struct_tag_rsdp*>(stivale2_get_tag(bootloader_data, STIVALE2_STRUCT_TAG_RSDP_ID));
		ACPI::initialize(reinterpret_cast<ACPI::RSDP*>(rsdp_tag->rsdp + AK::s_physical_memory_offset));
		APIC::initialize();
		HPET::initialize();

		auto* smp_tag =
			reinterpret_cast<stivale2_struct_tag_smp*>(stivale2_get_tag(bootloader_data, STIVALE2_STRUCT_TAG_SMP_ID));
		SMP::initialize(smp_tag);

		auto* framebuffer_tag = reinterpret_cast<stivale2_struct_tag_framebuffer*>(
			stivale2_get_tag(bootloader_data, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID));
		auto* screen = reinterpret_cast<uint8_t*>(framebuffer_tag->framebuffer_addr);
		for (size_t y = 0; y < framebuffer_tag->framebuffer_height; ++y)
		{
			const size_t screen_position_y = y * framebuffer_tag->framebuffer_pitch;
			for (size_t x = 0; x < framebuffer_tag->framebuffer_width; ++x)
			{
				const size_t screen_position_x = x * (framebuffer_tag->framebuffer_bpp / 8);
				screen[screen_position_x + screen_position_y + 0] = 255;
				screen[screen_position_x + screen_position_y + 1] = 0;
				screen[screen_position_x + screen_position_y + 2] = 255;
			}
		}

} // namespace Kernel
*/