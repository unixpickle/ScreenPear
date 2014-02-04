//
//  UBConnection.h
//  Umbilical
//
//  Created by Alex Nichol on 2/2/14.
//
//

#import <Foundation/Foundation.h>
#import "UBUserClientShared.h"

@interface UBConnection : NSObject

@property (readonly) io_connect_t connect;

- (BOOL)open;
- (void)close;
- (BOOL)getCount:(uint64_t *)count;
- (BOOL)getEnabled:(uint64_t *)flag atIndex:(uint64_t)index;

- (BOOL)setMode:(UBUserClientResolution)resolution atIndex:(uint64_t)index;
- (BOOL)disableAtIndex:(uint64_t)index;

@end
