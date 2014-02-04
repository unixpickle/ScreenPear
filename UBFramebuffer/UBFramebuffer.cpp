#import "UBFramebuffer.h"

#define super IOFramebuffer

OSDefineMetaClassAndStructors(UBFramebuffer, super);

#define kIOPMIsPowerManagedKey "IOPMIsPowerManaged"

enum {
	kUBSleepState = 0,
	kUBDozeState = 1,
	kUBWakeState = 2
};

static const IODisplayModeInformation gDisplayModes[] = {
	{0, 0, 0, 0, 0}, // padding for 1-start-index
	
    // 16:9, from http://pacoup.com/2011/06/12/list-of-true-169-resolutions/
    {640, 360, 60 << 16, 0, kDisplayModeValidFlag},
    {768, 432, 60 << 16, 0, kDisplayModeValidFlag},
    {896, 504, 60 << 16, 0, kDisplayModeValidFlag},
    {640, 360, 60 << 16, 0, kDisplayModeValidFlag},
    {1024, 576, 60 << 16, 0, kDisplayModeValidFlag},
    {1152, 648, 60 << 16, 0, kDisplayModeValidFlag},
    {1280, 720, 60 << 16, 0, kDisplayModeValidFlag},
    {1408, 792, 60 << 16, 0, kDisplayModeValidFlag},
    {1536, 864, 60 << 16, 0, kDisplayModeValidFlag},
    {1664, 936, 60 << 16, 0, kDisplayModeValidFlag},
    {1792, 1008, 60 << 16, 0, kDisplayModeValidFlag},
    {1920, 1080, 60 << 16, 0, kDisplayModeValidFlag}, // 1080p
    {2560, 1440, 60 << 16, 0, kDisplayModeValidFlag}, // 27" TV
    {3840, 2160, 60 << 16, 0, kDisplayModeValidFlag}, // 4k
    
    // 16:10
    {1280, 800, 60 << 16, 0, kDisplayModeValidFlag},
    {1440, 900, 60 << 16, 0, kDisplayModeValidFlag},
    {1680, 1050, 60 << 16, 0, kDisplayModeValidFlag},
    {1920, 1200, 60 << 16, 0, kDisplayModeValidFlag},
    {2560, 1600, 60 << 16, 0, kDisplayModeValidFlag},
    
    // this is what I'll call the NULL display
	{0, 0, 60 << 16, 0, kDisplayModeValidFlag | kDisplayModeAlwaysShowFlag}
};

#define kDisplayModesCount sizeof(gDisplayModes) / sizeof(IODisplayModeInformation) - 1
#define kLargestDisplayMode 14
#define kStartupDisplayMode 1

#pragma mark - Control -

void UBFramebuffer::setEnabled(bool flag) {
    if (flag == isEnabled) return;
    isEnabled = flag;
    connectChangeInterrupt(this, NULL);
}

bool UBFramebuffer::getEnabled() const {
    return isEnabled;
}

IOBufferMemoryDescriptor * UBFramebuffer::getBuffer() const {
    return buffer;
}

#pragma mark - Initialization -

bool UBFramebuffer::start(IOService * provider) {
    if (!super::start(provider)) return false;
    setName("JTSK,UBFramebuffer");
    
    isEnabled = false;
    const char * location = provider->getLocation();
    if (!location) return false;
    setLocation(location);
	
	currentDisplayMode = kStartupDisplayMode;
	currentDepth = 0;
	buffer = NULL;
    
    IOLog("UBFramebuffer::start - returning\n");
    return true;
}

void UBFramebuffer::stop(IOService * provider) {
    IOLog("UBFramebuffer::stop()\n");
    if (buffer) {
		buffer->release();
		buffer = NULL;
	}
	
    currentDepth = 0;
	currentDisplayMode = 0;
    
    super::stop(provider);
}

