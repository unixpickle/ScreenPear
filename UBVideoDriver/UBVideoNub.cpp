#import "UBVideoNub.h"
#import "UBFramebuffer.h"

#define super IOService

OSDefineMetaClassAndStructors(UBVideoNub, super);

#define GETFB UBFramebuffer * fb = OSDynamicCast(UBFramebuffer, getClient());\
              if (!fb) return kIOReturnNotAttached

bool UBVideoNub::init(OSDictionary * dict) {
    if (!super::init(dict)) return false;
    setProperty("compatible", "UBVideoNub");
    
    uint64_t size = kDefaultScreenHeight * kDefaultScreenWidth * 3;
    memory = IOBufferMemoryDescriptor::withCapacity(size, kIODirectionInOut);
    
    return true;
}

void UBVideoNub::free() {
    memory->release();
    super::free();
}

#pragma mark - Getters -

IOReturn UBVideoNub::getEnabled(uint64_t * enabled) {
    if (!enabled) return kIOReturnBadArgument;
    GETFB;
    
    *enabled = (uint64_t)fb->getIsEnabled();
    
    return kIOReturnSuccess;
}

IOReturn UBVideoNub::getFramebufferMemory(IOMemoryDescriptor ** output) {
    if (!output) return kIOReturnBadArgument;
    output[0] = memory;
    memory->retain();
    return kIOReturnSuccess;
}

IOReturn UBVideoNub::updateFramebuffer() {
    GETFB;
    if (!fb->getIsEnabled()) return kIOReturnNotOpen;
    
    // get our buffers
    IODeviceMemory * vram = fb->getVRAMRange();
    IOMemoryMap * sourceMap = vram->map(kIOMapAnywhere);
    IOMemoryMap * destinationMap = memory->map(kIOMapAnywhere);
    
    // datatypes for our buffers
    UInt32 * words = (UInt32 *)sourceMap->getVirtualAddress();
    UInt8 * pixelDest = (unsigned char *)destinationMap->getVirtualAddress();
    
    IODisplayModeInformation info = fb->getCurrentMode();
    for (UInt32 y = 0; y < info.nominalHeight; y++) {
        for (UInt32 x = 0; x < info.nominalWidth; x++) {
            // copy the RGB components
            UInt32 word = *(words++);
            *(pixelDest++) = (word & 0xFF);
            *(pixelDest++) = (word & 0xFF00) >> 8;
            *(pixelDest++) = (word & 0xFF0000) >> 16;
        }
        
        // skip forward 32 bytes
        words += 8;
    }

    sourceMap->release();
    destinationMap->release();
    vram->release();
    return kIOReturnSuccess;
}

#pragma mark - Setters -

IOReturn UBVideoNub::setMode(UBUserClientResolution resolution) {
    GETFB;
    
    uint64_t area = (uint64_t)resolution.width * (uint64_t)resolution.height;
    if (area > kDefaultScreenWidth * kDefaultScreenHeight) {
        return kIOReturnNoSpace;
    }
    
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

