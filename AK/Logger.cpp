/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <AK/Logger.hpp>
#include <AK/Serial.hpp>

namespace AK
{
	Spinlock Logger::s_spinlock{};
	
	void Logger::log(Logger::Level level, const char* string, ...)
	{
		s_spinlock.lock();
		
		va_list args;
		va_start(args, string);
		
		vlog(level, string, args);
		
		va_end(args);
		
		s_spinlock.unlock();
	}
	
	void Logger::vlog(Logger::Level level, const char* string, va_list args)
	{
		const char* level_string = "";
		
		switch (level)
		{
		case Logger::Level::Info:
			level_string = "\033[39m[\033[32m INFO \033[39m] ";
			break;
		case Logger::Level::Warning:
			level_string = "\033[39m[\033[33m WARNING \033[39m] ";
			break;
		case Logger::Level::Error:
			level_string = "\033[39m[\033[31m ERROR \033[39m] ";
			break;
		case Logger::Level::Debug:
			level_string = "\033[39m[\033[36m DEBUG \033[39m] ";
			break;
		}
		
		Serial::write(level_string);
		
		while (*string)
		{
			if (*string == '%')
			{
				++string;
				switch (*string)
				{
				case 'c':
				{
					auto c = static_cast<char>(va_arg(args, int));
					Serial::write(c);
					break;
				}
				case 'd':
				{
					int32_t i = va_arg(args, int32_t);
					if (i < 0)
					{
						i = -i;
						Serial::write('-');
					}
					Serial::write(Logger::convert_string(i, 10));
					break;
				}
				case 'u':
				{
					uint64_t i = va_arg(args, uint64_t);
					Serial::write(Logger::convert_string(i, 10));
					break;
				}
				case 'o':
				{
					uint64_t i = va_arg(args, uint64_t);
					Serial::write(Logger::convert_string(i, 8));
					break;
				}
				case 's':
				{
					char* str = va_arg(args, char*);
					Serial::write(str);
					break;
				}
				case 'x':
				{
					uint64_t i = va_arg(args, uint64_t);
					Serial::write("0x");
					Serial::write(Logger::convert_string(i, 16));
					break;
				}
				default:
					Serial::write('%');
					break;
				}
			}
			else
			{
				Serial::write(*string);
			}
			
			++string;
		}
		
		Serial::write('\n');
	}
	
	void Logger::info(const char* string, ...)
	{
		s_spinlock.lock();
		
		va_list args;
		va_start(args, string);
		
		vlog(Logger::Level::Info, string, args);
		
		va_end(args);
		
		s_spinlock.unlock();
	}
	
	void Logger::warning(const char* string, ...)
	{
		s_spinlock.lock();
		
		va_list args;
		va_start(args, string);
		
		vlog(Logger::Level::Warning, string, args);
		
		va_end(args);
		
		s_spinlock.unlock();
	}
	
	void Logger::error(const char* string, ...)
	{
		s_spinlock.lock();
		
		va_list args;
		va_start(args, string);
		
		vlog(Logger::Level::Error, string, args);
		
		va_end(args);
		
		s_spinlock.unlock();
	}
	
	void Logger::debug(const char* string, ...)
	{
		s_spinlock.lock();
		
		va_list args;
		va_start(args, string);
		
		vlog(Logger::Level::Debug, string, args);
		
		va_end(args);
		
		s_spinlock.unlock();
	}
	
	char* Logger::convert_string(uint64_t number, int64_t base)
	{
		static char representation[] = "0123456789ABCDEF";
		
		static char buffer[50];
		char* ptr;
		
		ptr = &buffer[49];
		*ptr = '\0';
		
		do
		{
			*--ptr = representation[number % base];
			number /= base;
		} while (number != 0);
		
		return ptr;
	}
}