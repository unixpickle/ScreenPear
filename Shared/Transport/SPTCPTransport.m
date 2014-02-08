//
//  UBTCPTransport.m
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTCPTransport.h"

#define kTCPBufferSize 65535

@interface SPTCPTransport (Private)

- (void)readThread;
- (void)writeThread;
- (void)executeWrite:(NSData *)data;

@end

@implementation SPTCPTransport

+ (void)initialize {
    // nobody should handle this, man
    signal(SIGPIPE, SIG_IGN);
}

- (id)initWithFileDescriptor:(int)_fd {
    if ((self = [super init])) {
        fd = _fd;
        
        readThread = [[NSThread alloc] initWithTarget:self selector:@selector(readThread) object:nil];
        writeThread = [[NSThread alloc] initWithTarget:self selector:@selector(writeThread) object:nil];
        [readThread start];
        [writeThread start];
    }
    return self;
}

- (void)shutdown {
    [super shutdown];
    [readThread cancel];
    [writeThread cancel];
    close(fd);
}

- (void)_writeData:(NSData *)data {
    if (!self.open) return;
    [writeRunloop performSelector:@selector(executeWrite:) target:self
                         argument:data order:(ordering++) modes:@[NSRunLoopCommonModes]];
}

- (void)_closed {
    if (!self.open) return;
    [super _closed];
    [writeThread cancel];
    [readThread cancel];
    close(fd);
}

#pragma mark - Private -

- (void)readThread {
    char buffer[kTCPBufferSize];
    
    while (YES) {
        @autoreleasepool {
            ssize_t result = read(fd, buffer, kTCPBufferSize);
            if ([[NSThread currentThread] isCancelled]) return;
            
            // do a read() loop here
            if (result < 0) {
                if (errno == EINTR) {
                    continue;
                }
                [self performSelectorOnMainThread:@selector(_closed) withObject:nil waitUntilDone:NO];
                return;
            }
            
            NSData * data = [[NSData alloc] initWithBytes:buffer length:result];
            [self performSelectorOnMainThread:@selector(_gotData:) withObject:data waitUntilDone:NO];
        }
    }
}

- (void)writeThread {
    @autoreleasepool {
        writeRunloop = [NSRunLoop currentRunLoop];
        [writeRunloop run];
    }
}

- (void)executeWrite:(NSData *)data {
    const char * bytes = (const char *)data.bytes;
    size_t written = 0;
    while (written < data.length) {
        ssize_t result = write(fd, &bytes[written], data.length - written);
        if (result == 0) {
            // write queue is full
            [NSThread sleepForTimeInterval:0.001];
        } else if (result < 0) {
            if (errno == EINTR) continue;
            [self performSelectorOnMainThread:@selector(_closed) withObject:nil waitUntilDone:NO];
            return;
        }
        written += result;
    }
    write(fd, data.bytes, data.length);
}

@end
