# LaptopWithExternalMonitorUtility
- Automatic switches power plans when an external monitor is connected.
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
2. Compile the `C++` code using whatever compiler you want.
3. Use the GUIDs as the command-line options of the executable file. `.\LaptopWithExternalMonitorUtility POWER_PLAN_GUID_MUTIPLE_MONITOR POWER_PLAN_GUID_SINGLE_MONITOR`
    - For example: `.\LaptopWithExternalMonitorUtility 381b4222-f694-41f0-9685-ff5bb260df2e eb40421b-7b88-4ed1-8aa1-2f71d8b6a801`
    - Alternatively, you can also use `Windows Task Scheduler` to automate this task.