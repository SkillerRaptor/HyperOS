/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <Kernel/Common/Logger.hpp>
#include <Kernel/Common/Memory.hpp>
#include <Kernel/Interrupts/APIC.hpp>
#include <Kernel/Interrupts/IDT.hpp>
#include <Kernel/Memory/PhysicalMemoryManager.hpp>
#include <Kernel/Memory/VirtualMemoryManager.hpp>
#include <Kernel/Scheduling/Scheduler.hpp>
#include <Kernel/System/CPU.hpp>
#include <Kernel/System/SMP.hpp>

namespace Kernel
{
	pid_t Scheduler::s_task_count{};
	tid_t Scheduler::s_thread_count{};
	Map<ssize_t, Task> Scheduler::s_task_list{};
	Spinlock Scheduler::s_spinlock{};

	void Scheduler::initialize()
	{
		Logger::info("Scheduler: Initializing...");

		s_task_count = 0;
		s_task_list = {};

		IDT::set_handler(0x20, IDT::HandlerType::Present | IDT::HandlerType::InterruptGate, Scheduler::schedule);

		Logger::info("Scheduler: Initializing finished!");
	}

	pid_t Scheduler::create_task(pid_t ppid, PageMap* page_map)
	{
		Task task{};
		task.pid = s_task_count++;
		task.ppid = -1;
		task.state = Task::State::Waiting;
		task.idle_count = 0;
		task.page_map = VirtualMemoryManager::kernel_page_map();
		task.threads = {};

		if (ppid != -1)
		{
			if (s_task_list[ppid].pid != -1)
			{
				task.ppid = ppid;
			}
		}

		if (page_map != nullptr)
		{
			task.page_map = page_map;
		}

		s_task_list[task.pid] = task;
		return task.pid;
	}

	tid_t Scheduler::create_thread(pid_t pid, uint64_t rip, uint16_t cs)
	{
		if (pid == -1)
		{
			return -1;
		}

		if (s_task_list[pid].pid == -1)
		{
			return -1;
		}

		Thread thread{};
		thread.tid = s_thread_count++;
		thread.state = Task::State::Waiting;
		thread.idle_count = 0;
		thread.error = 0;
		thread.kernel_stack = reinterpret_cast<uintptr_t>(PhysicalMemoryManager::callocate(2));
		thread.kernel_stack_size = 0;
		thread.registers = {};
		thread.registers.rip = rip;
		thread.registers.cs = cs;
		thread.registers.flags = 0x202;
		thread.registers.rsp = thread.kernel_stack + s_physical_memory_offset;
		thread.registers.ss = cs + 8;

		s_task_list[pid].threads[thread.tid] = thread;

		return 0;
	}

