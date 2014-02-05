//
//  UBDisplaySocket.h
//  Umbilical
//
//  Created by Alex Nichol on 2/5/14.
//
//

#import <Foundation/Foundation.h>
#import "UBUserClientShared.h"

@interface UBDisplaySocket : NSObject {
    void * mappedBuffer;
    BOOL isOpen;
}

@property (readonly) io_connect_t connect;
@property (readonly) UBUserClientResolution resolution;

+ (UBDisplaySocket *)openUnusedDisplay:(UBUserClientResolution)resolution;
+ (void)closeAllDisplays;

- (void)close;

- (void *)framebuffer;
- (NSImage *)createImage;

@end
