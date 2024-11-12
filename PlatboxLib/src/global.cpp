#include "global.h"
#include "msr.h"

HANDLE g_hDevice = 0;
int DEBUG_MODE = 0;

void debug_print(const char *fmt, ...) {
    if (DEBUG_MODE) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
}

int _prev_debug_mode = 0;
void enable_debug_mode() {
    _prev_debug_mode = DEBUG_MODE;
    DEBUG_MODE = 1;
}

void restore_debug_mode() {
    DEBUG_MODE = _prev_debug_mode;
}

char _aux_print_buff[2048];

void print_green(const char *fmt, ...) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else

        memset(_aux_print_buff, 0x00, sizeof(_aux_print_buff));
        snprintf(_aux_print_buff, sizeof(_aux_print_buff), "\x1b[32m%s\x1b[0m", fmt);        

        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, _aux_print_buff, ap);
        va_end(ap);

    #endif
}

void print_red(const char *fmt, ...) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else

        memset(_aux_print_buff, 0x00, sizeof(_aux_print_buff));
        snprintf(_aux_print_buff, sizeof(_aux_print_buff), "\x1b[31m%s\x1b[0m", fmt);

        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, _aux_print_buff, ap);
        va_end(ap);

    #endif
}



#pragma comment(lib, "DeltaFuzzLib.lib")

FUZZER_OBJECT *g_fuzzer;
PCI_ECAM g_pci_ecam;




void InitializeDeltaFuzz() {
	CreateFuzzerObject(&g_fuzzer, 0, TRUE, FALSE);
}


void open_platbox_device() {
	#ifdef __linux__ 
		char device_name[256];  
		memset(device_name, 0x00, sizeof(device_name));
  		sprintf(device_name, "/dev/%s0", KERNETIX_DEVICE_NAME);
		int fd = open(device_name, O_RDWR);
		if (fd < 0) 
		{
			printf("Can't open device file: %s\n", device_name);
			exit(-1);
		}
		printf("%s opened successfully: %d\n", device_name, fd);
		g_hDevice = fd;
		
	#else // _WIN32
		HANDLE h = CreateFileW(
			L"\\\\.\\PlatboxDev",
			FILE_READ_ACCESS | FILE_WRITE_ACCESS,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (h == NULL || (int)h == -1) {
			printf("Error: %08x\n", GetLastError());
			printf("Check Platbox driver is loaded!\n");
			exit(-1);
		}

		g_hDevice = h;

	#endif	
	
}

void close_platbox_device() {
	#ifdef __linux__ 
		close(g_hDevice);
	#else // _WIN32
		CloseHandle(g_hDevice);
	#endif	
}



void start_service() {
	#ifdef __linux__ 
		printf("start_service Unimplemented!");
		exit(-1);

	#else // _WIN32
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}

		SC_HANDLE hSvc = OpenServiceA(hSCM, SVC_NAME, SERVICE_START | DELETE | SERVICE_STOP);

		if (hSvc == NULL) {
			printf("-> could not open service %s - error: %08x\n", SVC_NAME, GetLastError());
		}
		else {
			get_more_privileges();
			
			if (StartServiceA(hSvc, 0, NULL)) {
				printf("-> service started successfully!\n");
			}
			else {
				DWORD error = GetLastError();
				if (error == ERROR_SERVICE_ALREADY_RUNNING) {
					printf("-> service has already been started!\n");
				}
				else {
					printf("-> could not start service %s - error: %08x\n", SVC_NAME, error);
				}
			}

			CloseServiceHandle(hSCM);
		}

		CloseServiceHandle(hSCM);
	#endif	
}	

char service_exe[512];
void install_driver(char *driverNameArg) {
	memset(service_exe, 0x00, sizeof(service_exe));

	#ifdef __linux__ 		
		system("insmod kernetix_km.ko");
		//exit(-1);

	#else // _WIN32

		char driverName[128] = "\\Platbox.sys";

		char *pServiceExe = &service_exe[0];

		if (driverNameArg != NULL) {
			pServiceExe = driverNameArg;
		} else {
			GetCurrentDirectoryA(sizeof(service_exe), pServiceExe);
			strcat(pServiceExe, driverName);
		}

		get_more_privileges();

		printf("-> About to install the service: %s\n", pServiceExe);


		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}


		SC_HANDLE hSvc = CreateServiceA(
			hSCM,
			SVC_NAME,
			SVC_NAME,
			SERVICE_START | DELETE | SERVICE_STOP,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE,
			pServiceExe,            
			NULL,                  
			NULL,                  
			NULL,                  
			NULL,                  
			NULL                  
		);


		if (hSvc == NULL) {
			DWORD error_code = GetLastError();
			if (error_code == ERROR_SERVICE_EXISTS) {
				start_service();
			}
			else {
				printf("-> error creating the service - error: %08x\n", error_code);
			}
		}
		else {
			SERVICE_STATUS status;
			if (QueryServiceStatus(hSvc, &status)) {
				if (status.dwCurrentState == SERVICE_RUNNING) {
					printf("-> %s service created and started successfully!\n", SVC_NAME);
				}			
			}
			CloseServiceHandle(hSvc);
		}

		CloseServiceHandle(hSCM);


	#endif
}

void stop_service() {
	#ifdef __linux__ 
		printf("stop_service Unimplemented!");
		exit(-1);

	#else // _WIN32
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}

		SC_HANDLE hSvc = OpenServiceA(hSCM, SVC_NAME, SERVICE_ALL_ACCESS);
		SERVICE_STATUS status;
		if (ControlService(hSvc, SERVICE_CONTROL_STOP, &status)) {
			printf("-> service stopped successfully!\n");
		}
		else {
			printf("-> error stopping service: %08x\n", GetLastError());
		}

		CloseServiceHandle(hSvc);
		CloseServiceHandle(hSCM);

	#endif
}

void remove_driver() {
	#ifdef __linux__ 
		system("rmmod kernetix_km");

	#else // _WIN32
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!hSCM) {
			printf("-> OpenSCManager failed! error: %08x\n", GetLastError());
			return;
		}

		SC_HANDLE hSvc = OpenServiceA(hSCM, SVC_NAME, SERVICE_START | DELETE | SERVICE_STOP);

		if (hSvc == NULL) {
			printf("-> service can't be opened - error: %08x\n", GetLastError());
		}		
		else {
			if (DeleteService(hSvc)) {
				printf("-> %s service was removed successfully\n", SVC_NAME);
			}
			else {
				printf("-> service can't be removed - error: %08x\n", GetLastError());
			}
			CloseServiceHandle(hSvc);
		}
		CloseServiceHandle(hSCM);

	#endif
}


void init_os_specifics() {
	#ifdef _WIN32 
		/* Set white foregorund*/
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		resolve_nt_functions();
		find_pml4_idx();
	#endif
}

void doSleep(UINT64 milliseconds) {
	#ifdef _WIN32
		Sleep(milliseconds);
	#else
		usleep(milliseconds * 1000); // convert to ms to us
	#endif
}