//
//  MWAppDelegate.m
//  Mac Workstation
//
//  Created by Thomas Cherryhomes on 3/2/13.
//  Copyright (c) 2013 Thomas Cherryhomes. All rights reserved.
//

#import "MWAppDelegate.h"

@implementation MWAppDelegate

- (void) awakeFromNib
{
    statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    NSBundle *bundle = [NSBundle mainBundle];
    statusImage = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"Icon_r" ofType:@"png"]];
    statusHighlightImage = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"Icon_h" ofType:@"png"]];
    [statusItem setImage:statusImage];
    [statusItem setAlternateImage:statusHighlightImage];
    [statusItem setMenu:statusMenu];
    [statusItem setHighlightMode:YES];    
}

@end
