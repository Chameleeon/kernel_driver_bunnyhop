#pragma once
#include <stdio.h>
#include <Windows.h>
#include <iostream>

#include <vector>
#include<thread>
#include "../Math/vector.h"
#include "../client_dll.hpp"
#include "../offsets.hpp"
#include "driver.h"

namespace offset {
	constexpr std::ptrdiff_t dwEntityList = client_dll::dwEntityList;
	constexpr std::ptrdiff_t dwViewMatrix = client_dll::dwViewMatrix;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = client_dll::dwLocalPlayerPawn;
	constexpr std::ptrdiff_t m_hPawn = client_dll::CBasePlayerController::m_hPawn;
	constexpr std::ptrdiff_t m_iHealth = client_dll::C_BaseEntity::m_iHealth;
	constexpr std::ptrdiff_t m_iTeamNum = client_dll::C_BaseEntity::m_iTeamNum;
	constexpr std::ptrdiff_t m_vOldOrigin = client_dll::C_BasePlayerPawn::m_vOldOrigin;
	constexpr std::ptrdiff_t m_entitySpottedState = client_dll::C_CSPlayerPawn::m_entitySpottedState;
	constexpr std::ptrdiff_t m_hPlayerPawn = client_dll::CCSPlayerController::m_hPlayerPawn;
	constexpr std::ptrdiff_t m_iszPlayerName = client_dll::CBasePlayerController::m_iszPlayerName;
}


class Entity
{
public:
	std::uintptr_t pawAddress;
	std::uintptr_t controllerAddress;
	Vector origin;
	Vector view;
	int health;
	uint32_t lifeState;
	float distance;

};

class C_CSPlayerPawn {
public:
	int health, team;
	
	Vector Position;

	uintptr_t pCSPlayerPawn;
};

inline C_CSPlayerPawn CCSPlayerPawn;

class Reader {
public:
	uintptr_t client = 0;

	std::vector<C_CSPlayerPawn> playerList;
	void ThreadLoop();
	template<typename T>
	T read_memory(uintptr_t address);

	int selectedIndex;

private:
	void FilterPlayers();
	HANDLE driver;
};

inline Reader reader;

class IllegalAimer {
public:
	void doAiming();
private:
	Vector findClosest(const std::vector<Vector> playerPositions);
	void MoveMouseToPlayer(Vector position);
	bool toggleEnabled = false;
	bool wasKeyDown = false;
};

inline IllegalAimer aimer;

template<typename T>
T Reader::read_memory(uintptr_t address) {
	static_assert(std::is_trivially_copyable_v<T>, "read_memory requires trivially copyable types");

	T item = driver_ns::read_memory<T>(this->driver, address);
	return item;
}