#define SPFramebuffer com_jitsik_ScreenPear_Framebuffer

#define IOFRAMEBUFFER_PRIVATE
#import <IOKit/graphics/IOFramebuffer.h>
#undef IOFRAMEBUFFER_PRIVATE

#import <IOKit/IOBufferMemoryDescriptor.h>
#import "SPUserClientShared.h"
#import "EDID.h"

class SPFramebuffer : public IOFramebuffer {
    OSDeclareDefaultStructors(SPFramebuffer);
    
private:    
    // state
    bool isEnabled;
    IODisplayModeInformation currentMode;
    UInt32 maximumSize;
	UInt32 powerState;
    
    // persistent
    IOBufferMemoryDescriptor * buffer;
    IOLock * lock;
    
    // utility
    static IOReturn calculateFramebufferSize(IODisplayModeInformation info, UInt32 * sizeOut);
    static IOReturn calculatePixelInformation(IODisplayModeInformation info, IOPixelInformation * pixelInfo);
    
public:
    // control
    virtual IOReturn enableWithState(IODisplayModeInformation info);
    virtual IOReturn disable();
    virtual bool getIsEnabled();
    virtual IOBufferMemoryDescriptor * getBuffer();
    virtual IODisplayModeInformation getCurrentMode();
    
    // initialization
    virtual bool init(OSDictionary * dict = 0);
    virtual bool start(IOService * provider);
    virtual IOReturn enableController();
    
    // deinitialization
    virtual void stop(IOService * provider);
    virtual void free();
    
    // pixel formats
    virtual const char * getPixelFormats();
	virtual IOReturn getInformationForDisplayMode(IODisplayModeID mode, IODisplayModeInformation * info);
	virtual UInt64 getPixelFormatsForDisplayMode(IODisplayModeID mode, IOIndex depth);
	virtual IOReturn getPixelInformation(IODisplayModeID mode, IOIndex depth, IOPixelAperture aperature, IOPixelInformation * info);
    
    // memory
	virtual IODeviceMemory * getVRAMRange();
    virtual IODeviceMemory * getApertureRange(IOPixelAperture aperature);
	
    // general attributes
	virtual IOReturn getAttribute(IOSelect sel, uintptr_t * valueOut);
	virtual IOReturn setAttribute(IOSelect sel, uintptr_t value);
	
    // connections
	virtual IOItemCount getConnectionCount();
	virtual IOReturn getAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t * valueOut);
	virtual IOReturn setAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t value);
	
    // display modes, just return the current mode
	virtual IOItemCount getDisplayModeCount();
	virtual IOReturn getDisplayModes(IODisplayModeID * modes);
	virtual IOReturn setDisplayMode(IODisplayModeID mode, IOIndex depth);
	virtual IOReturn getCurrentDisplayMode(IODisplayModeID * modeOut, IOIndex * depthOut);
    
    // fake DDC, as per IOProxyVideoFamily
    virtual bool hasDDCConnect(IOIndex connectIndex);
    virtual IOReturn getDDCBlock(IOIndex connectIndex, UInt32 blockNumber,
                                 IOSelect blockType, IOOptionBits options,
                                 UInt8 * data, IOByteCount * length);
};
