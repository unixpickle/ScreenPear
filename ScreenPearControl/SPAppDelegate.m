//
//  UBAppDelegate.m
//  ScreenPearControl
//
//  Created by Alex Nichol on 2/4/14.
//
//

#import "SPAppDelegate.h"

@implementation SPAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
}

- (IBAction)disableAllDisplays:(id)sender {
    [SPDisplaySocket closeAllDisplays];
}

- (IBAction)quitPressed:(id)sender {
    [SPDisplaySocket closeAllDisplays];
    [[NSApplication sharedApplication] terminate:sender];
}

- (IBAction)addDisplay:(id)sender {
    int width = [widthField intValue];
    int height = [heightField intValue];
    int mmWidth = [mmWidthField intValue];
    int mmHeight = [mmHeightField intValue];
    SPUserClientResolution res;
    res.width = width;
    res.height = height;
    res.imageWidth = mmWidth;
    res.imageHeight = mmHeight;
    SPDisplaySocket * socket = [SPDisplaySocket openUnusedDisplay:res];
    if (socket) {
        NSLog(@"got display %@", socket);
        [self performSelector:@selector(getImage:) withObject:socket afterDelay:60];
    } else {
        NSRunAlertPanel(@"Unable to Open", @"Failed to open a new display. All available displays may have been used.", @"OK", nil, nil);
    }
}

- (void)getImage:(SPDisplaySocket *)socket {
    NSImage * img = [socket createImage];
    [[img TIFFRepresentation] writeToFile:@"/Users/alex/Desktop/file.tiff" atomically:YES];
}

@end
