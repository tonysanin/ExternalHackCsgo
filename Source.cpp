#include <Windows.h>
#include <iostream>
#include "MemMan.h"
#include "csgo.hpp"
#include <thread>

MemMan MemClass; //��� ������ � �������

//������ �������, ������, ������ ����
struct values
{
	DWORD localPlayer;
	DWORD process;
	DWORD gameModule;
	BYTE flag;
} val;

//��� ������������
struct software
{
	bool bhop;
	bool glowESP;
	bool noFlash;
	bool radarhack;
} cheats;


void BunnyHop() {
	//��������� ����� ������
	if (val.localPlayer == NULL)
		while (val.localPlayer == NULL) { //���� ����
			val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwLocalPlayer);
		}

	if (cheats.bhop)
	{
		if (GetAsyncKeyState(VK_SPACE)) {
			val.flag = MemClass.readMem<BYTE>(val.localPlayer + hazedumper::netvars::m_fFlags);
			if (val.flag & (1 << 0))
				MemClass.writeMem<DWORD>(val.gameModule + hazedumper::signatures::dwForceJump, 6);
		}
	}
}

void noFlash()
{
	Sleep(1);
	if (!cheats.noFlash)
		return;
	DWORD lp = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwLocalPlayer);
	int flashDuration = MemClass.readMem<int>(lp + hazedumper::netvars::m_flFlashDuration);
	if (flashDuration > 0)
		MemClass.writeMem<int>(lp + hazedumper::netvars::m_flFlashDuration, 0);
}

void radarHack()
{
	if (cheats.radarhack)
	{
		Sleep(20);
		for (int x = 0; x < 32; x++)
		{
			DWORD player = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwEntityList + x * 0x10);
			if (player == 0)
				continue;
			//�������� �� �����������
			if (MemClass.readMem<bool>(player + 0xED))
				continue;
			//�������� �������
			DWORD team = MemClass.readMem<DWORD>(player + hazedumper::netvars::m_iTeamNum);
			if (team != 2 && team != 3)
				continue;
			//������������
			MemClass.writeMem<bool>(player + hazedumper::netvars::m_bSpotted, 1);
		}
	}
}

void GlowESP() {
	Sleep(1);
	if (cheats.glowESP) { //���� �������
		DWORD glowObject = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwGlowObjectManager);
		int myTeam = MemClass.readMem<int>(val.localPlayer + hazedumper::netvars::m_iTeamNum);

		for (short int i = 0; i < 64; i++)
		{
			DWORD entity = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwEntityList + i * 0x10);
			if (entity != NULL)
			{
				int glowIndx = MemClass.readMem<int>(entity + hazedumper::netvars::m_iGlowIndex);
				int entityTeam = MemClass.readMem<int>(entity + hazedumper::netvars::m_iTeamNum);

				if (myTeam == entityTeam) //���� �������
				{
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0x4), 0); //Red
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0x8), 0); //Green
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0xC), 2); //Blue
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0x10), 1);
				}
				else //������
				{
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0x4), 1); //Red
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0x8), 0); //Green
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0xC), 0); //Blue
					MemClass.writeMem<float>(glowObject + ((glowIndx * 0x38) + 0x10), 1);
				}
				MemClass.writeMem<bool>(glowObject + ((glowIndx * 0x38) + 0x24), true);
				MemClass.writeMem<bool>(glowObject + ((glowIndx * 0x38) + 0x25), false);
			}
		}
	}
}



int main()
{
	//runtime errors
	cheats.bhop = false;
	cheats.glowESP = false;
	cheats.noFlash = false;
	cheats.radarhack = false;
	val.localPlayer = NULL;
	val.flag = NULL;
	val.process = NULL;
	val.gameModule = NULL;
	//����������� ���� �������

	//���� �������, ���� ������������ �� ��������
	do {
		val.process = MemClass.getProcess("csgo.exe");
		val.gameModule = MemClass.getModule(val.process, "client_panorama.dll");
		if (val.process == NULL && val.gameModule == NULL) {
			system("cls");
			Sleep(5000);
			std::cout << "Start csgo. Waiting 5 seconds to retry";
		}
	} while (val.process == NULL && val.gameModule == NULL);
	std::cout << "Injected";
	//�������������


	//���� �� ������ PANIC KEY (TODO)
	while (true) {
		//������ start
		std::thread bhThread(BunnyHop);
		std::thread espThread(GlowESP);
		std::thread noFlashThread(noFlash);
		std::thread radarThread(radarHack);

		bhThread.join();
		espThread.join();
		noFlashThread.join();
		radarThread.join();
		//������ end

		val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwLocalPlayer);	//������ ���������, ��� ��� ������� ����/������ ����� �����

		//��������� �����
		if (GetAsyncKeyState(VK_F11)) {
			cheats.bhop = !cheats.bhop;
			std::cout << "\nBhop state : " << cheats.bhop;
			Sleep(1000); //����� �� ������� ��������� 100 ���
		}

		//��������� glow
		if (GetAsyncKeyState(VK_F12)) {
			cheats.glowESP = !cheats.glowESP;
			std::cout << "\nGlowESP state : " << cheats.glowESP;
			Sleep(1000); //Anti flood
		}

		//��������� noflash
		if (GetAsyncKeyState(VK_F9)) {
			cheats.noFlash = !cheats.noFlash;
			std::cout << "\nNo flash state : " << cheats.noFlash;
			Sleep(1000);
		}

		//��������� ������
		if (GetAsyncKeyState(VK_F8)) {
			cheats.radarhack = !cheats.radarhack;
			std::cout << "\nRadar Hack state : " << cheats.radarhack;
			Sleep(1000);
		}
	}
}

