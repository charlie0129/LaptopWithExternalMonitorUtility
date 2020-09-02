#include <iostream>
#include <Windows.h>
#include <ctime>

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
            "Usage .\\LaptopWithExternalMonitorUtility EXTERNAL_DISPLAY_ID POWER_PLAN_GUID_MUTIPLE_MONITOR POWER_PLAN_GUID_SINGLE_MONITOR\n"
            "-h  Print this help\n"
            "-l  list all the displays and their IDs"
            "Note: you need to run this utility with elevated privileges.\n"
           );
}

void ListDisplays()
{
    DISPLAY_DEVICE displayDevice;
    displayDevice.cb = sizeof(displayDevice);
    int deviceIndex = 0;

    while (EnumDisplayDevices(0, deviceIndex, &displayDevice, 0))
    {
        std::string deviceName = displayDevice.DeviceName;
        int monitorIndex = 0;

        while (EnumDisplayDevices(deviceName.c_str(), monitorIndex, &displayDevice, 0))
        {
            std::wcout << "Device ID: " << deviceIndex << ", Display ID: " << monitorIndex << ": "
                       << displayDevice.DeviceName << ", " << displayDevice.DeviceString << ", " << displayDevice.DeviceID << "\n";
            ++monitorIndex;
        }

        ++deviceIndex;
    }
}

int main(int argc, char **argv)
{

    if (!strcmp(argv[1], "-l"))
    {
        puts("Remember the Device ID and Display ID of your external display.");
        ListDisplays();
        return 0;
    }

    if (argc != 5)
    {
        Usage();
        return EXIT_FAILURE;
    }

    if (MonitorCount() < 0)
    {
        return EXIT_FAILURE;
    }

    time_t t;
    tm *local;
    char timebuf[128] = { 0 };

    const int DEVICE_INDEX = atoi(argv[1]);
    const int EXTERNAL_DISPLAY_INDEX = atoi(argv[2]);
    const std::string POWER_PLAN_GUID_MUTIPLE_MONITOR{ argv[3] };
    const std::string POWER_PLAN_GUID_SINGLE_MONITOR{ argv[4] };

    DISPLAY_DEVICE externalDisplay;
    externalDisplay.cb = sizeof(externalDisplay);
    EnumDisplayDevices(0, DEVICE_INDEX, &externalDisplay, 0);
    const std::string DEVICE_NAME{ externalDisplay.DeviceName };
    EnumDisplayDevices(DEVICE_NAME.c_str(), EXTERNAL_DISPLAY_INDEX, &externalDisplay, 0);
    const std::string EXTERNAL_DISPLAY_ID{ externalDisplay.DeviceID };
    std::cout << "The external monitor you selected: " << EXTERNAL_DISPLAY_ID << std::endl;

    // ShowWindow(GetConsoleWindow(), SW_HIDE); // hide console window

    int nCurrentMonitors = 0;

    while (true)
    {
        if (MonitorCount() != nCurrentMonitors)
        {
            nCurrentMonitors = MonitorCount();
            DISPLAY_DEVICE displayDevice;
            displayDevice.cb = sizeof(displayDevice);
            int deviceIndex = 0;
            bool isExternalDisplayConnected = false;

            while (EnumDisplayDevices(0, deviceIndex, &displayDevice, 0))
            {
                std::string deviceName = displayDevice.DeviceName;
                int monitorIndex = 0;

                while (EnumDisplayDevices(deviceName.c_str(), monitorIndex, &displayDevice, 0))
                {
                    if (EXTERNAL_DISPLAY_ID == std::string{ displayDevice.DeviceID })
                    {
                        isExternalDisplayConnected = true;
                    }
                    ++monitorIndex;
                }

                ++deviceIndex;
            }

            std::string cmd{ "powercfg /setactive " };

            if (isExternalDisplayConnected)
            {
                t = time(NULL);
                local = localtime(&t);
                strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", local);

                if (system((cmd + POWER_PLAN_GUID_MUTIPLE_MONITOR).c_str()) != EXIT_SUCCESS)
                {
                    fprintf(stderr, "%s: Error when setting power plans.\n", timebuf);
                    Usage();
                    return EXIT_FAILURE;
                }
                else
                {
                    printf("%s: Activated multi-monitor power plan.\n", timebuf);
                }
            }
            else
            {
                if (system((cmd + POWER_PLAN_GUID_SINGLE_MONITOR).c_str()) != EXIT_SUCCESS)
                {
                    fprintf(stderr, "%s: Error when setting power plans.\n", timebuf);
                    Usage();
                    return EXIT_FAILURE;
                }
                else
                {
                    printf("%s: Activated single-monitor power plan.\n", timebuf);
                }
            }
        }

        fflush(stdout);
        Sleep(1000);
    }

    return EXIT_SUCCESS;
}