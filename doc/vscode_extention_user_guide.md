# Environment Prepare
1. Download SDK from Official repository(Github or Gitee).
2. Open SDK with Visual Studio Code.
    - Select [File] - [Open Folder...] and select the tl_ble_sdk you downloaded.3. Install extension.
    - Search "telink" in extensions and install "Telink Development Tool".
4. Open "telink tools" extension you just installed (The icon is a "T").
    - After several seconds, you will see the projects under "PROJECT OUTLINE".
5. Config toolchain.
    - Take TL_BLE_SDK_721X/acl_peripheral_demo as an example. Expand the folder to find this demo in "PROJECT OUTLINE".
    - Move your mouse on the demo, you will see a "rocket" icon on the right side of the demo bar, which shows "Build Target" under your mouse. Click the icon.
    - A popup will come out on the right corner saying "No find toolchain...". 
        - Click [Configure] if you have TelinkIoTStudio installed in the environment and find the <toolchian>/bin/riscv-elf-gcc path. There is tips on the top of folder explorer. In this example we choose "/home/ble/program/TelinkIOT/RDS/V5.3.x/toolchains/nds32le-elf-mculib-v5f/bin/riscv32-elf-gcc".
        - Click [Install] if you have never intalled TelinkIoTStudio in your enviroment. And wait for installing toolchian.
    - Another popup may be "CMake is not available..."
        - Click [Install] to install it and wait for its procedure completion. Usually it takes less than 1 min.
6. Then you can click the "rocket" beside the demo again. And it will start to build and log out in the OUTPUT window in vscode.
