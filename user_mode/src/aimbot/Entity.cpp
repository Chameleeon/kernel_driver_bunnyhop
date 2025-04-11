#include "Entity.h"
#include "driver.h"

void Reader::ThreadLoop() {
	const DWORD pid = get_process_id(L"cs2.exe");

	if (pid == 0) {
		std::cout << "[-] Failed to find CS2\n";
		std::cin.get();
		return;
	}

	driver = CreateFile(L"\\\\.\\ArchyDriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (driver == INVALID_HANDLE_VALUE) {
		std::cout << "[-] Failed to create driver handle!\n";
		std::cin.get();
		return;
	}

	if (driver_ns::attach_to_process(driver, pid) == true) {
		std::cout << "[+] Attachment successful.\n";

		if (const std::uintptr_t client = get_module_base(pid, L"client.dll"); client != 0) {
			std::cout << "[+] Client dll found.\n";
			this->client = client;

			int loops = 0;

			while (true) {

				std::this_thread::sleep_for(std::chrono::milliseconds(1));

				if (loops > 25) {
					FilterPlayers();

					loops = 0;
				}

				++loops;		
			}
		}
	}

	CloseHandle(driver);

	std::cin.get();
}

void Reader::FilterPlayers() {
	playerList.clear();


	std::uintptr_t entityList = driver_ns::read_memory<std::uintptr_t>(driver, client + offset::dwEntityList);

	if (!entityList) {
		std::cout << "[-] Entity list not found!" << std::endl;
		return;
	}

	auto localPlayerPtr = driver_ns::read_memory<std::uintptr_t>(driver, client + offset::dwLocalPlayerPawn);

	for (int i = 0; i < 64; i++) 
	{
		std::uintptr_t listEntry1 = driver_ns::read_memory<std::uintptr_t>(driver, entityList + (0x8 * ((i & 0x7FFF) >> 9) + 0x10));

		std::uintptr_t playerController = driver_ns::read_memory<std::uintptr_t>(driver, listEntry1 + 0x78 *(i & 0x1FF));

		uint32_t playerPawn = driver_ns::read_memory<int>(driver, playerController + offset::m_hPawn);

		std::uintptr_t listEntry2 = driver_ns::read_memory<std::uintptr_t>(driver, entityList + (0x8 * ((playerPawn & 0x7FFF) >> 9) + 0x10));

		uintptr_t pCSPlayerPawnPtr = driver_ns::read_memory<std::uintptr_t>(driver, listEntry2 + (0x78 * (playerPawn & 0x1FF)));

		std::uint32_t health = driver_ns::read_memory<std::uint32_t>(driver, pCSPlayerPawnPtr + offset::m_iHealth);

		if (health <= 0 || health > 100)
			continue;

		std::uint32_t team = driver_ns::read_memory<std::uint32_t>(driver, pCSPlayerPawnPtr + offset::m_iTeamNum);

		if(team == driver_ns::read_memory<std::uint32_t>(driver, localPlayerPtr + offset::m_iTeamNum))
			continue;

		CCSPlayerPawn.pCSPlayerPawn = pCSPlayerPawnPtr;
		playerList.push_back(CCSPlayerPawn);
	}

}