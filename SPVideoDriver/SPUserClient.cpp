#import "SPUserClient.h"
#import "SPVideoDriver.h"

#define super IOUserClient

OSDefineMetaClassAndStructors(SPUserClient, super);

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
    {(IOExternalMethodAction)&SPUserClient::gGetCountCall, 0, 0, 1, 0},
    {(IOExternalMethodAction)&SPUserClient::gGetEnabledCall, 1, 0, 1, 0},
    {(IOExternalMethodAction)&SPUserClient::gSetModeCall, 1, sizeof(SPUserClientResolution), 0, 0},
    {(IOExternalMethodAction)&SPUserClient::gDisableCall, 1, 0, 0, 0},
    {(IOExternalMethodAction)&SPUserClient::gRequestBufferCall, 1, 0, 0, 0}
};

static const int kMethodCount = sizeof(gMethods) / sizeof(IOExternalMethodDispatch);

bool SPUserClient::initWithTask(task_t owningTask, void * security_id, UInt32 type) {
    if (!super::initWithTask(owningTask, security_id, type)) return false;
    task = owningTask;
    return true;
}

SPVideoDriver * SPUserClient::getVideoDriver() {
    return OSDynamicCast(SPVideoDriver, getProvider());
}

IOReturn SPUserClient::message(UInt32 type, IOService * provider, void * argument) {
    IOLog("%s::message(%u, %p, %p)\n", getMetaClass()->getClassName(), type, provider, argument);
    return KERN_SUCCESS;
}

IOReturn SPUserClient::clientClose(void) {
    task = NULL;
    return terminate();
}

#pragma mark - Calling Externals -

IOReturn SPUserClient::externalMethod(uint32_t selector,
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

IOReturn SPUserClient::clientMemoryForType(UInt32 type,
                                           IOOptionBits * options,
                                           IOMemoryDescriptor ** memory) {
	SPVideoDriver * driver = getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    if (type >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(type)->getFramebufferMemory(memory);
}

IOReturn SPUserClient::gGetCountCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    SPVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    arguments->scalarOutput[0] = driver->getNubCount();
    return kIOReturnSuccess;
}

IOReturn SPUserClient::gGetEnabledCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    SPVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(index)->getEnabled(arguments->scalarOutput);
}

IOReturn SPUserClient::gSetModeCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    // TODO: somewhere here I should convert the byte order
    SPVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    SPUserClientResolution resolution;
    memcpy(&resolution, arguments->structureInput, sizeof(resolution));
    // TODO: validate the resolution here, somehow
    return driver->getNub(index)->setMode(resolution);
}

IOReturn SPUserClient::gDisableCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    SPVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(index)->disable();
}

IOReturn SPUserClient::gRequestBufferCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments) {
    SPVideoDriver * driver = target->getVideoDriver();
    if (!driver) return kIOReturnNotAttached;
    
    uint32_t index = (uint32_t)arguments->scalarInput[0];
    if (index >= driver->getNubCount()) return kIOReturnBadArgument;
    return driver->getNub(index)->updateFramebuffer();
}
