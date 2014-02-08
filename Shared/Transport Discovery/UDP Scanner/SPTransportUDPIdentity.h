//
//  UBTransportUDPIdentity.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import <Foundation/Foundation.h>
#import "SPTransportScanner.h"

@interface SPTransportUDPIdentity : NSObject <SPTransportIdentity>

@property (readonly) NSDictionary * addressPortPairs;
@property (readonly) NSString * deviceName;
@property (readonly) NSString * deviceType;

- (id)initWithData:(NSData *)data;

@end
