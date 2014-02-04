#define UBUserClient com_jitsik_Umbilical_UserClient

#include <IOKit/IOUserClient.h>
#include <IOKit/IOLib.h>
#import "UBUserClientShared.h"
#import "UBVideoDriver.h"

class UBUserClient : public IOUserClient {
    OSDeclareDefaultStructors(UBUserClient);
    
protected:
    task_t task;
    
public:
    virtual bool initWithTask(task_t owningTask, void * security_id, UInt32 type);
    virtual IOReturn open();
    virtual IOReturn close();
    virtual void stop(IOService * provider);
    virtual bool start(IOService * provider);
    virtual UBVideoDriver * getVideoDriver();
    
    virtual IOReturn message(UInt32 type, IOService * provider, void * argument = 0);
    virtual IOReturn clientClose(void);
    
    virtual IOReturn externalMethod(uint32_t selector,
                                    IOExternalMethodArguments * arguments,
									IOExternalMethodDispatch * dispatch = 0,
                                    OSObject * target = 0,
                                    void * reference = 0);
    virtual IOReturn clientMemoryForType(UInt32 type, IOOptionBits * options, IOMemoryDescriptor ** memory);
    
    static IOReturn gOpenCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gCloseCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gGetCountCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gGetEnabledCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gSetModeCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments);
    static IOReturn gDisableCall(UBUserClient * target, void * reference, IOExternalMethodArguments * arguments);
};
