# LaptopWithExternalMonitorUtility
- Automatically switches power plans when an external monitor is connected on Microsoft Windows.
    - For example, you can disable lid-sleep when an external display is connected and re-enable lid-sleep when the external display is disconnected.
## Usage:
1. Check the corresponding GUID to your power plans
    - Enter `powercfg /list` in Command Prompt. You should see results like:
    ```
    Existing Power Schemes (* Active)
    -----------------------------------
    Power Scheme GUID: 381b4222-f694-41f0-9685-ff5bb260df2e  (Balanced) *
    Power Scheme GUID: eb40421b-7b88-4ed1-8aa1-2f71d8b6a801  (Parallels)
    ```
    - Write down the GUIDs to the power plans that you want to switch to and back.
2. Compile the `C++` code using a proper compiler.
3. List all your monitors using `.\LaptopWithExternalMonitorUtility.exe -l`.
    - You should see outputs like 
    ```
    Device ID: 0, Display ID: 0: \\.\DISPLAY1\Monitor0, Generic PnP Monitor, MONITOR\BNQ7950\{4d36e96e-e325-11ce-bfc1-08002be10318}\0004
    Device ID: 1, Display ID: 0: \\.\DISPLAY2\Monitor0, Generic PnP Monitor, MONITOR\APPA040\{4d36e96e-e325-11ce-bfc1-08002be10318}\0013
    ```
    - Write down the Device ID and Display ID of your external monitor.
4. Select the external monitor and power plans using what you have just written down. `.\LaptopWithExternalMonitorUtility.exe -s <DEVICE_ID> <DISPLAY_ID>  <POWER_PLAN_GUID_EXTERNAL_MONITOR> <POWER_PLAN_GUID_INTERNAL_MONITOR>`
    - For example: `.\LaptopWithExternalMonitorUtility.exe -s 1 0 381b4222-f694-41f0-9685-ff5bb260df2e eb40421b-7b88-4ed1-8aa1-2f71d8b6a801`
4. Start the program. `.\LaptopWithExternalMonitorUtility.exe`
    - The program will not exit by itself. When external monitors are connected, power plan `381b4222-f694-41f0-9685-ff5bb260df2e` will be activated. When it is disconnected, power plan `eb40421b-7b88-4ed1-8aa1-2f71d8b6a801` will be activated.
    - Alternatively, you can also use `Windows Task Scheduler` to automate this task.
