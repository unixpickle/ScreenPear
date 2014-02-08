//
//  UBTransportScanner.h
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import <Foundation/Foundation.h>
#import "UBTransport.h"

typedef void (^UBTransportConnectCallback)(NSError * err, UBTransport * transport);

@protocol UBTransportIdentity

- (id)openTransport:(UBTransportConnectCallback)callback;
- (void)cancelOpen:(id)job;

- (BOOL)isEqualToIdentity:(id)identity;

@end

@protocol UBTransportScannerDelegate <NSObject>

@optional
- (void)transport:(id)transport failedWithError:(NSError *)error;
- (void)transport:(id)transport addIdentity:(id<UBTransportIdentity>)identity;
- (void)transport:(id)transport removeIdentity:(id<UBTransportIdentity>)identity;

@end

@interface UBTransportScanner : NSObject {
    NSMutableArray * identities;
}

@property (nonatomic, weak) id<UBTransportScannerDelegate> delegate;
@property (readonly) BOOL open;

- (void)start;
- (void)stop;

- (void)_handleFailed:(NSError *)error;
- (void)_handleIdentity:(id<UBTransportIdentity>)identity;
- (void)_handleIdentityGone:(id<UBTransportIdentity>)identity;

@end
