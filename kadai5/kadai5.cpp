#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Setting.h"
#define BUFFSIZE 1024
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <vector>

#pragma comment(lib, "winmm.lib")
#define NUMTHREAD 2

#define USEMUTEX

void G_Inimake(void);
typedef struct Greengrocer {
	int Apple;//リンゴの数
}Greengrocer;
typedef struct _customer {
	int Apple;//リンゴの数
	int m_apple;//リンゴの上限
	int wt;//移動時間
	int ut;//荷下ろし
	int buy;//やり取り
}customer;

Greengrocer* Akito = (Greengrocer*)malloc(sizeof(Greengrocer));
customer* Sakura = (customer*)malloc(sizeof(customer));
customer* Tomoyo = (customer*)malloc(sizeof(customer));

HANDLE threadMutex;
int gCounter;
std::vector<int> threadQueue;

void process(customer* a) {
	while(Akito->Apple>0) {
		Sleep(a->wt*1000);//店に行く
#ifdef USEMUTEX
		WaitForSingleObject(threadMutex, INFINITE);
#endif
		Sleep(a->buy * 1000);//八百屋とやり取りする
		if (Akito->Apple > a->m_apple) {
			Akito->Apple = Akito->Apple - a->m_apple;
			a->Apple = a->Apple + a->m_apple;
		}
		else {
			a->Apple = a->Apple + Akito->Apple;
			Akito->Apple = 0;
		}
		printf("残り%d個\n", Akito->Apple);
#ifdef USEMUTEX
		ReleaseMutex(threadMutex);
#endif
		Sleep((a->ut + a->wt) * 1000);//店から帰り荷下ろしする
	}
}

void threadSample(LPVOID data) {
	process((customer*)data);
}


int main(void) {
	char currentDirectory[CHARBUFF];
	getCurrentDirectory(currentDirectory);
	char file[CHARBUFF];
	char settingFile[CHARBUFF];
	sprintf_s(file, "%s", "Greengrocer.ini");
	sprintf_s(settingFile, "%s\\%s", currentDirectory, file);
	G_Inimake();
	Akito->Apple = readInt("Akito","Apple",-1,settingFile);
	Sakura->Apple = readInt("Sakura", "Apple", -1, settingFile);
	Sakura->m_apple = readInt("Sakura", "m_apple", -1, settingFile);
	Sakura->wt = readInt("Sakura", "wt", -1, settingFile);
	Sakura->ut = readInt("Sakura", "ut", -1, settingFile);
	Sakura->buy = readInt("Sakura", "buy", -1, settingFile);
	Tomoyo->Apple = readInt("Tomoyo", "Apple", -1, settingFile);
	Tomoyo->m_apple = readInt("Tomoyo", "m_apple", -1, settingFile);
	Tomoyo->wt = readInt("Tomoyo", "wt", -1, settingFile);
	Tomoyo->ut = readInt("Tomoyo", "ut", -1, settingFile);
	Tomoyo->buy = readInt("Tomoyo", "buy", -1, settingFile);

	fprintf(stdout, "Pass with variable\n");

	int count = 0;
	HANDLE threadHandler[NUMTHREAD];
	DWORD threadID[NUMTHREAD];
	threadMutex = CreateMutex(NULL, FALSE, NULL);//ミューテックス生成
	threadHandler[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadSample, (LPVOID)Sakura, 0, &threadID[0]);
	threadHandler[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadSample, (LPVOID)Tomoyo, 0, &threadID[1]);
	for (int i = 0; i < NUMTHREAD; i++) {
		WaitForSingleObject(threadHandler[i], INFINITE);
		CloseHandle(threadHandler[i]);
	}

	CloseHandle(threadMutex);
	printf("さくら:%d個", Sakura->Apple);
	printf("ともよ:%d個", Tomoyo->Apple);
	sprintf_s(file, "%s", "買い物結果.ini");
	sprintf_s(settingFile, "%s\\%s", currentDirectory, file);
	if (writeInt("Sakura", "Apple", Sakura->Apple, settingFile));
	if (writeInt("Tomoyo", "Apple", Tomoyo->Apple, settingFile));
	return 0;
}
void G_Inimake(void) {
	char currentDirectory[CHARBUFF];
	getCurrentDirectory(currentDirectory);
	char file[CHARBUFF];
	char settingFile[CHARBUFF];
	sprintf_s(file, "%s",  "Greengrocer.ini");
	sprintf_s(settingFile, "%s\\%s", currentDirectory, file);
	if (writeInt("Akito", "Apple",100 , settingFile));
	if (writeInt("Sakura", "Apple", 0, settingFile));
	if (writeInt("Tomoyo", "Apple", 0, settingFile));
	if (writeInt("Sakura", "m_apple", 5, settingFile));
	if (writeInt("Tomoyo", "m_apple", 3, settingFile));
	if (writeInt("Sakura", "wt", 1, settingFile));
	if (writeInt("Tomoyo", "wt", 2, settingFile));
	if (writeInt("Sakura", "ut", 3, settingFile));
	if (writeInt("Tomoyo", "ut", 1, settingFile));
	if (writeInt("Sakura", "buy", 1, settingFile));
	if (writeInt("Tomoyo", "buy", 2, settingFile));
}