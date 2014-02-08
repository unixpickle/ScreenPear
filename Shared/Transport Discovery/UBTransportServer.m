//
//  UBTransportServer.m
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransportServer.h"

@implementation UBTransportServer

- (id)initWithDeviceName:(NSString *)device {
    if ((self = [super init])) {
        _deviceName = device;
    }
    return self;
}

- (void)startServer {
    _open = YES;
}

- (void)stopServer {
    _open = NO;
}

- (void)_handleClient:(UBTransport *)transport {
    if (!self.open) return;
    [self.delegate transportServer:self gotTransport:transport];
}

- (void)_handleError:(NSError *)error {
    if (!self.open) return;
    _open = NO;
    [self.delegate transportServer:self failedWithError:error];
}

@end
