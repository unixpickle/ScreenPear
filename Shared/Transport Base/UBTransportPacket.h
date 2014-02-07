//
//  UBTransportPacket.h
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import <Foundation/Foundation.h>

@interface UBTransportPacket : NSObject

@property (readonly) NSData * data;
@property (readonly) UInt32 type;

- (id)initWithData:(NSData *)data type:(UInt32)type;

@end
