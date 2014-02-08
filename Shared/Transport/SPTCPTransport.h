//
//  UBTCPTransport.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "SPTransport.h"

@interface SPTCPTransport : SPTransport {
    NSThread * readThread, * writeThread;
    NSRunLoop * writeRunloop;
    int fd;
    NSUInteger ordering;
}

+ (void)initialize;
- (id)initWithFileDescriptor:(int)fd;

@end
