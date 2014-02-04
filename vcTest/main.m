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
        if (argc != 4) {
            NSLog(@"Usage: %s <index> <width> <height>", argv[0]);
            return 1;
        }
        
        int index = atoi(argv[1]);
        int width = atoi(argv[2]);
        int height = atoi(argv[3]);
        
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
        
        UBUserClientResolution resolution;
        bzero(&resolution, sizeof(resolution));
        resolution.width = width;
        resolution.height = height;
        
        if (![conn setMode:resolution atIndex:index]) {
            NSLog(@"failed to set mode for display %d", index);
            return 1;
        }
        
        NSLog(@"hit enter to disable display again...");
        
        char buffer[512];
        fgets(buffer, 512, stdin);
        
        if (![conn disableAtIndex:index]) {
            NSLog(@"failed to set enabled = false for display 0");
            return 1;
        }
        
        [conn close];
        return 0;
    }
    return 0;
}

