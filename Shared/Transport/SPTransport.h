//
//  UBTransport.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTransportPacket.h"

@protocol UBTransportDelegate <NSObject>

@optional
- (void)transport:(id)tp gotPacket:(SPTransportPacket *)packet;
- (void)transportClosed:(id)tp;

@end

/**
 * An abstract base-class for ScreenPear transport
 */
@interface SPTransport : NSObject {
    NSMutableData * _buffer;
}

@property (readonly) BOOL open;
@property (nonatomic, weak) id<UBTransportDelegate> delegate;

// basic interface
- (void)sendPacket:(SPTransportPacket *)packet;
- (void)shutdown;

// methods to be called by sub-classes
- (void)_gotData:(NSData *)data;
- (void)_closed;

// methods to be implemented by the sub-class
- (void)_writeData:(NSData *)data;

@end
