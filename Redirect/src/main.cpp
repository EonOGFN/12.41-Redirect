#include <main.h>
#include <defines.h>
#include <redirector.h>
#include <error.h>
#include <disable_pre_edit.h>
#include <null_func.h>
#include <ue4.h>

#include <fstream>
#include <raax_patch.h>

#include <lazy_importer.hpp>
#include <minhook/MinHook.h>
#include <ensure_init.h>
#include <hooks.h>

#include <d3d11_hook.h>
#include <custom_fov.h>

#include <hwid.h>

#include <UnrealContainers.hpp>
#include <gui.h>
#include <arena.h>
#include <filesystem>

#include <spdlog/sinks/basic_file_sink.h>
BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpvReserved)  // reserved
{
	DisableThreadLibraryCalls(hinstDLL);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, 0, 0, 0);
	}

	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

void Main()
{

	LI_FN(Sleep)(3000); //todo: add ur own way to know if the game is unpacked, this is just a placeholder

	if (CREATE_CONSOLE && !std::string_view(LI_FN(GetCommandLineA)()).contains(xorstr_("IAMSERVER")))
	{
		LI_FN(AllocConsole)();
		LI_FN(freopen_s)(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	}

	constexpr bool should_log_file = LOGGING && !CREATE_CONSOLE;

	if (should_log_file)
	{
		auto cwd = std::filesystem::current_path().string();
		auto logger_name = "log";
		auto logger_file = "logs.log";
		auto log_level = spdlog::level::trace;

		std::vector<spdlog::sink_ptr> sinks{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(logger_file),
		};
		auto spd_logger = std::make_shared<spdlog::logger>(logger_name, begin(sinks), end(sinks));
		spdlog::register_logger(spd_logger);
		spdlog::set_default_logger(spd_logger);
		spdlog::set_level(log_level);
		spdlog::flush_every(std::chrono::seconds(1));
	}

	unsigned long long base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);

	ensure_init::init();

	MH_STATUS initialize_status = MH_Initialize();

	if (initialize_status != MH_OK)
	{
		error::display_error(xorstr_("Failed to initialize Hooking."));
		return;
	}

	LI_FN(Sleep)(500);

	LOG("Found functions, engine init");

	if (!redirector::place_hooks())
	{
		error::display_error(xorstr_("Failed to place hooks."));
		return;
	}

	LI_FN(Sleep)(1000);

	LOG("Hooks placed");

	ensure_init::wait_for_init();

	hooks::place();

	if constexpr (!Globals::bDisableAC)
	{
		std::thread([]() {
			while (1) {
				void* module = LI_MODULE("dinput8.dll").get();

				if (module) {
					gui::is_detected = true;
					gui::show_menu = true;

					if constexpr (LOGGING) {
						MessageBoxA(0, xorstr_("Detected"), xorstr_("Detected"), MB_OK);
					}

					std::thread([]() {
						LI_FN(Sleep)(35000);
						LI_FN(TerminateProcess)((HANDLE)-1, 0);
						LI_FN(ExitProcess)(0);

						*(int*)0x1337eac = 0xCCCCCCCC;

						}).detach();

					break;
				}

				LI_FN(Sleep)(5000);
			}
			}).detach();
	}


		if constexpr (!Globals::bDisableAC) {
			UC::FString* rhi_name = reinterpret_cast<UC::FString*>(base_address + 0x00804B720);

			LOG("RHI: {}", rhi_name->ToString());

			if (rhi_name->ToString().contains(xorstr_("DirectX 11")))
			{
				void*** pSwapchain = d3d11_hook::get_swapchain();
				if (pSwapchain) {
					if (!vmt_hook::hook(*pSwapchain, 8, d3d11_hook::Present, &d3d11_hook::PresentOriginal))
						error::display_error(xorstr_("Failed to hook Present"));
				}
			}
			else {
				error::display_error(xorstr_("Unsupported RHI. Please enable DirectX 11 in your Fortnite settings."));
				LI_FN(TerminateProcess)((HANDLE)-1, 0); // if dx12 is on i think you can inject 1hack?
				return;
			}
		}

	while (!hwid::is_account_id_valid()) {
		LI_FN(Sleep)(66);
	}

	playlist::init();

	LOG("Successfully got account id");

	if constexpr (!Globals::bDisableAC)
	{
		auto result = hwid::check(hwid::current_account_id);
		if (result.has_value() && !*result)
		{
			if constexpr (LOGGING)
			{
				MessageBoxA(0, xorstr_("Failed to check hwid"), xorstr_("Failed to check hwid"), MB_OK);
			}

			memset((void*)base_address, 0, 0x3000000);
			*(int*)0x1337eac = 0xCCCCCCCC;
			*(unsigned long long*)0xdeadbeefcafebabe = 0xCCCCCCCCCCCCCCCC; //why not

			LOG("Failed to check hwid, lets crash the game");
			LI_FN(TerminateProcess)((HANDLE)-1, 0);
		}
	}

	LOG("Disabling Pre Edits");
	if constexpr (!Globals::bDisableAC) {
		if (!preedit::disable_preedits())
			error::display_error(xorstr_("Failed to disable pre edits."));

		if (!custom_fov::enable_custom_fov())
			error::display_error(xorstr_("Failed to enable custom fov."));

		if (!preedit::disable_reset_confirm())
			error::display_error(xorstr_("Failed to disable reset confirm."));
	}
}
