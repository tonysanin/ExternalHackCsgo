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


int main()
{
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


	while (true) {
		//������ start
		std::thread bhThread(BunnyHop);

		bhThread.join();
		//������ end

		val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwLocalPlayer);	//������ ���������, ��� ��� ������� ����/������ ����� �����

		//��������� �����
		if (GetAsyncKeyState(VK_F11)) {
			cheats.bhop = !cheats.bhop;	
			std::cout << "\nBhop state : " << cheats.bhop;
			Sleep(1000); //����� �� ������� ��������� 100 ���
		}
	}
}

