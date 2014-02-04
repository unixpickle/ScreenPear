//
//  UBConnection.m
//  Umbilical
//
//  Created by Alex Nichol on 2/2/14.
//
//

#import "UBConnection.h"

@implementation UBConnection

- (id)init {
    if ((self = [super init])) {
        _connect = IO_OBJECT_NULL;
    }
    return self;
}

- (BOOL)open {
    io_iterator_t iterator;
    kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                        IOServiceMatching("com_jitsik_Umbilical_VideoDriver"),
                                                        &iterator);
    if (result != KERN_SUCCESS) return NO;
    
    io_service_t service;
    while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
        result = IOServiceOpen(service, mach_task_self(), 0, &_connect);
        if (result != KERN_SUCCESS) {
            return NO;
        } else {
            IOObjectRelease(iterator);
            if (IOConnectCallMethod(self.connect, kUBUserClientCommandOpen, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL)) {
                IOServiceClose(self.connect);
                return NO;
            }
            return YES;
        }
    }
    IOObjectRelease(iterator);
    return NO;
}

- (void)close {
    IOConnectCallMethod(self.connect, kUBUserClientCommandClose, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
    IOServiceClose(self.connect);
    _connect = IO_OBJECT_NULL;
}

- (BOOL)getCount:(uint64_t *)count {
    uint32_t expected = 1;
    IOReturn ret =  IOConnectCallMethod(self.connect, kUBUserClientCommandGetCount, NULL,
                                        0, NULL, 0, count, &expected, NULL, NULL);
    return ret == kIOReturnSuccess;
}

- (BOOL)getEnabled:(uint64_t *)flag atIndex:(uint64_t)index {
    uint32_t expected = 1;
    IOReturn ret =  IOConnectCallMethod(self.connect, kUBUserClientCommandGetEnabled, &index,
                                        1, NULL, 0, flag, &expected, NULL, NULL);
    return ret == kIOReturnSuccess;
}

- (BOOL)setMode:(UBUserClientResolution)resolution atIndex:(uint64_t)index {
    IOReturn ret = IOConnectCallMethod(self.connect, kUBUserClientCommandSetMode,
                                       &index, 1, &resolution, sizeof(resolution),
                                       NULL, NULL, NULL, NULL);
    return ret == kIOReturnSuccess;
}

- (BOOL)disableAtIndex:(uint64_t)index {
    IOReturn ret = IOConnectCallMethod(self.connect, kUBUserClientCommandDisable,
                                       &index, 1, NULL, 0, NULL, NULL, NULL, NULL);
    return ret == kIOReturnSuccess;
}

@end
