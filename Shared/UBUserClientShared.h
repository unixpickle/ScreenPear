#ifndef Umbilical_UBUserClientShared_H
#define Umbilical_UBUserClientShared_H

typedef enum {
    kUBUserClientCommandGetCount,
    kUBUserClientCommandGetEnabled,
    kUBUserClientCommandSetMode,
    kUBUserClientCommandDisable
} UBUserClientCommands;

typedef struct {
    UInt32 width, height;
    UInt16 imageWidth, imageHeight;
    UInt32 reserved;
} __attribute__((packed)) UBUserClientResolution;

#endif
