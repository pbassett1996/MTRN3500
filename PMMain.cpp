#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <tlhelp32.h>
#include "SMObject.h"
#include "SMStructs.h"

#define CRITICAL_MASK 0xFF
#define NONCRITICAL_MASK 0x00
#define NUM_UNITS 7

//Start up sequence
TCHAR* Units[10] = //
{
	TEXT("Laser.exe"),
	TEXT("Plotting.exe"),
	TEXT("XBox.exe"),
	TEXT("OpenGL.exe"),
	TEXT("Simulator.exe"),
	TEXT("UGV.exe"),
	TEXT("GPS.exe"),
};

//Deceleration of IsProcessRunning()
bool IsProcessRunning(const char *processName);

int main() {

	__int64 Frequency, HPCCount;
	double PMTimeStamp;
	int NonCriticalMaskCount = 0;
	int CriticalMaskCount = 0;

	//Module execution based variable declerations
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	SMObject PMObj(TEXT("ProcessManagment"), sizeof(ProcessManagement));
	SMObject GPSObj(TEXT("GPS"), sizeof(GPS));
	SMObject LaserObj(TEXT("Laser"), sizeof(Laser));
	SMObject PlotObj(TEXT("Plotting"), sizeof(Plotting));
	SMObject SimObj(TEXT("Simulator"), sizeof(Simulator));
	SMObject UGVObj(TEXT("UGV"), sizeof(UGV));
	SMObject XBoxObj(TEXT("XBox"), sizeof(Remote));

	//Set up shared memory
	PMObj.SMCreate();
	GPSObj.SMCreate();
	LaserObj.SMCreate();
	PlotObj.SMCreate();
	SimObj.SMCreate();
	UGVObj.SMCreate();
	XBoxObj.SMCreate();

	//Get acces to shared memory2
	PMObj.SMAccess();
	XBoxObj.SMAccess();

	//Start other modules in specified order
	//Run all other processes

	std::cout << "Started: PM.exe" << std::endl;

	for (int i = 0; i < NUM_UNITS; i++) {
		//Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));

			//Start the child processes
			if (!CreateProcess(NULL, //No moudle name (usecommand line)
				Units[i],					//Command Line
				NULL,				//Process handle not inheritable
				NULL,				//Thread handle not inheritable
				FALSE,				//Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,	//No creation flags
				NULL,				//Use Parent's environment block
				NULL,				//Use parent's starting directoruy
				&s[i],				//Pointer to STARTUPINFO structure
				&p[i])
				)				//Pointer to PROCESS_INFORMATION structure
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
				return -1;
			}
		}
		std::cout << "Started: " << Units[i] << std::endl;
		Sleep(1000);

	}


	//Get HPC Frequency
	QueryPerformanceFrequency((LARGE_INTEGER *)&Frequency);

	ProcessManagement *PM = (ProcessManagement*)PMObj.pData;
	Remote *pXBox = (Remote*)XBoxObj.pData;



	while (!PM->Shutdown.Flags.PM)
	{
		PM->HeartBeat.Flags.PM = 1;
		//Get HPC Count
		QueryPerformanceCounter((LARGE_INTEGER*)&HPCCount);
		//Calculate TimeStamp
		PMTimeStamp = (double)HPCCount / (double)Frequency;
		PM->PMTimeStamp = PMTimeStamp;

		//Keep track of live execution of all modules
		//If a module ahs failed
		//Restart if not ciritcal

		if ((PM->HeartBeat.Status & NONCRITICAL_MASK) != NONCRITICAL_MASK)
		{
			NonCriticalMaskCount++;
			if (NonCriticalMaskCount > 100)
			{
				//Intiated Restart

				std::cout << "Initiating Restart" << std::endl;
				NonCriticalMaskCount = 0;
				PM->Shutdown.Status = 0xFF - NONCRITICAL_MASK;
				Sleep(200);

				PM->HeartBeat.Status = 0xFF;
				NonCriticalMaskCount = 0;
			}
		}
		else
		{
			NonCriticalMaskCount = 0;
		}

		//Enter into safe mode if critical failure (could mean shutdown)
		if ((PM->HeartBeat.Status & CRITICAL_MASK) != CRITICAL_MASK)
		{
			CriticalMaskCount++;
			if (CriticalMaskCount > 100)
			{
				PM->Shutdown.Status = 0xFF;
				CriticalMaskCount = 0;
			}
		}
		else {
			CriticalMaskCount = 0;
		}

		if (pXBox->Terminate)
		{
			break;
		}

		//Keep looping
		if (_kbhit())
			break;
		//Process management resetting heartbeats of all other processes
		PM->HeartBeat.Status = 0x00;
	}

	//at termination
	//-shutdown modules in the specified order
	PM->Shutdown.Flags.UGV = 1;
	Sleep(200);
	PM->Shutdown.Flags.Laser = 1;
	Sleep(200);
	PM->Shutdown.Status = 0xFF;
	std::cout << "Terminating normally" << std::endl;

}


bool IsProcessRunning(const char *processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}



