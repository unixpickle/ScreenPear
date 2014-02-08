#define UBVideoNub com_jitsik_ScreenPear_VideoNub

#import <IOKit/IOService.h>
#import <IOKit/IOBufferMemoryDescriptor.h>
#import "SPUserClientShared.h"

class UBVideoNub : public IOService {
    OSDeclareDefaultStructors(UBVideoNub);
    
private:
    IOBufferMemoryDescriptor * memory;
    
public:
    virtual bool init(OSDictionary * dict = 0);
    virtual void free();
    
    virtual IOReturn getEnabled(uint64_t * enabled);
    virtual IOReturn getFramebufferMemory(IOMemoryDescriptor ** output);
    virtual IOReturn updateFramebuffer();
    
    virtual IOReturn setMode(SPUserClientResolution resolution);
    virtual IOReturn disable();
};
