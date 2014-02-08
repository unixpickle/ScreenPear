#define SPUserClient com_jitsik_ScreenPear_UserClient

#include <IOKit/IOUserClient.h>
#include <IOKit/IOLib.h>
#import "SPUserClientShared.h"
#import "SPVideoDriver.h"

class SPUserClient : public IOUserClient {
    OSDeclareDefaultStructors(SPUserClient);
    
protected:
    task_t task;
    
public:
    virtual bool initWithTask(task_t owningTask, void * security_id, UInt32 type);
    virtual SPVideoDriver * getVideoDriver();
    
    virtual IOReturn message(UInt32 type, IOService * provider, void * argument = 0);
    virtual IOReturn clientClose(void);
    
    virtual IOReturn externalMethod(uint32_t selector,
                                    IOExternalMethodArguments * arguments,
									IOExternalMethodDispatch * dispatch = 0,
                                    OSObject * target = 0,
                                    void * reference = 0);
    virtual IOReturn clientMemoryForType(UInt32 type, IOOptionBits * options, IOMemoryDescriptor ** memory);
    static IOReturn gCloseCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gGetCountCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gGetEnabledCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gSetModeCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gDisableCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gRequestBufferCall(SPUserClient * target, void * reference, IOExternalMethodArguments * arguments);
};