	void Scheduler::schedule(Registers* registers)
	{
		s_spinlock.lock();

		pid_t next_pid = 0;

		size_t task_idle_count = 0;
		for (pid_t i = 0; i < static_cast<pid_t>(s_task_list.size()); ++i)
		{
			Task& next_task = s_task_list[i];
			++next_task.idle_count;

			if (next_task.state == Task::State::Waiting && task_idle_count < next_task.idle_count)
			{
				task_idle_count = next_task.idle_count;
				next_pid = next_task.pid;
			}
		}

		CPU::Data& cpu_data = SMP::local_cpu();

		if (next_pid == -1)
		{
			if (cpu_data.pid == -1)
			{
				s_spinlock.unlock();
				return;
			}

			next_pid = cpu_data.pid;
		}

		Task& next_task = s_task_list[next_pid];
		tid_t next_tid = 0;

		size_t thread_idle_count = 0;
		for (tid_t i = 0; i < static_cast<tid_t>(next_task.threads.size()); ++i)
		{
			Thread& next_thread = next_task.threads[i];
			++next_thread.idle_count;

			if (next_thread.state == Task::State::Waiting && thread_idle_count < next_thread.idle_count)
			{
				thread_idle_count = next_thread.idle_count;
				next_tid = next_thread.tid;
			}
		}

		if (next_tid == -1)
		{
			s_spinlock.unlock();
			return;
		}

		Thread& next_thread = next_task.threads[next_tid];
		if (cpu_data.pid != -1 && cpu_data.tid != -1)
		{
			Task& last_task = s_task_list[cpu_data.pid];
			Thread& last_thread = last_task.threads[cpu_data.tid];

			last_task.state = Task::State::Waiting;
			last_thread.state = Task::State::Waiting;

			last_thread.registers = *registers;
		}

		cpu_data.pid = next_pid;
		cpu_data.tid = next_tid;
		cpu_data.error = next_thread.error;

		cpu_data.page_map = next_task.page_map;
		VirtualMemoryManager::switch_page_map(cpu_data.page_map);

		next_task.idle_count = 0;
		next_thread.idle_count = 0;

		next_task.state = Task::State::Running;
		next_thread.state = Task::State::Running;

		APIC::lapic_end_of_interrupt();

		s_spinlock.unlock();

		/*
		Logger::debug(
			"Register dump: 1\n"
			"          rax=0x%16X rbx=0x%16X rcx=0x%16X rdx=0x%16X\n"
			"          rsi=0x%16X rdi=0x%16X rbp=0x%16X rsp=0x%16X\n"
			"           r8=0x%16X  r9=0x%16X r10=0x%16X r11=0x%16X\n"
			"          r12=0x%16X r13=0x%16X r14=0x%16X r15=0x%16X\n"
			"          rip=0x%16X  cs=0x%16X  ss=0x%16X flg=0x%16X",
			registers->rax,
			registers->rbx,
			registers->rcx,
			registers->rdx,
			registers->rsi,
			registers->rdi,
			registers->rbp,
			registers->rsp,
			registers->r8,
			registers->r9,
			registers->r10,
			registers->r11,
			registers->r12,
			registers->r13,
			registers->r14,
			registers->r15,
			registers->rip,
			registers->cs,
			registers->ss,
			registers->flags);

		Logger::debug(
			"Register dump: 2\n"
			"          rax=0x%16X rbx=0x%16X rcx=0x%16X rdx=0x%16X\n"
			"          rsi=0x%16X rdi=0x%16X rbp=0x%16X rsp=0x%16X\n"
			"           r8=0x%16X  r9=0x%16X r10=0x%16X r11=0x%16X\n"
			"          r12=0x%16X r13=0x%16X r14=0x%16X r15=0x%16X\n"
			"          rip=0x%16X  cs=0x%16X  ss=0x%16X flg=0x%16X",
			s_task_list[next_pid].threads[next_tid].registers.rax,
			s_task_list[next_pid].threads[next_tid].registers.rbx,
			s_task_list[next_pid].threads[next_tid].registers.rcx,
			s_task_list[next_pid].threads[next_tid].registers.rdx,
			s_task_list[next_pid].threads[next_tid].registers.rsi,
			s_task_list[next_pid].threads[next_tid].registers.rdi,
			s_task_list[next_pid].threads[next_tid].registers.rbp,
			s_task_list[next_pid].threads[next_tid].registers.rsp,
			s_task_list[next_pid].threads[next_tid].registers.r8,
			s_task_list[next_pid].threads[next_tid].registers.r9,
			s_task_list[next_pid].threads[next_tid].registers.r10,
			s_task_list[next_pid].threads[next_tid].registers.r11,
			s_task_list[next_pid].threads[next_tid].registers.r12,
			s_task_list[next_pid].threads[next_tid].registers.r13,
			s_task_list[next_pid].threads[next_tid].registers.r14,
			s_task_list[next_pid].threads[next_tid].registers.r15,
			s_task_list[next_pid].threads[next_tid].registers.rip,
			s_task_list[next_pid].threads[next_tid].registers.cs,
			s_task_list[next_pid].threads[next_tid].registers.ss,
			s_task_list[next_pid].threads[next_tid].registers.flags);
		*/
		
		switch_task(&s_task_list[next_pid].threads[next_tid].registers);
	}
} // namespace Kernel
