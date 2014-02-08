//
//  UBDisplaySocket.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/5/14.
//
//

#import <Foundation/Foundation.h>
#import "SPUserClientShared.h"

@interface SPDisplaySocket : NSObject {
    void * mappedBuffer;
    BOOL isOpen;
}

@property (readonly) io_connect_t connect;
@property (readonly) SPUserClientResolution resolution;

+ (SPDisplaySocket *)openUnusedDisplay:(SPUserClientResolution)resolution;
+ (void)closeAllDisplays;

- (void)close;

- (void *)framebuffer;
- (void)requestUpdate;
- (NSImage *)createImage;

@end
