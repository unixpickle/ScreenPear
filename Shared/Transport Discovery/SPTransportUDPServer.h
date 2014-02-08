//
//  UBTransportUDPServer.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTransportServer.h"
#import "SPTCPTransport.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

@interface SPTransportUDPServer : SPTransportServer {
    UInt16 port, broadcastPort;
    int serverSocket, serverSocket6;
    int udpBroadcast, udpBroadcast6;
    
    NSTimer * broadcastTimer, * ipsTimer;
    NSThread * acceptThread, * accept6Thread;
    NSData * payload;
}

+ (NSString *)getDeviceName;
+ (NSString *)getDeviceType;

- (id)initWithDeviceName:(NSString *)device port:(UInt16)aPort broadcastPort:(UInt16)bcastPort;

@end
