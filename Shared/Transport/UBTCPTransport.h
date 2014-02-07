//
//  UBTCPTransport.h
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import "UBTransport.h"

@interface UBTCPTransport : UBTransport {
    NSThread * readThread, * writeThread;
    NSRunLoop * writeRunloop;
    int fd;
    NSUInteger ordering;
}

+ (void)initialize;
- (id)initWithFileDescriptor:(int)fd;

@end
