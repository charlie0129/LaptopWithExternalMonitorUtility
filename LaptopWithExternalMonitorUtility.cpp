#include <iostream>
#include <Windows.h>
#include <powrprof.h>

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    int *Count = (int *)dwData;
    (*Count)++;
    return TRUE;
}

int MonitorCount()
{
    int Count = 0;

    if (EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Count))
    {
        return Count;
    }

    return -1;
}

void Usage(void)
{
    fprintf(stderr,
            "Usage .\\LaptopWithExternalMonitorUtility POWER_PLAN_GUID_MUTIPLE_MONITOR POWER_PLAN_GUID_SINGLE_MONITOR\n"
            "-h  Print this help\n"
            "Note: you need to run this utility with elevated privileges.\n"
           );
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Usage();
        return EXIT_FAILURE;
    }

    if (MonitorCount() < 0)
    {
        return EXIT_FAILURE;
    }

    std::string POWER_PLAN_GUID_MUTIPLE_MONITOR{ argv[1] };
    std::string POWER_PLAN_GUID_SINGLE_MONITOR{ argv[2] };

    // ShowWindow(GetConsoleWindow(), SW_HIDE); // hide console window

    int nCurrentMonitors = 0;

    while (true)
    {
        if (MonitorCount() == nCurrentMonitors ? false : nCurrentMonitors = MonitorCount())
        {
            if (nCurrentMonitors >= 2) // mutiple monitors
            {

                std::string cmd{ "power /setactive " };

                if (system((cmd + POWER_PLAN_GUID_MUTIPLE_MONITOR).c_str()) != EXIT_SUCCESS)
                {
                    fputs("Error when setting power plans.\n", stderr);
                    Usage();
                    return EXIT_FAILURE;
                }
                else
                {
                    puts("Activated multi-monitor power plan.\n");
                }

                // puts("mutiple");
            }
            else // single monitor
            {
                std::string cmd{ "power /setactive " };

                if (system((cmd + POWER_PLAN_GUID_SINGLE_MONITOR).c_str()) != EXIT_SUCCESS)
                {
                    fputs("Error when setting power plans.\n", stderr);
                    Usage();
                    return EXIT_FAILURE;
                }
                else
                {
                    puts("Activated single-monitor power plan.\n");
                }

                // puts("single");
            }
        }

        Sleep(1000);
    }

    return EXIT_SUCCESS;
}