IOReturn UBFramebuffer::enableController() {
    IOLog("UBFramebuffer::enableController()\n");
    // configure power management
    powerState = kUBWakeState;
    IOPMPowerState powerStates [] = {
		{kIOPMPowerStateVersion1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // kUBSleepState
		{kIOPMPowerStateVersion1, 0, 0, IOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0}, // kUBDozeState
		{kIOPMPowerStateVersion1, IOPMDeviceUsable, IOPMPowerOn, IOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0} // kUBWakeState
    };
    registerPowerDriver(this, powerStates, 3);
    temporaryPowerClampOn();
    changePowerStateTo(kUBSleepState);
	getProvider()->setProperty(kIOPMIsPowerManagedKey, true);
    
    // configure framebuffer
    UInt32 bufferSize = getApertureSize(kLargestDisplayMode, 0);
	IOLog("%s: attempting to allocate %d bytes\n", getMetaClass()->getClassName(), bufferSize);
	buffer = IOBufferMemoryDescriptor::withOptions(kIODirectionInOut | kIOMemoryKernelUserShared, bufferSize, page_size);
	if (!buffer) {
		IOLog("%s: error allocating memory: %d bytes\n", getMetaClass()->getClassName(), bufferSize);
        return KERN_FAILURE;
	}
	
	return kIOReturnSuccess;
}

#pragma mark - Pixel Formats -

const char * UBFramebuffer::getPixelFormats() {
    IOLog("UBFramebuffer::getPixelFormats()\n");
    return IO8BitIndexedPixels "\0" IO16BitDirectPixels "\0" IO32BitDirectPixels "\0\0";
}

IOReturn UBFramebuffer::getInformationForDisplayMode(IODisplayModeID mode, IODisplayModeInformation * info) {
    IOLog("UBFramebuffer::getInformationForDisplayMode()\n");
    if (!info) return kIOReturnBadArgument;
	bzero(info, sizeof(*info));
	
    IODisplayModeInformation modeInfo = gDisplayModes[mode];
	info->maxDepthIndex	= 0;
	info->nominalWidth = modeInfo.nominalWidth;
	info->nominalHeight	= modeInfo.nominalHeight;
	info->refreshRate = modeInfo.refreshRate;
	info->flags = modeInfo.flags;
	
    return kIOReturnSuccess;
}

UInt64 UBFramebuffer::getPixelFormatsForDisplayMode(IODisplayModeID mode, IOIndex depth) {
    IOLog("UBFramebuffer::getPixelFormatsForDisplayMode()\n");
    return 0; // this method is deprecated
}

IOReturn UBFramebuffer::getPixelInformation(IODisplayModeID mode,
                                            IOIndex depth,
                                            IOPixelAperture aperature,
                                            IOPixelInformation * info) {
    IOLog("UBFramebuffer::getPixelInformation()\n");
    if (info == NULL) {
        IOLog("UBFramebuffer::getPixelInformation() - bad argument\n");
        return kIOReturnBadArgument;
    }
	
    bzero(info, sizeof(*info));
    
    IODisplayModeInformation modeInfo = gDisplayModes[mode];
	info->activeWidth = modeInfo.nominalWidth;
	info->activeHeight = modeInfo.nominalHeight;
    IOLog("UBFramebuffer::getPixelInformation() - width: %u, height: %u\n", info->activeWidth, info->activeHeight);
    strncpy(info->pixelFormat, IO32BitDirectPixels, sizeof(IOPixelEncoding));
	info->pixelType = kIORGBDirectPixels;
	info->componentMasks[0] = 0x00FF0000;
	info->componentMasks[1] = 0x0000FF00;
	info->componentMasks[2] = 0x000000FF;
	info->bitsPerPixel = 32;
	info->componentCount = 3;
    info->bitsPerComponent = 8;
	info->bytesPerRow = (info->activeWidth * info->bitsPerPixel / 8) + 32;		// 32 byte row header?
    
    return kIOReturnSuccess;
}

bool UBFramebuffer::isConsoleDevice() {
    IOLog("UBFramebuffer::isConsoleDevice()\n");
    // this will never be true
    return false;
}

#pragma mark - Memory -

IODeviceMemory * UBFramebuffer::getVRAMRange() {
    IOLog("UBFramebuffer::getVRAMRange()\n");
    if (!buffer) return NULL;
    buffer->retain();
    return (IODeviceMemory *)buffer;
}

UInt32 UBFramebuffer::getApertureSize(IODisplayModeID mode, IOIndex depth) {
    IOLog("UBFramebuffer::getApertureSize()\n");
    IOPixelInformation info;
	getPixelInformation(mode, depth, kIOFBSystemAperture, &info);
    
	return (info.bytesPerRow * info.activeHeight) + 128;
}

IODeviceMemory * UBFramebuffer::getApertureRange(IOPixelAperture aperature) {
    IOLog("UBFramebuffer::getApertureRange()\n");
    if (!currentDisplayMode) return NULL;
	if (aperature != kIOFBSystemAperture) return NULL;
	if (!buffer) return NULL;
    
	IODeviceMemory * apertureRange = IODeviceMemory::withRange(buffer->getPhysicalAddress(),
                                                               getApertureSize(currentDisplayMode, currentDepth));
    
	return apertureRange;
}

#pragma mark - General Attributes -

IOReturn UBFramebuffer::getAttribute(IOSelect sel, uintptr_t * valueOut) {
    char selName[5];
    memcpy(selName, &sel, 4);
    selName[4] = 0;
    IOLog("UBFramebuffer::getAttribute()\n");
    
    if (!valueOut) return kIOReturnBadArgument;
	switch (sel) {
		case kIOHardwareCursorAttribute:
			*valueOut = 0;
			return kIOReturnSuccess;
		case kIOPowerAttribute:
		case kIOMirrorAttribute:
		case kIOMirrorDefaultAttribute:
		case kIOCapturedAttribute:
		case kIOCursorControlAttribute:
		case kIOSystemPowerAttribute:
		case kIOVRAMSaveAttribute:
		case kIODeferCLUTSetAttribute:
		default: break;
	}
    return super::getAttribute(sel, valueOut);
}

IOReturn UBFramebuffer::setAttribute(IOSelect sel, uintptr_t value) {
    char selName[5];
    memcpy(selName, &sel, 4);
    selName[4] = 0;
    IOLog("UBFramebuffer::setAttribute(%s, %d)\n", selName, (int)value);
    
    switch (sel) {
		case kIOPowerAttribute:
			handleEvent((value >= kUBWakeState) ? kIOFBNotifyWillPowerOn : kIOFBNotifyWillPowerOff);
            powerState = (int)value;
			handleEvent((value >= kUBWakeState) ? kIOFBNotifyDidPowerOn : kIOFBNotifyDidPowerOff);
			return kIOReturnSuccess;
		default: break;
	}
    return super::setAttribute(sel, value);
}

#pragma mark - Connections -

IOItemCount UBFramebuffer::getConnectionCount() {
    IOLog("UBFramebuffer::getConnectionCount() - Returning %d connections\n", isEnabled);
    return 1;
}

IOReturn UBFramebuffer::getAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t * valueOut) {
    char selName[5];
    memcpy(selName, &sel, 4);
    selName[4] = 0;
    IOLog("UBFramebuffer::getAttributeForConnection(%d, %s)\n", (int)index, selName);
    if (!valueOut) return kIOReturnBadArgument;
	
	switch (sel) {
        case kConnectionCheckEnable:
		case kConnectionEnable:
			*valueOut = isEnabled;
			return kIOReturnSuccess;
		case kConnectionFlags:
		case kConnectionSupportsAppleSense:
		case kConnectionSupportsLLDDCSense:
		case kConnectionPower:
		case kConnectionPostWake:
		case kConnectionDisplayParameterCount:
		case kConnectionDisplayParameters:
		default: break;
	}
    return kIOReturnUnsupported;
}

