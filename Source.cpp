#include <Windows.h>
#include <iostream>
#include "MemMan.h"
#include "csgo.hpp"
#include <thread>

MemMan MemClass; //Для работы с памятью

//Храним процесс, игрока, модуль игры
struct values
{
	DWORD localPlayer;
	DWORD process;
	DWORD gameModule;
	BYTE flag;
} val;


void BunnyHop() {
	//Обновляем адрес игрока
	if (val.localPlayer == NULL)
		while (val.localPlayer == NULL) { //Ищем себя
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


int main()
{
	//Ищем процесс, пока пользователь не запустит
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
	//Заинжектились


	while (true) {
		//Потоки start
		std::thread bhThread(BunnyHop);

		bhThread.join();
		//Потоки end

		val.localPlayer = MemClass.readMem<DWORD>(val.gameModule + hazedumper::signatures::dwLocalPlayer);	//Всегда обновляем, ибо мог сменить тиму/начать новую катку

		//Активация бхопа
		if (GetAsyncKeyState(VK_F11)) {
			cheats.bhop = !cheats.bhop;	
			std::cout << "\nBhop state : " << cheats.bhop;
			Sleep(1000); //Чтобы не флудило активацию 100 раз
		}
	}
}

