//
//  UBTransportUDPServer.m
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTransportUDPServer.h"

@interface SPTransportServer (Private)

- (void)acceptThreadMethod:(NSNumber *)socket;
- (void)sendBroadcast;
- (void)updatePayload;

@end

@implementation SPTransportUDPServer

+ (NSString *)getDeviceName {
#if TARGET_OS_IPHONE
    return [[UIDevice currentDevice] name];
#else
    return [[NSHost currentHost] localizedName];
#endif
}

+ (NSString *)getDeviceType {
#if TARGET_OS_IPHONE
    // TODO: better detection here
#else
    return @"imac";
#endif
}

+ (NSArray *)addressesForProtocol:(int)protocol temp:(char *)temp tempLen:(int)len {
    int s;
    struct ifconf ifconf;
    struct ifreq ifr[50];
    int ifs;
    int i;
    
    s = socket(protocol, SOCK_STREAM, 0);
    if (s < 0) return nil;
    
    ifconf.ifc_buf = (char *) ifr;
    ifconf.ifc_len = sizeof ifr;
    
    if (ioctl(s, SIOCGIFCONF, &ifconf) < 0) {
        return nil;
    }
    
    ifs = ifconf.ifc_len / sizeof(ifr[0]);
    NSMutableArray * addresses = [NSMutableArray array];
    for (i = 0; i < ifs; i++) {
        struct sockaddr_in * s_in = (struct sockaddr_in *)&ifr[i].ifr_addr;
        if (!inet_ntop(protocol, &s_in->sin_addr, temp, len)) {
            return nil;
        }
        NSString * name = [[NSString alloc] initWithUTF8String:temp];
        [addresses addObject:name];
    }
    
    close(s);
    
    return addresses;
}

- (id)initWithDeviceName:(NSString *)device port:(UInt16)aPort broadcastPort:(UInt16)bcastPort {
    if ((self = [super initWithDeviceName:device])) {
        port = aPort;
        broadcastPort = bcastPort;
    }
    return self;
}

- (void)startServer {
    [super startServer];
    
    // create socket server
    struct sockaddr_in serv_addr;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    bind(serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(serverSocket, 2);
    
    // socket server (ipv6)
    struct sockaddr_in6 serv_addr6;
    struct in6_addr addr6 = IN6ADDR_ANY_INIT;
    serverSocket6 = socket(AF_INET6, SOCK_STREAM, 0);
    bzero(&serv_addr6, sizeof(serv_addr6));
    serv_addr6.sin6_family = AF_INET;
    serv_addr6.sin6_addr = addr6;
    serv_addr6.sin6_port = htons(port);
    bind(serverSocket6, (struct sockaddr *)&serv_addr6, sizeof(serv_addr6));
    listen(serverSocket6, 2);
    
    // create accept threads
    acceptThread = [[NSThread alloc] initWithTarget:self selector:@selector(acceptThreadMethod:)
                                             object:@(serverSocket)];
    [acceptThread start];
    
    accept6Thread = [[NSThread alloc] initWithTarget:self selector:@selector(acceptThreadMethod:)
                                              object:@(serverSocket6)];
    [accept6Thread start];
    
    // create UDP broadcaster
    udpBroadcast = socket(AF_INET, SOCK_DGRAM, 0);
    udpBroadcast6 = socket(AF_INET6, SOCK_DGRAM, 0);
    int broadcastEnable = 1;
    if (setsockopt(udpBroadcast, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        NSLog(@"failed to start broadcasting.");
    }
    if (setsockopt(udpBroadcast6, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        NSLog(@"failed to start broadcasting (6).");
    }
    
    broadcastTimer = [NSTimer scheduledTimerWithTimeInterval:1
                                                      target:self
                                                    selector:@selector(sendBroadcast)
                                                    userInfo:nil
                                                     repeats:YES];
    
    ipsTimer = [NSTimer scheduledTimerWithTimeInterval:10
                                                target:self
                                              selector:@selector(updatePayload)
                                              userInfo:nil
                                               repeats:YES];
    [ipsTimer fire];
}

- (void)stopServer {
    [super stopServer];
    [broadcastTimer invalidate];
    [ipsTimer invalidate];
    [acceptThread cancel];
    [accept6Thread cancel];
    close(serverSocket);
    close(serverSocket6);
    close(udpBroadcast);
    close(udpBroadcast6);
}

#pragma mark - Private -

- (void)acceptThreadMethod:(NSNumber *)fd {
    while (YES) {
        @autoreleasepool {
            int aConn = accept(fd.intValue, NULL, NULL);
            if ([[NSThread currentThread] isCancelled]) return;
            if (aConn < 0) {
                NSError * error = [NSError errorWithDomain:NSPOSIXErrorDomain
                                                      code:errno
                                                  userInfo:nil];
                [self performSelectorOnMainThread:@selector(_handleError:)
                                       withObject:error
                                    waitUntilDone:NO];
                return;
            }
            SPTransport * tp = [[SPTCPTransport alloc] initWithFileDescriptor:aConn];
            [self performSelectorOnMainThread:@selector(_handleClient:)
                                   withObject:tp
                                waitUntilDone:NO];
        }
    }
}

- (void)sendBroadcast {
    struct sockaddr_in s;
    bzero(&s, sizeof(struct sockaddr_in));
    s.sin_family = AF_INET;
    s.sin_port = (in_port_t)htons(broadcastPort);
    s.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    sendto(udpBroadcast, payload.bytes, payload.length, 0, (struct sockaddr *)&s, sizeof(s));
    
    struct sockaddr_in6 s6;
    struct in6_addr addr6 = IN6ADDR_ANY_INIT;
    bzero(&s6, sizeof(struct sockaddr_in6));
    s6.sin6_family = AF_INET6;
    s6.sin6_port = (in_port_t)htons(broadcastPort);
    s6.sin6_addr = addr6;
    sendto(udpBroadcast6, payload.bytes, payload.length, 0, (struct sockaddr *)&s6, sizeof(s6));
}

- (void)updatePayload {
    NSArray * addrs = [[NSHost currentHost] addresses];
    NSArray * blacklist = @[@"::1", @"127.0.0.1"];
    NSMutableData * _payload = [NSMutableData data];
    UInt16 thePort = htons(thePort);
    UInt8 nullByte = 0;
    [_payload appendData:[[self.class getDeviceName] dataUsingEncoding:NSUTF8StringEncoding]];
    [_payload appendBytes:&nullByte length:1];
    [_payload appendData:[[self.class getDeviceType] dataUsingEncoding:NSUTF8StringEncoding]];
    [_payload appendBytes:&nullByte length:1];
    for (NSString * host in addrs) {
        if ([host rangeOfString:@"%"].location != NSNotFound) continue;
        if ([blacklist containsObject:host]) continue;
        [_payload appendBytes:&thePort length:2];
        [_payload appendData:[host dataUsingEncoding:NSUTF8StringEncoding]];
        [_payload appendBytes:&nullByte length:1];
    }
    payload = _payload;
}

@end
