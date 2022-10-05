#pragma once


/*
 * 1.2.2
 * Adaptation of file structural changes;
 * Init log statement changed to INFO level;
 * 
 * 1.2.1
 * Macro expansion;
 * 
 * 1.2.0
 * Changed log level controls;
 * 
 * 1.1.0
 * F4SE integration;
 * 
 * 1.0.0
 * Basic spdlog implementation;
 *
 */


#define DKU_L_VERSION_MAJOR 1
#define DKU_L_VERSION_MINOR 2
#define DKU_L_VERSION_REVISION 2


#include "Impl/PCH.hpp"


#ifndef DKU_DISABLE_LOGGING
#	include <spdlog/sinks/basic_file_sink.h>
#	include <spdlog/spdlog.h>

#	define __SHORTF__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#	define INFO(...)                                                                       \
		{                                                                                  \
			std::source_location src = std::source_location::current();                    \
			spdlog::log(spdlog::source_loc{ src.file_name(), static_cast<int>(src.line()), \
							src.function_name() },                                         \
				spdlog::level::info, __VA_ARGS__);                                         \
		}
#	define DEBUG(...)                                                                      \
		{                                                                                  \
			std::source_location src = std::source_location::current();                    \
			spdlog::log(spdlog::source_loc{ src.file_name(), static_cast<int>(src.line()), \
							src.function_name() },                                         \
				spdlog::level::debug, __VA_ARGS__);                                        \
		}
#	define ERROR(...)                                                                             \
		{                                                                                         \
			std::source_location src = std::source_location::current();                           \
			const auto msg = fmt::format(__VA_ARGS__);                                            \
			const auto error = fmt::format("Error occured at code -> [{}:{}]\n{}\n{}\n",          \
				__SHORTF__, src.line(), __FUNCTION__, msg);                                       \
			spdlog::default_logger_raw()->log(                                                    \
				spdlog::source_loc{ src.file_name(), static_cast<int>(src.line()),                \
					src.function_name() },                                                        \
				spdlog::level::critical, __VA_ARGS__);                                            \
			MessageBoxA(nullptr, error.c_str(), Plugin::NAME.data(), MB_OK | MB_ICONEXCLAMATION); \
			ExitProcess('EXIT');                                                                  \
		}

#	define ENABLE_DEBUG DKUtil::Logger::SetLevel(spdlog::level::debug);
#	define DISABLE_DEBUG DKUtil::Logger::SetLevel(spdlog::level::info);

#	ifndef LOG_PATH

#		if defined(F4SEAPI)
#			define LOG_PATH "My Games\\Fallout4\\F4SE"sv
#		elif defined(SKSEAPI)
#			define LOG_PATH "My Games\\Skyrim Special Edition\\SKSE"sv
#		else
#			error "Neither F4SE nor SKSE mode enabled, and LOG_PATH is undefined!"
#		endif

#	endif

#else

#	define DEBUG(...) void(0)
#	define ERROR(...) void(0)
#	define INFO(...) void(0)

#	define ENABLE_DEBUG void(0)
#	define DISABLE_DEBUG void(0)

#endif


namespace DKUtil
{
	constexpr auto DKU_L_VERSION = DKU_L_VERSION_MAJOR * 10000 + DKU_L_VERSION_MINOR * 100 + DKU_L_VERSION_REVISION;
}  // namespace DKUtil


namespace DKUtil::Logger
{
	// From CommonLibSSE https://github.com/Ryan-rsm-McKenzie/CommonLibSSE
	inline std::optional<std::filesystem::path> log_directory()
	{
		wchar_t* buffer{ nullptr };
		const auto result = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, std::addressof(buffer));
		std::unique_ptr<wchar_t[], decltype(&CoTaskMemFree)> knownPath(buffer, CoTaskMemFree);
		if (!knownPath || result != S_OK) {
			return std::nullopt;
		}

		std::filesystem::path path = knownPath.get();
		path /= LOG_PATH;

		return path;
	}


#ifndef DKU_DISABLE_LOGGING
	inline void Init(const std::string_view a_name, const std::string_view a_version)
	{
		auto path = log_directory();

		*path /= a_name;
		*path += ".log"sv;

		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

#	ifndef NDEBUG
		sink->set_pattern("[%i][%l](%s:%#) %v"s);
#	else
		sink->set_pattern("[%D %T][%l](%s:%#) %v"s);
#	endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#	ifndef NDEBUG
		log->set_level(spdlog::level::debug);
#	else
		log->set_level(spdlog::level::info);
#	endif
		log->flush_on(spdlog::level::debug);

		set_default_logger(std::move(log));

#	if defined(F4SEAPI)
#		define MODE "Fallout 4"
#	elif defined(SKSEAPI)
#		define MODE "Skyrim Special Edition"
#	else
#		define MODE "DKUtil"
#	endif

		INFO("Logger init - {} {}", MODE, a_version);
	}


	inline void SetLevel(const spdlog::level::level_enum a_level)
	{
		spdlog::default_logger()->set_level(a_level);
	}
#endif
}  // namespace DKUtil::Logger
