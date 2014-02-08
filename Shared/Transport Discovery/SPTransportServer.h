//
//  UBTransportServer.h
//  ScreenPear
//
//  Created by Alex Nichol on 2/7/14.
//
//

#import <Foundation/Foundation.h>
#import "SPTransport.h"

@protocol SPTransportServer

- (void)transportServer:(id)sender gotTransport:(SPTransport *)transport;
- (void)transportServer:(id)sender failedWithError:(NSError *)error;

@end

@interface SPTransportServer : NSObject

@property (readonly) NSString * deviceName;
@property (nonatomic, weak) id<SPTransportServer> delegate;
@property (readonly) BOOL open;

- (id)initWithDeviceName:(NSString *)device;
- (void)startServer;
- (void)stopServer;

- (void)_handleClient:(SPTransport *)transport;
- (void)_handleError:(NSError *)error;

@end
