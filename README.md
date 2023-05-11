# ESP IDF OTA UPDATE

This is OTA Update application with version control.

# Steps to implement this feature

1. Add custom partition table configuration to sdkconfig.defaults
2. Add partition table csv to accommodate the OTA. Example partition table is as follows for 8MB flash

```
# Name,   Type, SubType, Offset,   Size,   Flags
nvs,      data, nvs,     0x9000,   0x6000,
phy_init, data, phy,     0xf000,   0x1000,
factory,  app,  factory, 0x10000,  0x1E0000,
ota_0,    app,  ota_0,   0x200000, 0x1C0000,
ota_1,    app,  ota_1,   0x3C0000, 0x1C0000,
ota_data, data, ota,     0x5C0000, 0x2000,
```

3. Add website certificate downloaded as follows
   - Click on the lock symbol of the website the OTA Firmware is hosted
   - Save the certificate in Base64 - encoded ASCII, single certificate in cert/ directory
4. Add this file path to the CMakeLists.txt in main/ directory.

```
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "../cert/google.crt")
```
