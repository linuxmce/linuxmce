//
//  MWAppDelegate.h
//  Mac Workstation
//
//  Created by Thomas Cherryhomes on 3/2/13.
//  Copyright (c) 2013 Thomas Cherryhomes. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MWAppDelegate : NSObject <NSApplicationDelegate>
{
    IBOutlet NSMenu *statusMenu;
    NSStatusItem *statusItem;
    NSImage *statusImage;
    NSImage *statusHighlightImage;
    
}

@end
