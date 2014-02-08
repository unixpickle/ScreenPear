#import "SPFramebuffer.h"

#define super IOFramebuffer

OSDefineMetaClassAndStructors(SPFramebuffer, super);

#define kIOPMIsPowerManagedKey "IOPMIsPowerManaged"

static const char * getSelectorString(IOSelect sel);

enum {
	kUBSleepState = 0,
	kUBDozeState = 1,
	kUBWakeState = 2
};

#pragma mark - Control -

IOReturn SPFramebuffer::enableWithState(IODisplayModeInformation info) {
    IOLockLock(lock);
    if (isEnabled) {
        IOLockUnlock(lock);
        return kIOReturnBusy;
    }
    UInt32 size;
    IOReturn ret;
    if ((ret = calculateFramebufferSize(info, &size))) {
        IOLockUnlock(lock);
        return ret;
    }
    if (size > maximumSize) {
        IOLockUnlock(lock);
        return kIOReturnNoMemory;
    }
    currentMode = info;
    isEnabled = true;
    connectChangeInterrupt(this, NULL);
    IOLockUnlock(lock);
    return kIOReturnSuccess;
}

IOReturn SPFramebuffer::disable() {
    IOLockLock(lock);
    if (!isEnabled) {
        IOLockUnlock(lock);
        return kIOReturnNotOpen;
    }
    isEnabled = false;
    connectChangeInterrupt(this, NULL);
    IOLockUnlock(lock);
    return kIOReturnSuccess;
}

bool SPFramebuffer::getIsEnabled() {
    IOLockLock(lock);
    bool result = isEnabled;
    IOLockUnlock(lock);
    return result;
}

IOBufferMemoryDescriptor * SPFramebuffer::getBuffer() {
    return buffer;
}

IODisplayModeInformation SPFramebuffer::getCurrentMode() {
    return currentMode;
}

#pragma mark - Initialization -

bool SPFramebuffer::init(OSDictionary * dict) {
    if (!super::init(dict)) return false;
    
    lock = IOLockAlloc();
    
    return true;
}

bool SPFramebuffer::start(IOService * provider) {
    if (!super::start(provider)) return false;
    setName("JTSK,SPFramebuffer");
    
    isEnabled = false;
    const char * location = provider->getLocation();
    if (!location) return false;
    setLocation(location);
	
    currentMode = {kDefaultScreenWidth, kDefaultScreenHeight, 60 << 16, 0, kDisplayModeValidFlag};
	buffer = NULL;
    
    IOLog("SPFramebuffer::start - returning\n");
    return true;
}

