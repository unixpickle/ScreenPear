#import "SPVideoDriver.h"

#define super IOService

OSDefineMetaClassAndStructors(SPVideoDriver, super);

bool SPVideoDriver::init(OSDictionary * dictionary) {
    if (!super::init(dictionary)) return false;
    
    OSNumber * number = OSDynamicCast(OSNumber, dictionary->getObject("nubCount"));
    if (!number) return false;
    nubCapacity = (int)number->unsigned32BitValue();
    nubs = OSArray::withCapacity(nubCapacity);
    
    return nubs != NULL;
}

void SPVideoDriver::free() {
    if (nubs) nubs->release();
    super::free();
}

bool SPVideoDriver::start(IOService * provider) {
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

void SPVideoDriver::stop(IOService * provider) {
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

uint32_t SPVideoDriver::getNubCount() const {
    return (uint32_t)nubs->getCount();
}

UBVideoNub * SPVideoDriver::getNub(uint32_t index) const {
    return OSDynamicCast(UBVideoNub, nubs->getObject((unsigned int)index));
}
