#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#include "aimbot/Entity.h"

int main() {
	std::cout << "[+] Initializing..." << std::endl;

	std::thread ReadThread(&Reader::ThreadLoop, &reader);

	ReadThread.detach();

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::cout << "\n\n[1] Use C to toggle aim locking." << std::endl << std::endl << std::endl;

	while (true)
	{
		aimer.doAiming();
	}


	return 0;
}