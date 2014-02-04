#import "UBUserClient.h"
#import "UBVideoDriver.h"

#define super IOUserClient

OSDefineMetaClassAndStructors(UBUserClient, super);

/*
 struct IOExternalMethodDispatch
 {
 IOExternalMethodAction    function;
 uint32_t                  checkScalarInputCount;
 uint32_t                  checkStructureInputSize;
 uint32_t                  checkScalarOutputCount;
 uint32_t                  checkStructureOutputSize;
 };
 */
static IOExternalMethodDispatch gMethods[] = {
    {(IOExternalMethodAction)&UBUserClient::gOpenCall, 0, 0, 0, 0},
    {(IOExternalMethodAction)&UBUserClient::gCloseCall, 0, 0, 0, 0},
    {(IOExternalMethodAction)&UBUserClient::gGetCountCall, 0, 0, 1, 0},
    {(IOExternalMethodAction)&UBUserClient::gGetEnabledCall, 1, 0, 1, 0},
    {(IOExternalMethodAction)&UBUserClient::gGetModeCountCall, 1, 0, 1, 0},
    {(IOExternalMethodAction)&UBUserClient::gSetModeCall, 3, 0, 0, 0},
    {(IOExternalMethodAction)&UBUserClient::gSetEnabledCall, 2, 0, 0, 0},
};

static const int kMethodCount = sizeof(gMethods) / sizeof(IOExternalMethodDispatch);

bool UBUserClient::initWithTask(task_t owningTask, void * security_id, UInt32 type) {
    if (!super::initWithTask(owningTask, security_id, type)) return false;
    task = owningTask;
    return true;
}

IOReturn UBUserClient::open() {
    if (!getProvider()) return kIOReturnNotAttached;
    if (!getProvider()->open(this)) return kIOReturnExclusiveAccess;
    return kIOReturnSuccess;
}

IOReturn UBUserClient::close() {
    if (!getProvider()) return kIOReturnNotAttached;
    if (getProvider()->isOpen(this)) {
        getProvider()->close(this);
    }
    return kIOReturnSuccess;
}

void UBUserClient::stop(IOService * provider) {
    super::stop(provider);
}

bool UBUserClient::start(IOService * provider) {
    if (!super::start(provider)) return false;
    setName("UBUserClient");
    return true;
}

UBVideoDriver * UBUserClient::getVideoDriver() {
    return OSDynamicCast(UBVideoDriver, getProvider());
}

IOReturn UBUserClient::message(UInt32 type, IOService * provider, void * argument) {
    IOLog("%s::message(%u, %p, %p)\n", getMetaClass()->getClassName(), type, provider, argument);
    return KERN_SUCCESS;
}

IOReturn UBUserClient::clientClose(void) {
    close();
    task = NULL;
    return terminate();
}

#pragma mark - Calling Externals -

IOReturn UBUserClient::externalMethod(uint32_t selector,
                                      IOExternalMethodArguments * arguments,
                                      IOExternalMethodDispatch * dispatch,
                                      OSObject * target,
                                      void * reference) {
    if (selector >= kMethodCount) {
        return super::externalMethod(selector, arguments, dispatch, target, reference);
    }
    
    dispatch = &gMethods[selector];
    if (!target) target = this;
    
	return super::externalMethod(selector, arguments, dispatch, target, reference);
}

IOReturn UBUserClient::clientMemoryForType(UInt32 type,
                                           IOOptionBits * options,
                                           IOMemoryDescriptor ** memory) {
	UBVideoDriver * driver = getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    if (type >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(type)->getFramebufferMemory(memory);
}

IOReturn UBUserClient::gOpenCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->open();
}

IOReturn UBUserClient::gCloseCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    return target->close();
}

IOReturn UBUserClient::gGetCountCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    UBVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    arguments->scalarOutput[0] = driver->getNubCount();
    return kIOReturnSuccess;
}

IOReturn UBUserClient::gGetEnabledCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    UBVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(index)->getEnabled(arguments->scalarOutput);
}

IOReturn UBUserClient::gGetModeCountCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    UBVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(index)->getModeCount(arguments->scalarOutput);
}

IOReturn UBUserClient::gSetModeCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    UBVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    uint64_t mode = arguments->scalarInput[1];
    uint64_t depth = arguments->scalarInput[2];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(index)->setModeIndex(mode, depth);
}

IOReturn UBUserClient::gSetEnabledCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    UBVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    uint64_t flag = arguments->scalarInput[1];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(index)->setEnabled((bool)flag);
}