IOReturn SPFramebuffer::enableController() {
    IOLog("SPFramebuffer::enableController()\n");
    // configure power management
    powerState = kUBWakeState;
    IOPMPowerState powerStates [] = {
		{kIOPMPowerStateVersion1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // kUBSleepState
		{kIOPMPowerStateVersion1, 0, 0, IOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0}, // kUBDozeState
		{kIOPMPowerStateVersion1, IOPMDeviceUsable, IOPMPowerOn, IOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0} // kUBWakeState
    };
    registerPowerDriver(this, powerStates, 3);
    temporaryPowerClampOn();
    changePowerStateTo(kUBWakeState);
	getProvider()->setProperty(kIOPMIsPowerManagedKey, true);
    
    // configure framebuffer
    IOReturn ret;
    if ((ret = calculateFramebufferSize(currentMode, &maximumSize))) return ret;
    
	IOLog("%s: attempting to allocate %d bytes\n", getMetaClass()->getClassName(), maximumSize);
	buffer = IOBufferMemoryDescriptor::withCapacity(maximumSize, kIODirectionInOut);
	if (!buffer) {
		IOLog("%s: error allocating memory: %d bytes\n", getMetaClass()->getClassName(), maximumSize);
        return KERN_FAILURE;
	}
	
	return kIOReturnSuccess;
}

#pragma mark - Deinitialization -

void SPFramebuffer::stop(IOService * provider) {
    IOLog("SPFramebuffer::stop()\n");
    if (buffer) {
		buffer->release();
		buffer = NULL;
	}
	
    isEnabled = false;
    
    super::stop(provider);
}

void SPFramebuffer::free() {
    IOLockFree(lock);
    super::free();
}

#pragma mark - Pixel Formats -

const char * SPFramebuffer::getPixelFormats() {
    return IO32BitDirectPixels "\0\0";
}

IOReturn SPFramebuffer::getInformationForDisplayMode(IODisplayModeID mode, IODisplayModeInformation * info) {
    if (!info) return kIOReturnBadArgument;
    
    memcpy(info, &currentMode, sizeof(IODisplayModeInformation));
    return kIOReturnSuccess;
}

UInt64 SPFramebuffer::getPixelFormatsForDisplayMode(IODisplayModeID mode, IOIndex depth) {
    return 0; // this method is deprecated
    // TODO: see if this can be omitted
}

IOReturn SPFramebuffer::getPixelInformation(IODisplayModeID mode,
                                            IOIndex depth,
                                            IOPixelAperture aperature,
                                            IOPixelInformation * info) {
    if (mode != 1) return kIOReturnBadArgument;
    return calculatePixelInformation(currentMode, info);
}

#pragma mark - Memory -

IODeviceMemory * SPFramebuffer::getVRAMRange() {
    if (!buffer) return NULL;
    // TODO: maybe there's a better way than a cast
    buffer->retain();
    return (IODeviceMemory *)buffer;
}

IODeviceMemory * SPFramebuffer::getApertureRange(IOPixelAperture aperature) {
	if (!buffer) return NULL;
    
    UInt32 length;
    if (calculateFramebufferSize(currentMode, &length)) return NULL;
    
    return IODeviceMemory::withSubRange((IODeviceMemory *)buffer, 0, (IOPhysicalLength)length);
}

#pragma mark - General Attributes -

IOReturn SPFramebuffer::getAttribute(IOSelect sel, uintptr_t * valueOut) {
    IOLog("SPFramebuffer::getAttribute(%s, %p)\n", getSelectorString(sel), valueOut);
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

IOReturn SPFramebuffer::setAttribute(IOSelect sel, uintptr_t value) {
    IOLog("SPFramebuffer::setAttribute(%s, %lu)\n", getSelectorString(sel), value);
    switch (sel) {
		case kIOPowerAttribute:
			handleEvent(value >= kUBWakeState ? kIOFBNotifyWillPowerOn : kIOFBNotifyWillPowerOff);
            powerState = (int)value;
			handleEvent(value >= kUBWakeState ? kIOFBNotifyDidPowerOn : kIOFBNotifyDidPowerOff);
			return kIOReturnSuccess;
		default: break;
	}
    return super::setAttribute(sel, value);
}

#pragma mark - Connections -

IOItemCount SPFramebuffer::getConnectionCount() {
    IOLog("SPFramebuffer::getConnectionCount() - Returning %d connections\n", isEnabled);
    return 1;
}

IOReturn SPFramebuffer::getAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t * valueOut) {
    IOLog("SPFramebuffer::getAttributeForConnection(%d, %s)\n", (int)index, getSelectorString(sel));
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

IOReturn SPFramebuffer::setAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t value) {
    IOLog("SPFramebuffer::setAttributeForConnection(%u, %s, %lu)\n", index, getSelectorString(sel), value);
    if (sel == kConnectionProbe) {
		connectChangeInterrupt(this, 0);
		return kIOReturnSuccess;
	}
    
    switch (sel) {
		case kConnectionPower: return kIOReturnSuccess;
		default: return kIOReturnUnsupported;
	}
}

#pragma mark - Display Modes -

IOItemCount SPFramebuffer::getDisplayModeCount() {
    IOLog("SPFramebuffer::getDisplayModeCount()\n");
    return 1;
}

IOReturn SPFramebuffer::getDisplayModes(IODisplayModeID * modes) {
    IOLog("SPFramebuffer::getDisplayModes()\n");
    modes[0] = 1;
    return kIOReturnSuccess;
}

IOReturn SPFramebuffer::setDisplayMode(IODisplayModeID mode, IOIndex depth) {
    IOLog("SPFramebuffer::setDisplayMode()\n");
    if (mode != 1) return kIOReturnBadArgument;
    if (depth != 0) return kIOReturnBadArgument;
	
	return kIOReturnSuccess;
}

IOReturn SPFramebuffer::getCurrentDisplayMode(IODisplayModeID * modeOut, IOIndex * depthOut) {
    IOLog("SPFramebuffer::getCurrentDisplayMode()\n");
    if (!modeOut || !depthOut) return kIOReturnBadArgument;
	
	*modeOut = 1;
	*depthOut = 0;
	
	return kIOReturnSuccess;
}

#pragma mark - DDC -

bool SPFramebuffer::hasDDCConnect(IOIndex connectIndex) {
    return true;
}

IOReturn SPFramebuffer::getDDCBlock(IOIndex connectIndex, UInt32 blockNumber,
                     IOSelect blockType, IOOptionBits options,
                     UInt8 * data, IOByteCount * length) {
    if (!data) return kIOReturnBadArgument;
	
	EDID edid;
	
	bzero(&edid, sizeof(edid));
	
	edid.descriptorBlock1.blockType = 0xfc; // monitor name
	edid.descriptorBlock1.info[0] = 'n';
	edid.descriptorBlock1.info[1] = 'e';
	edid.descriptorBlock1.info[2] = 'd';
	
	// copy edid out
	memcpy(data, &edid, sizeof(edid));
	*length = sizeof(edid);
	
	return kIOReturnSuccess;
}

#pragma mark - Utility -

IOReturn SPFramebuffer::calculateFramebufferSize(IODisplayModeInformation info, UInt32 * valueOut) {
    if (!valueOut) return kIOReturnBadArgument;
    
    IOPixelInformation pixelInfo;
	IOReturn ret;
    if ((ret = calculatePixelInformation(info, &pixelInfo))) return ret;
    
	*valueOut = (pixelInfo.bytesPerRow * pixelInfo.activeHeight) + 128;
    return kIOReturnSuccess;
}

IOReturn SPFramebuffer::calculatePixelInformation(IODisplayModeInformation info, IOPixelInformation * pixelInfo) {
    if (!pixelInfo) return kIOReturnBadArgument;
    if (info.maxDepthIndex != 0) return kIOReturnBadArgument;
    
    bzero(pixelInfo, sizeof(*pixelInfo));
    
	pixelInfo->activeWidth = info.nominalWidth;
	pixelInfo->activeHeight = info.nominalHeight;
    strncpy(pixelInfo->pixelFormat, IO32BitDirectPixels, sizeof(IOPixelEncoding));
    
	pixelInfo->pixelType = kIORGBDirectPixels;
	pixelInfo->componentMasks[0] = 0xff0000;
    pixelInfo->componentMasks[1] = 0x00ff00;
    pixelInfo->componentMasks[2] = 0x0000ff;
	pixelInfo->bitsPerPixel = 32;
	pixelInfo->componentCount = 3;
    pixelInfo->bitsPerComponent = 8;
	pixelInfo->bytesPerRow = (pixelInfo->activeWidth * 4) + 32;
    
    return kIOReturnSuccess;
}

static const char * getSelectorString(IOSelect sel) {
    static char buff[5];
    memcpy(buff, &sel, 4);
    buff[4] = 0;
    return buff;
}
