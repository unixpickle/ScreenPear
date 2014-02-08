#ifndef ScreenPear_SPUserClientShared_H
#define ScreenPear_SPUserClientShared_H

typedef enum {
    kSPUserClientCommandGetCount,
    kSPUserClientCommandGetEnabled,
    kSPUserClientCommandSetMode,
    kSPUserClientCommandDisable,
    kSPUserClientCommandRequestBuffer
} SPUserClientCommands;

typedef struct {
    UInt32 width, height;
    UInt16 imageWidth, imageHeight;
    UInt32 reserved;
} __attribute__((packed)) SPUserClientResolution;

#define kDefaultScreenWidth 3840
#define kDefaultScreenHeight 2160

#endif
