//
//  UBTransportServer.h
//  Umbilical
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import <Foundation/Foundation.h>
#import "UBTransport.h"

@protocol UBTransportServer

- (void)transportServer:(id)sender gotTransport:(UBTransport *)transport;
- (void)transportServer:(id)sender failedWithError:(NSError *)error;

@end

@interface UBTransportServer : NSObject

@property (readonly) NSString * deviceName;
@property (nonatomic, weak) id<UBTransportServer> delegate;
@property (readonly) BOOL open;

- (id)initWithDeviceName:(NSString *)device;
- (void)startServer;
- (void)stopServer;

- (void)_handleClient:(UBTransport *)transport;
- (void)_handleError:(NSError *)error;

@end
