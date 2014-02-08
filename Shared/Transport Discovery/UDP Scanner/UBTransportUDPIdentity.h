//
//  UBTransportUDPIdentity.h
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import <Foundation/Foundation.h>
#import "UBTransportScanner.h"

@interface UBTransportUDPIdentity : NSObject <UBTransportIdentity>

@property (readonly) NSDictionary * addressPortPairs;
@property (readonly) NSString * deviceName;
@property (readonly) NSString * deviceType;

- (id)initWithData:(NSData *)data;

@end