IOReturn UBFramebuffer::setAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t value) {
    if (sel == kConnectionProbe) {
		IOLog("UBFramebuffer::setAttributeForConnection() - sense!\n");
		connectChangeInterrupt(this, 0);
		return kIOReturnSuccess;
	}
    
    char selName[5];
    memcpy(selName, &sel, 4);
    selName[4] = 0;
    IOLog("UBFramebuffer::setAttributeForConnection(%d, %s, %d)\n", (int)index, selName, (int)value);
    
    switch (sel) {
		case kConnectionPower: return kIOReturnSuccess;
		default: return kIOReturnUnsupported;
	}
}

#pragma mark - Display Modes -

IOItemCount UBFramebuffer::getDisplayModeCount() {
    IOLog("UBFramebuffer::getDisplayModeCount()\n");
    // TODO: see if we can change this mid-life
    return kDisplayModesCount;
}

IOReturn UBFramebuffer::getDisplayModes(IODisplayModeID * modes) {
    IOLog("UBFramebuffer::getDisplayModes()\n");
    for (int i = 0; i < kDisplayModesCount; i++) {
        modes[i] = i + 1;
    }
    return kIOReturnSuccess;
}

IOReturn UBFramebuffer::setDisplayMode(IODisplayModeID mode, IOIndex depth) {
    IOLog("UBFramebuffer::setDisplayMode()\n");
    if (mode > kDisplayModesCount || mode < 1) return kIOReturnBadArgument;
    if (depth != 0) return kIOReturnBadArgument;
    
    currentDisplayMode = mode;
	currentDepth = depth;
	
	return kIOReturnSuccess;
}

IOReturn UBFramebuffer::getCurrentDisplayMode(IODisplayModeID * modeOut, IOIndex * depthOut) {
    IOLog("UBFramebuffer::getCurrentDisplayMode()\n");
    if (!modeOut || !depthOut) return kIOReturnBadArgument;
	
	*modeOut = currentDisplayMode;
	*depthOut = currentDepth;
	
	return kIOReturnSuccess;
}
