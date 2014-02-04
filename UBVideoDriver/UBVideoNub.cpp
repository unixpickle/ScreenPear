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

IOReturn UBVideoNub::getModeCount(uint64_t * count) {
    if (!count) return kIOReturnBadArgument;
    GETFB;
    
    *count = (uint64_t)fb->getDisplayModeCount();
    
    return kIOReturnSuccess;
}

IOReturn UBVideoNub::getEnabled(uint64_t * enabled) {
    if (!enabled) return kIOReturnBadArgument;
    GETFB;
    
    *enabled = (uint64_t)fb->getEnabled();
    
    return kIOReturnSuccess;
}

IOReturn UBVideoNub::setModeIndex(uint64_t index) {
    GETFB;
    
    uint64_t count;
    IOReturn err;
    if ((err = getModeCount(&count)) != kIOReturnSuccess) {
        return err;
    }
    if (index >= count) return kIOReturnBadArgument;
    
    return fb->setDisplayMode((uint32_t)index + 1, 0);
}

IOReturn UBVideoNub::setEnabled(bool flag) {
    GETFB;
    fb->setEnabled(flag);
    return kIOReturnSuccess;
}

IOReturn UBVideoNub::getFramebufferMemory(IOMemoryDescriptor ** output) {
    GETFB;
    if (!output) return kIOReturnBadArgument;
    *output = fb->getBuffer();
    if (!*output) return kIOReturnNotReady;
    return kIOReturnSuccess;
}

