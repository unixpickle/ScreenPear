#import "UBVideoNub.h"
#import "UBFramebuffer.h"

#define super IOService

OSDefineMetaClassAndStructors(UBVideoNub, super);

#define GETFB UBFramebuffer * fb = OSDynamicCast(UBFramebuffer, getClient());\
              if (!fb) return kIOReturnNotAttached

bool UBVideoNub::init(OSDictionary * dict) {
    if (!super::init(dict)) return false;
    setProperty("compatible", "UBVideoNub");
    return true;
}

#pragma mark - Getters -

IOReturn UBVideoNub::getEnabled(uint64_t * enabled) {
    if (!enabled) return kIOReturnBadArgument;
    GETFB;
    
    *enabled = (uint64_t)fb->getIsEnabled();
    
    return kIOReturnSuccess;
}

IOReturn UBVideoNub::getFramebufferMemory(IOMemoryDescriptor ** output) {
    GETFB;
    if (!output) return kIOReturnBadArgument;
    output[0] = fb->getBuffer();
    if (!output[0]) return kIOReturnNotReady;
    output[0]->retain();
    return kIOReturnSuccess;
}

#pragma mark - Setters -

IOReturn UBVideoNub::setMode(UBUserClientResolution resolution) {
    GETFB;
    
    IODisplayModeInformation info;
    bzero(&info, sizeof(info));
    info.imageWidth = resolution.imageWidth;
    info.imageHeight = resolution.imageHeight;
    info.nominalWidth = resolution.width;
    info.nominalHeight = resolution.height;
    info.flags = kDisplayModeValidFlag;
    info.refreshRate = 60 << 16;
    return fb->enableWithState(info);
}

IOReturn UBVideoNub::disable() {
    GETFB;
    
    return fb->disable();
}

