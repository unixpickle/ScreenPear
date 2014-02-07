//
//  UBTransport.h
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransportPacket.h"

@protocol UBTransportDelegate <NSObject>

- (void)transport:(id)tp gotPacket:(UBTransportPacket *)packet;
- (void)transportClosed:(id)tp;

@end

/**
 * An abstract base-class for Umbilical transport
 */
@interface UBTransport : NSObject {
    NSMutableData * _buffer;
}

@property (readonly) BOOL open;
@property (nonatomic, weak) id<UBTransportDelegate> delegate;

// basic interface
- (void)sendPacket:(UBTransportPacket *)packet;
- (void)shutdown;

// methods to be called by sub-classes
- (void)_gotData:(NSData *)data;
- (void)_closed;

// methods to be implemented by the sub-class
- (void)_writeData:(NSData *)data;

@end
