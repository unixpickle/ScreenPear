#define UBVideoNub com_jitsik_Umbilical_VideoNub

#import <IOKit/IOService.h>

class UBVideoNub : public IOService {
    OSDeclareDefaultStructors(UBVideoNub);
    
public:
    virtual bool init(OSDictionary * dict = 0);
    
    virtual IOReturn getModeCount(uint64_t * count);
    virtual IOReturn getEnabled(uint64_t * enabled);
    
    virtual IOReturn setModeIndex(uint64_t index);
    virtual IOReturn setEnabled(bool flag);
    
    virtual IOReturn getFramebufferMemory(IOMemoryDescriptor ** output);
};
