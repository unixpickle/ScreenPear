#define SPVideoDriver com_jitsik_ScreenPear_VideoDriver

#import <IOKit/IOService.h>
#import "SPVideoNub.h"

class SPVideoDriver : public IOService {
    OSDeclareDefaultStructors(SPVideoDriver);
    
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
