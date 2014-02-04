#define UBFramebuffer com_jitsik_Umbilical_Framebuffer

#define IOFRAMEBUFFER_PRIVATE
#import <IOKit/graphics/IOFramebuffer.h>
#undef IOFRAMEBUFFER_PRIVATE

#import <IOKit/IOBufferMemoryDescriptor.h>

class UBFramebuffer : public IOFramebuffer {
    OSDeclareDefaultStructors(UBFramebuffer);
    
private:
    // TODO: use an IOLock here to make sure sync issues don't occur
    
    // state
    bool isEnabled;
    IODisplayModeID currentDisplayMode;
	IOIndex currentDepth;
	UInt32 powerState;
    
    // persistent
    IOBufferMemoryDescriptor * buffer;
    
public:
    // control
    virtual void setEnabled(bool flag);
    virtual bool getEnabled() const;
    virtual IOBufferMemoryDescriptor * getBuffer() const;
    
    // initialization
    virtual bool start(IOService * provider);
    virtual void stop(IOService * provider);
    virtual IOReturn enableController();
    
    // pixel formats
    virtual const char * getPixelFormats();
	virtual IOReturn getInformationForDisplayMode(IODisplayModeID mode, IODisplayModeInformation * info);
	virtual UInt64 getPixelFormatsForDisplayMode(IODisplayModeID mode, IOIndex depth);
	virtual IOReturn getPixelInformation(IODisplayModeID mode, IOIndex depth, IOPixelAperture aperature, IOPixelInformation * info);
    virtual bool isConsoleDevice();
    
    // memory
	virtual IODeviceMemory * getVRAMRange();
    virtual UInt32 getApertureSize(IODisplayModeID mode, IOIndex depth);
    virtual IODeviceMemory * getApertureRange(IOPixelAperture aperature);
	
    // general attributes
	virtual IOReturn getAttribute(IOSelect sel, uintptr_t * valueOut);
	virtual IOReturn setAttribute(IOSelect sel, uintptr_t value);
	
    // connections
	virtual IOItemCount getConnectionCount();
	virtual IOReturn getAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t * valueOut);
	virtual IOReturn setAttributeForConnection(IOIndex index, IOSelect sel, uintptr_t value);
	
    // display modes
	virtual IOItemCount getDisplayModeCount();
	virtual IOReturn getDisplayModes(IODisplayModeID * modes);
	virtual IOReturn setDisplayMode(IODisplayModeID mode, IOIndex depth);
	virtual IOReturn getCurrentDisplayMode(IODisplayModeID * modeOut, IOIndex * depthOut);
};
