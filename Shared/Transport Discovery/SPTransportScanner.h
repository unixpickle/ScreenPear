//
//  UBTransportScanner.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import <Foundation/Foundation.h>
#import "SPTransport.h"

typedef void (^SPTransportConnectCallback)(NSError * err, SPTransport * transport);

@protocol SPTransportIdentity

- (id)openTransport:(SPTransportConnectCallback)callback;
- (void)cancelOpen:(id)job;

- (BOOL)isEqualToIdentity:(id)identity;

@end

@protocol SPTransportScannerDelegate <NSObject>

@optional
- (void)transport:(id)transport failedWithError:(NSError *)error;
- (void)transport:(id)transport addIdentity:(id<SPTransportIdentity>)identity;
- (void)transport:(id)transport removeIdentity:(id<SPTransportIdentity>)identity;

@end

@interface SPTransportScanner : NSObject {
    NSMutableArray * identities;
}

@property (nonatomic, weak) id<SPTransportScannerDelegate> delegate;
@property (readonly) BOOL open;

- (void)start;
- (void)stop;

- (void)_handleFailed:(NSError *)error;
- (void)_handleIdentity:(id<SPTransportIdentity>)identity;
- (void)_handleIdentityGone:(id<SPTransportIdentity>)identity;

@end
