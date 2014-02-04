#import "UBVideoDriver.h"

#define super IOService

OSDefineMetaClassAndStructors(UBVideoDriver, super);

bool UBVideoDriver::init(OSDictionary * dictionary) {
    if (!super::init(dictionary)) return false;
    
    OSNumber * number = OSDynamicCast(OSNumber, dictionary->getObject("nubCount"));
    if (!number) return false;
    nubCapacity = (int)number->unsigned32BitValue();
    nubs = OSArray::withCapacity(nubCapacity);
    
    return nubs != NULL;
}

void UBVideoDriver::free() {
    if (nubs) nubs->release();
    super::free();
}

bool UBVideoDriver::start(IOService * provider) {
    if (!super::start(provider)) return false;
    
    setName("JTSK,UBVideoDevice");
    registerService();
    
    // create our nubs
    for (int i = 0; i < nubCapacity; i++) {
        UBVideoNub * nub = OSTypeAlloc(UBVideoNub);
        if (!nub) return false;
        
        if (!nub->init() || !nub->attach(this)) {
            nub->release();
            return false;
        }
        
        char location[5];
        snprintf(location, sizeof(location), "%02X", i);
        nub->setLocation(location);
        nub->registerService();
        nubs->setObject(nub);
        nub->release();
    }
    
    return true;
}

void UBVideoDriver::stop(IOService * provider) {
    super::stop(provider);
    
    // TODO: figure out if these clients are user clients or nubs
    OSIterator * clientIter = getClientIterator();
	IOService * client;
	
	while ((client = (IOService *)clientIter->getNextObject())) {
		client->terminate(kIOServiceRequired | kIOServiceTerminate | kIOServiceSynchronous);
		client->release();
	}
    
	clientIter->release();
}

uint32_t UBVideoDriver::getNubCount() const {
    return (uint32_t)nubs->getCount();
}

UBVideoNub * UBVideoDriver::getNub(uint32_t index) const {
    return OSDynamicCast(UBVideoNub, nubs->getObject((unsigned int)index));
}
