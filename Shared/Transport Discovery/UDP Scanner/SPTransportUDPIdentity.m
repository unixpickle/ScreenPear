//
//  UBTransportUDPIdentity.m
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTransportUDPIdentity.h"

static NSData * readUnitlNULL(NSData * aData, NSData ** remaining);

@implementation SPTransportUDPIdentity

- (id)initWithData:(NSData *)data {
    if ((self = [super init])) {
        NSData * remainingData = data;
        NSData * nameData = readUnitlNULL(data, &remainingData);
        NSData * typeData = readUnitlNULL(remainingData, &remainingData);
        if (!typeData || !nameData) return nil;
        _deviceName = [[NSString alloc] initWithData:nameData encoding:NSUTF8StringEncoding];
        _deviceType = [[NSString alloc] initWithData:typeData encoding:NSUTF8StringEncoding];
        NSMutableDictionary * mAddresses = [NSMutableDictionary dictionary];
        while (remainingData.length) {
            NSData * ipData = readUnitlNULL(remainingData, &remainingData);
            if (!ipData) return nil;
            if (remainingData.length < 2) return nil;
            UInt16 portValue = htons(*((const UInt16 *)remainingData.bytes));
            NSString * addrStr = [[NSString alloc] initWithData:ipData encoding:NSUTF8StringEncoding];
            if (!addrStr) return nil;
            [mAddresses setObject:@(portValue) forKey:addrStr];
        }
        _addressPortPairs = [mAddresses copy];
    }
    return self;
}

- (id)openTransport:(SPTransportConnectCallback)callback {
    return nil;
}

- (void)cancelOpen:(id)job {
    return;
}

- (BOOL)isEqualToIdentity:(id)identity {
    if (![identity isKindOfClass:[SPTransportUDPIdentity class]]) return NO;
    NSDictionary * dict = [(SPTransportUDPIdentity *)identity addressPortPairs];
    return [self.addressPortPairs isEqualToDictionary:dict];
}

@end

static NSData * readUnitlNULL(NSData * aData, NSData ** remaining) {
    if (!aData) return nil;
    NSMutableData * result = [NSMutableData data];
    const char * bytes = (const char *)aData.bytes;
    for (int i = 0; i < aData.length; i++) {
        if (!bytes[i]) {
            *remaining = [aData subdataWithRange:NSMakeRange(i + 1, aData.length - i)];
            return result;
        }
        [result appendBytes:&bytes[i] length:1];
    }
    return nil;
}
