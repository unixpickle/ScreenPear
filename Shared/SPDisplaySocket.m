//
//  UBDisplaySocket.m
//  ScreenPear
//
//  Created by Alex Nichol on 2/5/14.
//
//

#import "SPDisplaySocket.h"

@interface SPDisplaySocket ()

- (id)initWithConnect:(io_connect_t)connect index:(uint32_t)index;

@property (readwrite) uint32_t index;
@property (readwrite) SPUserClientResolution resolution;

@end

@implementation SPDisplaySocket

@synthesize index = _index;
@synthesize resolution = _resolution;

+ (SPDisplaySocket *)openUnusedDisplay:(SPUserClientResolution)resolution {
    io_iterator_t iterator;
    kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                        IOServiceMatching("com_jitsik_ScreenPear_VideoDriver"),
                                                        &iterator);
    if (result != KERN_SUCCESS) return nil;
    
    io_service_t service;
    service = IOIteratorNext(iterator);
    IOObjectRelease(iterator);
    if (service == IO_OBJECT_NULL) return nil;
    
    io_connect_t connect;
    result = IOServiceOpen(service, mach_task_self(), 0, &connect);
    if (result != KERN_SUCCESS) return nil;
    
    // get the mode count
    uint32_t expected = 1;
    uint64_t count;
    IOReturn ret =  IOConnectCallMethod(connect, kSPUserClientCommandGetCount, NULL,
                                        0, NULL, 0, &count, &expected, NULL, NULL);
    if (ret) {
        IOServiceClose(connect);
        return nil;
    }
    
    for (uint64_t index = 0; index < count; index++) {
        uint64_t flag;
        ret =  IOConnectCallMethod(connect, kSPUserClientCommandGetEnabled, &index,
                                   1, NULL, 0, &flag, &expected, NULL, NULL);
        if (ret) {
            IOServiceClose(connect);
            return nil;
        }
        
        // if it's not enabled, enable it with our resolution
        if (!flag) {
            ret = IOConnectCallMethod(connect, kSPUserClientCommandSetMode,
                                      &index, 1, &resolution, sizeof(SPUserClientResolution),
                                      NULL, NULL, NULL, NULL);
            if (ret) {
                continue;
            }
            
            // this is a valid index
            SPDisplaySocket * sock = [[SPDisplaySocket alloc] initWithConnect:connect index:(uint32_t)index];
            sock.resolution = resolution;
            return sock;
        }
    }
    
    IOServiceClose(connect);
    return nil;
}

+ (void)closeAllDisplays {
    io_iterator_t iterator;
    kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                        IOServiceMatching("com_jitsik_ScreenPear_VideoDriver"),
                                                        &iterator);
    if (result != KERN_SUCCESS) return;
    
    io_service_t service;
    service = IOIteratorNext(iterator);
    IOObjectRelease(iterator);
    if (service == IO_OBJECT_NULL) return;
    
    io_connect_t connect;
    result = IOServiceOpen(service, mach_task_self(), 0, &connect);
    if (result != KERN_SUCCESS) return;
    
    // get the mode count
    uint32_t expected = 1;
    uint64_t count;
    IOReturn ret =  IOConnectCallMethod(connect, kSPUserClientCommandGetCount, NULL,
                                        0, NULL, 0, &count, &expected, NULL, NULL);
    if (ret) {
        IOServiceClose(connect);
        return;
    }
    
    for (uint64_t index = 0; index < count; index++) {
        IOConnectCallMethod(connect, kSPUserClientCommandDisable, &index,
                            1, NULL, 0, NULL, NULL, NULL, NULL);
    }
    
    IOServiceClose(connect);
}

- (void)close {
    if (!isOpen) return;
    uint64_t theIndex = self.index;
    IOConnectCallMethod(self.connect, kSPUserClientCommandDisable, &theIndex,
                        1, NULL, 0, NULL, NULL, NULL, NULL);
    IOServiceClose(self.connect);
}

- (void *)framebuffer {
    if (!isOpen) return NULL;
    if (!mappedBuffer) {
        // map the memory
        mach_vm_address_t address = 0;
        mach_vm_size_t vmsize = 0;
        IOConnectMapMemory(self.connect, self.index, mach_task_self(), &address, &vmsize, kIOMapAnywhere);
        mappedBuffer = (void *)address;
    }
    return mappedBuffer;
}

- (void)requestUpdate {
    uint64_t index = self.index;
    IOReturn ret = IOConnectCallMethod(self.connect, kSPUserClientCommandRequestBuffer,
                                       &index, 1, NULL, 0, NULL, NULL, NULL, NULL);
    NSLog(@"got result %u", ret);
}

- (NSImage *)createImage {
    [self requestUpdate];
    void * buffer = [self framebuffer];
    NSData * data = [[NSData alloc] initWithBytes:buffer length:self.resolution.width * self.resolution.height * 3];
	CGDataProviderRef dataprovider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
	CGImageRef image = CGImageCreate(self.resolution.width, self.resolution.height, 8, 24, self.resolution.width * 3, CGColorSpaceCreateDeviceRGB(), 0, dataprovider, NULL, NO, kCGRenderingIntentDefault);
    return [[NSImage alloc] initWithCGImage:image size:NSMakeSize(self.resolution.width, self.resolution.height)];
}

- (void)dealloc {
    [self close];
}

#pragma mark - Private -

- (id)initWithConnect:(io_connect_t)connect index:(uint32_t)index {
    if ((self = [super init])) {
        _connect = connect;
        self.index = index;
        isOpen = YES;
    }
    return self;
}

@end
