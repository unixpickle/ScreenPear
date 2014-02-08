//
//  UBTransportUDPScanner.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTransportScanner.h"
#include <sys/socket.h>
#include <netinet/in.h>

@interface SPTransportUDPScanner : SPTransportScanner {
    UInt16 port;
    
    int fd, fd6;
    NSThread * thread;
}

- (id)initWithPort:(UInt16)aPort;

@end
