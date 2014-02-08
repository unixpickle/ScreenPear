#define UBVideoDriver com_jitsik_Umbilical_VideoDriver

#import <IOKit/IOService.h>
#import "UBVideoNub.h"

class UBVideoDriver : public IOService {
    OSDeclareDefaultStructors(UBVideoDriver);
    
private:
    OSArray * nubs;
    uint32_t nubCapacity;
    
public:
    virtual bool init(OSDictionary * dictionary = 0);
    virtual void free();
    virtual bool start(IOService * provider);
    virtual void stop(IOService * provider);
    
    virtual uint32_t getNubCount() const;
    virtual UBVideoNub * getNub(uint32_t index) const;
};
