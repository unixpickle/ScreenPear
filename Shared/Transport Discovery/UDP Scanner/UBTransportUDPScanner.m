//
//  UBTransportUDPScanner.m
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransportUDPScanner.h"

@interface UBTransportUDPScanner (Private)

- (void)readPackets:(NSNumber *)theFd;

@end

@implementation UBTransportUDPScanner

- (id)initWithPort:(UInt16)aPort {
    if ((self = [super init])) {
        port = aPort;
        struct sockaddr_in addr;
        fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        int port=6000;
        int broadcast=1;
        
        setsockopt(s, SOL_SOCKET, SO_BROADCAST,
                   &broadcast, sizeof broadcast);
        
        memset(&si_me, 0, sizeof(si_me));
        si_me.sin_family = AF_INET;
        si_me.sin_port = htons(port);
        si_me.sin_addr.s_addr = INADDR_ANY;
        
        assert(::bind(s, (sockaddr *)&si_me, sizeof(sockaddr))!=-1);
        
        while(1)
        {
            char buf[10000];
            unsigned slen=sizeof(sockaddr);
            recvfrom(s, buf, sizeof(buf)-1, 0, (sockaddr *)&si_other, &slen);
            
            printf("recv: %s\n", buf);
        }
    }
    return self;
}

- (void)readPackets:(NSNumber *)theFd {
    
}

@end
