//
//  UBAppDelegate.h
//  UmbilicalControl
//
//  Created by Alex Nichol on 2/4/14.
//
//

#import <Cocoa/Cocoa.h>
#import "UBDisplaySocket.h"

@interface UBAppDelegate : NSObject <NSApplicationDelegate> {
    IBOutlet NSTextField * widthField, * mmWidthField;
    IBOutlet NSTextField * heightField, * mmHeightField;
}

@property (assign) IBOutlet NSWindow * window;

- (IBAction)disableAllDisplays:(id)sender;
- (IBAction)quitPressed:(id)sender;
- (IBAction)addDisplay:(id)sender;
- (void)getImage:(UBDisplaySocket *)socket;

@end
