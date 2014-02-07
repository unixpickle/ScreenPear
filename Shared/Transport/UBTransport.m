//
//  UBTransport.m
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransport.h"

@interface UBTransport (Private)

- (UBTransportPacket *)_dequeuePacket;

@end

@implementation UBTransport

- (id)init {
    if ((self = [super init])) {
        _open = YES;
        _buffer = [[NSMutableData alloc] init];
    }
    return self;
}

- (void)sendPacket:(UBTransportPacket *)packet {
    UInt64 lenField = packet.data.length;
    lenField = CFSwapInt64HostToBig(lenField);
    UInt32 typeField = CFSwapInt32HostToBig(packet.type);
    [self _writeData:[NSData dataWithBytes:&lenField length:8]];
    [self _writeData:[NSData dataWithBytes:&typeField length:4]];
    [self _writeData:packet.data];
}

- (void)shutdown {
    _open = NO;
}

#pragma mark - Internal -

- (void)_gotData:(NSData *)data {
    if (!self.open) return;
    
    // append to buffer, read packets etc.
    [_buffer appendData:data];
    
    UBTransportPacket * packet;
    while ((packet = [self _dequeuePacket])) {
        [self.delegate transport:self gotPacket:packet];
    }
}

- (void)_closed {
    if (!self.open) return;
    
    _open = NO;
    [self.delegate transportClosed:self];
}

- (void)_writeData:(NSData *)data {
    [self doesNotRecognizeSelector:@selector(_writeData:)];
}

#pragma mark - Private -

- (UBTransportPacket *)_dequeuePacket {
    if (_buffer.length < 8) return nil;
    UInt64 length = *((const UInt64 *)_buffer.bytes);
    length = CFSwapInt64BigToHost(length);
    if (length + 12 > _buffer.length) return nil;
    UInt32 type = ((const UInt32 *)_buffer.bytes)[2];
    type = CFSwapInt32BigToHost(type);
    
    NSData * payload = [_buffer subdataWithRange:NSMakeRange(12, length)];
    [_buffer replaceBytesInRange:NSMakeRange(0, length + 12) withBytes:NULL length:0];
    return [[UBTransportPacket alloc] initWithData:payload type:type];
}

@end
