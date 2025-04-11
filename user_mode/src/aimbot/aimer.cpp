#include "Entity.h"

void IllegalAimer::doAiming() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	auto view_matrix = reader.read_memory<view_matrix_t>(reader.client + offset::dwViewMatrix);

	std::vector<Vector> playerPositions;
	playerPositions.clear();

	for (auto& player : reader.playerList)
	{
		Vector playerPosition = reader.read_memory<Vector>(player.pCSPlayerPawn + offset::m_vOldOrigin);

		uintptr_t spottedState = reader.read_memory<uintptr_t>(player.pCSPlayerPawn + offset::m_entitySpottedState);

		if (!reader.read_memory<bool>(spottedState + 0x8))
			continue;

		Vector headPos = { playerPosition.x += 0.0, playerPosition.y += 0.0, playerPosition.z += 65.0f };

		Vector f, h;

		if (Vector::world_to_screen(view_matrix, playerPosition, f) &&
			Vector::world_to_screen(view_matrix, headPos, h))
		{
			playerPositions.push_back(h);
		}
	}

	SHORT keyState = GetAsyncKeyState(0x43); // 'C'
	bool isKeyDown = (keyState & 0x8000);

	if (isKeyDown && !wasKeyDown) {
		toggleEnabled = !toggleEnabled; // Toggle state on key down transition
	}

	wasKeyDown = isKeyDown;

	if (toggleEnabled) {
		auto closest_player = findClosest(playerPositions);
		MoveMouseToPlayer(closest_player);
	}
}

Vector IllegalAimer::findClosest(const std::vector<Vector> playerPositions) {
	if (playerPositions.empty())
	{
		return { 0.0f,0.0f,0.0f };
	}
	
	Vector center_of_screen{ (float)GetSystemMetrics(0) * 0.5f, (float)GetSystemMetrics(1) * 0.5f, 0.0f };

	float lowestDistance = 10000;

	int index = -1;

	for (int i = 0; i < playerPositions.size(); i++) {
		float distance(std::pow(playerPositions[i].x - center_of_screen.x, 2) + std::pow(playerPositions[i].y - center_of_screen.y, 2));

		if (distance < lowestDistance) {
			lowestDistance = distance;
			index = i;
		}
	}
	if (index != -1) {
		return { playerPositions[index].x, playerPositions[index].y, 0.0f };
	}
	else {
		return { 0.0f,0.0f,0.0f };
	}
}

void IllegalAimer::MoveMouseToPlayer(Vector position) {
	if (position.x == 0.0 && position.y == 0.0 && position.z == 0.0)
		return;

	Vector center_of_screen{ (float)GetSystemMetrics(0) * 0.5f, (float)GetSystemMetrics(1) * 0.5f, 0.0f };

	auto new_x = position.x - center_of_screen.x;
	auto new_y = position.y - center_of_screen.y;

	if(reader.selectedIndex>0){
	auto player = reader.playerList[reader.selectedIndex];

	auto movementServices = reader.read_memory<std::uintptr_t>(player.pCSPlayerPawn + 0x11E8);

	auto crouchState = reader.read_memory<bool>(movementServices + 0x1ED);

	auto duckedState = reader.read_memory<bool>(movementServices + 0x1EC);
	if (crouchState || duckedState) {
		mouse_event(MOUSEEVENTF_MOVE, new_x, new_y, 0, 0);
		std::cout << "NEW Y: " << new_y << std::endl;
		return;
	}
	}

	else {
		mouse_event(MOUSEEVENTF_MOVE, new_x, new_y, 0, 0);
	}
}