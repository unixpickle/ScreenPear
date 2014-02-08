//
//  UBTransportPacket.m
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTransportPacket.h"

@implementation SPTransportPacket

- (id)initWithData:(NSData *)data type:(UInt32)type {
    if ((self = [super init])) {
        _data = data;
        _type = type;
    }
    return self;
}

@end
