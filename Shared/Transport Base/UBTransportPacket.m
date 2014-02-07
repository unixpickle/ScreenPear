//
//  UBTransportPacket.m
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransportPacket.h"

@implementation UBTransportPacket

- (id)initWithData:(NSData *)data type:(UInt32)type {
    if ((self = [super init])) {
        _data = data;
        _type = type;
    }
    return self;
}

@end
