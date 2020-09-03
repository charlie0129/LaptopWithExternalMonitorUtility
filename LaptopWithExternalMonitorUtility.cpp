#include <iostream>
#include <fstream>
#include <Windows.h>
#include <ctime>

enum class POWER_PLAN
{
    UNDEF,
    POWER_PLAN_EXTERNAL_MONITOR,
    POWER_PLAN_INTERNAL_MONITOR
};

void Usage(void)
{
    fprintf(stderr,
            "\nUsage .\\LaptopWithExternalMonitorUtility [-h] | [-l] | [-s DEVICE_ID DISPLAY_ID] |\n"
            "[--start POWER_PLAN_GUID_EXTERNAL_MONITOR POWER_PLAN_GUID_INTERNAL_MONITOR]\n\n"
            "-h: Print this help\n"
            "-l: list all the displays and their IDs\n"
            "-s <DEVICE_ID> <DISPLAY_ID>: select the external display\n"
            "--start <POWER_PLAN_GUID_EXTERNAL_MONITOR> <POWER_PLAN_GUID_INTERNAL_MONITOR>:\n"
            "       start this program\n\n"
            "Note: you need to run this utility with elevated privileges.\n\n"
           );
    return;
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
            std::cout << "Device ID: " << deviceIndex
                      << ", Display ID: " << monitorIndex
                      << ": " << displayDevice.DeviceName
                      << ", " << displayDevice.DeviceString
                      << ", " << displayDevice.DeviceID << "\n";
            ++monitorIndex;
        }

        ++deviceIndex;
    }

    return;
}

int main(int argc, char **argv)
{
    if (!strcmp(argv[1], "-l"))
    {
        ListDisplays();
        return EXIT_SUCCESS;
    }

    if (!strcmp(argv[1], "-h"))
    {
        Usage();
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[1], "-s"))
    {
        if (argc != 6)
        {
            Usage();
            return EXIT_FAILURE;
        }
        else
        {
            const int DEVICE_INDEX = atoi(argv[2]);
            const int EXTERNAL_DISPLAY_INDEX = atoi(argv[3]);
            const std::string POWER_PLAN_GUID_EXTERNAL_MONITOR{ argv[4] };
            const std::string POWER_PLAN_GUID_INTERNAL_MONITOR{ argv[5] };

            DISPLAY_DEVICE externalDisplay;
            externalDisplay.cb = sizeof(externalDisplay);
            EnumDisplayDevices(0, DEVICE_INDEX, &externalDisplay, 0);
            const std::string DEVICE_NAME{ externalDisplay.DeviceName };
            EnumDisplayDevices(DEVICE_NAME.c_str(), EXTERNAL_DISPLAY_INDEX, &externalDisplay, 0);
            const std::string EXTERNAL_DISPLAY_ID{ externalDisplay.DeviceID };

            std::cout << "The external monitor you selected: " << EXTERNAL_DISPLAY_ID << std::endl;
            std::cout << "External monitor power plan: " << POWER_PLAN_GUID_EXTERNAL_MONITOR << std::endl;
            std::cout << "Internal monitor power plan: " << POWER_PLAN_GUID_INTERNAL_MONITOR << std::endl;

            std::ofstream saveFileStream{ "./config.cfg" };

            if (saveFileStream.fail())
            {
                fprintf(stderr, "Error saving configuration.\n");
                saveFileStream.close();
                return EXIT_FAILURE;
            }
            else
            {
                saveFileStream << EXTERNAL_DISPLAY_ID << std::endl
                               << POWER_PLAN_GUID_EXTERNAL_MONITOR << std::endl
                               << POWER_PLAN_GUID_INTERNAL_MONITOR << std::endl;

                if (saveFileStream.fail())
                {
                    fprintf(stderr, "Error saving configuration.\n");
                    saveFileStream.close();
                    return EXIT_FAILURE;
                }
                else
                {
                    puts("Configuration file has been saved.");
                    saveFileStream.close();
                    return EXIT_SUCCESS;
                }
            }
        }

        return EXIT_SUCCESS;
    }

    if (argc != 2)
    {
        Usage();
        return EXIT_FAILURE;
    }

    time_t t;
    tm *local;
    char timebuf[128] = { 0 };
    POWER_PLAN currentPowerPlan = POWER_PLAN::UNDEF;

    std::string POWER_PLAN_GUID_EXTERNAL_MONITOR{};
    std::string POWER_PLAN_GUID_INTERNAL_MONITOR{};
    std::string EXTERNAL_DISPLAY_ID{};

    std::ifstream openFileStream{ "./config.cfg" };

    if (openFileStream.fail())
    {
        fprintf(stderr, "Error opening configuration.\nIf you have not selected a external display, please select it first.\n");
        openFileStream.close();
        return EXIT_FAILURE;
    }
    else
    {
        saveFileStream >> EXTERNAL_DISPLAY_ID
                       >> POWER_PLAN_GUID_EXTERNAL_MONITOR
                       >> POWER_PLAN_GUID_INTERNAL_MONITOR;

        if (openFileStream.fail())
        {
            fprintf(stderr, "Error opening configuration.\n");
            openFileStream.close();
            return EXIT_FAILURE;
        }
        else
        {
            std::cout << "The external monitor you selected: " << EXTERNAL_DISPLAY_ID << std::endl;
            std::cout << "External monitor power plan: " << POWER_PLAN_GUID_EXTERNAL_MONITOR << std::endl;
            std::cout << "Internal monitor power plan: " << POWER_PLAN_GUID_INTERNAL_MONITOR << std::endl;
            openFileStream.close();
        }
    }

    // ShowWindow(GetConsoleWindow(), SW_HIDE); // hide console window

    while (true)
    {
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

        // std::cout << isExternalDisplayConnected << std::endl;
        std::string cmd{ "powercfg /setactive " };

        if (isExternalDisplayConnected)
        {
            if (currentPowerPlan != POWER_PLAN::POWER_PLAN_EXTERNAL_MONITOR)
            {
                t = time(NULL);
                local = localtime(&t);
                strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", local);

                if (system((cmd + POWER_PLAN_GUID_EXTERNAL_MONITOR).c_str()) != EXIT_SUCCESS)
                {
                    fprintf(stderr, "%s: Error when setting power plans.\n", timebuf);
                    Usage();
                    return EXIT_FAILURE;
                }
                else
                {
                    printf("%s: Activated external-monitor power plan.\n", timebuf);
                    currentPowerPlan = POWER_PLAN::POWER_PLAN_EXTERNAL_MONITOR;
                }
            }
        }
        else
        {
            if (currentPowerPlan != POWER_PLAN::POWER_PLAN_INTERNAL_MONITOR)
                if (system((cmd + POWER_PLAN_GUID_INTERNAL_MONITOR).c_str()) != EXIT_SUCCESS)
                {
                    fprintf(stderr, "%s: Error when setting power plans.\n", timebuf);
                    Usage();
                    return EXIT_FAILURE;
                }
                else
                {
                    printf("%s: Activated internal-monitor power plan.\n", timebuf);
                    currentPowerPlan = POWER_PLAN::POWER_PLAN_INTERNAL_MONITOR;
                }
        }

        fflush(stdout);
        Sleep(1000);
    }

    return EXIT_FAILURE;
}