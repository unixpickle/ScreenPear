//
//  UBTransportScanner.m
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransportScanner.h"

@implementation UBTransportScanner

- (id)init {
    if ((self = [super init])) {
        identities = [NSMutableArray array];
    }
    return self;
}

- (void)start {
    _open = YES;
}

- (void)stop {
    _open = NO;
}

- (void)_handleFailed:(NSError *)error {
    if (!self.open) return;
    _open = NO;
    if ([self.delegate respondsToSelector:@selector(transport:failedWithError:)]) {
        [self.delegate transport:self failedWithError:error];
    }
}

- (void)_handleIdentity:(id<UBTransportIdentity>)identity {
    // check if exists
    for (id ident in identities) {
        if ([identity isEqualToIdentity:ident]) {
            return;
        }
    }
    [identities addObject:identity];
    if ([self.delegate respondsToSelector:@selector(transport:addIdentity:)]) {
        [self.delegate transport:self addIdentity:identity];
    }
}

- (void)_handleIdentityGone:(id<UBTransportIdentity>)identity {
    for (int i = 0; i < identities.count; i++) {
        id ident = identities[i];
        if ([identity isEqualToIdentity:ident]) {
            [identities removeObjectAtIndex:i];
            if ([self.delegate respondsToSelector:@selector(transport:removeIdentity:)]) {
                [self.delegate transport:self removeIdentity:ident];
            }
            return;
        }
    }
}

@end
