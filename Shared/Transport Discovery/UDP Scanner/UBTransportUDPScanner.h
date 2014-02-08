//
//  UBTransportUDPScanner.h
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransportScanner.h"
#include <sys/socket.h>
#include <netinet/in.h>

@interface UBTransportUDPScanner : UBTransportScanner {
    UInt16 port;
    
    int fd, fd6;
    NSThread * thread;
}

- (id)initWithPort:(UInt16)aPort;

@end
