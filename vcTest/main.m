//
//  main.m
//  vcTest
//
//  Created by Alex Nichol on 2/3/14.
//
//

#import <Foundation/Foundation.h>
#import "UBConnection.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        UBConnection * conn = [[UBConnection alloc] init];
        if (![conn open]) {
            NSLog(@"failed to open connection");
            return 1;
        }
        
        uint64_t count = 0;
        if (![conn getCount:&count]) {
            NSLog(@"failed to get display count");
            return 1;
        }
        NSLog(@"there are %llu displays.", (unsigned long long)count);
        
        if (![conn getModeCount:&count atIndex:0]) {
            NSLog(@"failed to get mode count at index 0");
            return 1;
        }
        NSLog(@"there are %llu modes for display 0.", (unsigned long long)count);
        
        if (![conn setMode:10 withDepth:2 forDisplay:0]) {
            NSLog(@"failed to set mode 10 with depth 2 for display 0");
            return 1;
        }
        NSLog(@"set display mode");
        if (![conn setEnabled:1 forDisplay:0]) {
            NSLog(@"failed to set enabled = true for display 0");
            return 1;
        }
        NSLog(@"hit enter to disable display again...");
        
        char buffer[512];
        fgets(buffer, 512, stdin);
        
        if (![conn setEnabled:0 forDisplay:0]) {
            NSLog(@"failed to set enabled = false for display 0");
            return 1;
        }
        
        [conn close];
        return 0;
    }
    return 0;
}

