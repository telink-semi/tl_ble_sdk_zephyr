# tl_v1.4.0-v4.0.4.8(PR)

### Bug Fixes

- N/A

### BREAKING CHANGES

* N/A

### Features

* This release includes all changes from **tl_ble_sdk V4.0.4.8_Patch_0001**. Please refer to [tl_ble_sdk_Release_Note.md](./tl_ble_sdk_Release_Note.md) for details.
* For the Libs, the naming convention has been optimized to clearly distinguish between different BLE roles (**multirole / central / peripheral**).
* For low-power devices, the **suspend latency compensation logic** has been optimized to ensure stable BLE packet transmission and reception.
    * **Details:** By recording the tick timestamps when entering and exiting sleep, the system configures the chip's wake-up delay to ensure that the chip wakes up in advance, avoiding timing issues that could affect BLE packet transmission and reception.
    * **Fix:** **TL321X/TL322X/TL323X/TL521X/TL721X** can adopt this change to avoid repeated latency measurements caused by differences in CPU frequency or chip characteristics.
    * **Recommendation:** **Recommended to update.**

### Known issues

- For some Central devices that support the **Peripheral-initiated Features Exchange** feature, the Peripheral needs to disable this feature to improve compatibility. This change will be included in the next release.

### Bug Fixes

- N/A

### BREAKING CHANGES

* N/A

### Features

* 该版本包含 **tl_ble_sdk V4.0.4.8_Patch_0001** 全部修改，参考 [tl_ble_sdk_Release_Note.md](./tl_ble_sdk_Release_Note.md)。
* 对于 Libs，优化了命名规则，以区分各种 BLE 角色（multirole/central/peripheral）。
* 对于低功耗设备，优化了 suspend 的延迟补偿逻辑，以保证 BLE 收发包的稳定性。
    * 详细描述：通过记录睡眠进出的 tick 时间点，设置芯片退出睡眠的延迟时间，保证芯片可以提前唤醒，避免影响 BLE 收发包的时序。
    * 修复效果：TL321X/TL322X/TL323X/TL521X/TL721X 可以引用该修改，并避免由于主频/芯片特性导致的重复采集延迟时间的问题。
    * 更新建议：建议更新。

### Known issues

- 对于部分支持 Peripheral-initiated Features Exchange 功能的 Central 设备，Peripheral 需要关闭该功能，以提高兼容性，该修改将在下一版本推送。