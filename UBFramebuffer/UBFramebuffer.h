#define UBFramebuffer com_jitsik_Umbilical_Framebuffer

#define IOFRAMEBUFFER_PRIVATE
#import <IOKit/graphics/IOFramebuffer.h>
#undef IOFRAMEBUFFER_PRIVATE

#import <IOKit/IOBufferMemoryDescriptor.h>

class UBFramebuffer : public IOFramebuffer {
    OSDeclareDefaultStructors(UBFramebuffer);
    
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
};
