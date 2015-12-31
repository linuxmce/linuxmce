//
//  Created by Serge Wagener on 10/08/10.
//  Copyright 2010. All rights reserved.
//
//  http://www.linuxmce.org
//

#import "proxy-orbiter-client.h"

@implementation proxy_orbiter_client

@synthesize orbiterImage;
@synthesize server, port, pollInterval;

BOOL isSending = NO, isNews = YES;

NSMutableData *data, *imageData;

NSInputStream *iStream;
NSOutputStream *oStream;

NSInteger currentState = 0, imageSize = 0, imageLoadedSize = 0; 

CFReadStreamRef readStream = NULL;
CFWriteStreamRef writeStream = NULL;

id _delegate;

- (id)init:(id)delegate {
	if ((self=[super init]) != nil) {
		[self setDelegate:delegate];
	}
	return (self);	
}

- (void)connect {
	NSLog(@"Connecting to proxy-orb %@ port %d...",server, port);
 	if ([server isEqualToString:@""]) {
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Connection error" message:@"Empty server address" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alert show];
			[alert release];
            return;
        }

	// create input and output network streams
	CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault, (CFStringRef) server, port, &readStream, &writeStream);
    if (readStream && writeStream) {
        // set properties
		
		//Does not work in iOS 3.2 :-(
		//CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
        //CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
        
        // open input strem
		iStream = (NSInputStream *)readStream;
        [iStream retain];
        [iStream setDelegate:self];
        [iStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [iStream open];
        
        // open output stream
		oStream = (NSOutputStream *)writeStream;
        [oStream retain];
        [oStream setDelegate:self];
        [oStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [oStream open];         
    }
}

- (void)connected {
	if([_delegate respondsToSelector:@selector(didConnect)]) {
		[_delegate didConnect];
	}
	else {
		NSLog(@"didConnect delegate not present");
	}
}

- (void)receivedNews {
	if([_delegate respondsToSelector:@selector(didReceiveNews)]) {
		[_delegate didReceiveNews];
	}
	else {
		NSLog(@"didReceiveNews delegate not present");
	}
}

- (void)updateImage:(UIImage *)newImage {
	if([_delegate respondsToSelector:@selector(didReceiveNewImage:)]) {
		[_delegate didReceiveNewImage:newImage];
	}
	else {
		NSLog(@"didReceiveNewImage delegate not present");
	}
}

- (void)connectionError:(NSString *)errorMessage {
	if([_delegate respondsToSelector:@selector(didReceiveConnectionError:)]) {
		[_delegate didReceiveConnectionError:errorMessage];
	}
	else {
		NSLog(@"didReceiveConnectionError delegate not present");
	}
	
}

-(void) disconnect {
    [iStream close];
    [oStream close];
}

-(void) writeToServer:(NSString *) str {
	NSLog(@"Sending (%.0fMB free memory) %@", [UIDevice currentDevice].availableMemory, str);
	const uint8_t *buf = (uint8_t *) [str cStringUsingEncoding:NSASCIIStringEncoding];	
	[oStream write:buf maxLength:strlen((char*)buf)];  
}

- (void) sendTouch:(NSInteger) x y:(NSInteger) y {
	NSLog(@"Sending touch %dx%d", x, y);
	NSString *out = [NSString stringWithFormat:@"TOUCH %dx%d\n", x, y];
	[self writeToServer:out];
	[self downloadImage];
}

-(void) poll {
	//NSLog(@"Polling");
	if(currentState == 0) [self writeToServer:@"ANYNEWS?\n"];
	if(isNews) {
		isNews=NO;
		[self downloadImage];
	}
}

-(void) downloadImage {
	NSLog(@"Downloading image");
	[self writeToServer:@"IMAGE\n"];
}

char *append(const char *oldstring, const char c)
{
    int result;
    char *newstring;
    result = asprintf(&newstring, "%s%c", oldstring, c);
    if (result == -1) newstring = NULL;
    return newstring;
}

- (void)stream:(NSStream *)stream handleEvent:(NSStreamEvent)eventCode {
	// here is where most of the orbiter com. logic is in
    switch(eventCode) {
        
		case NSStreamEventOpenCompleted: {
            if(stream == oStream) self.connected;
			isNews=YES;
        } break;
			
        // there is something in the input buffer. The problem with NSStream we have to deal with is
		// that is doesn't support a "lineread" mode. That means there can be any amount of data in the
		// input buffer including multiple lines and binary data. example: when proxy-orb returns the
		// IMAGE information with the imagesize the binary part of the image can start in the same received
		// buffer data. You really have to filter parts out manually .... pffff .....
			
		// 2nd problem is that all this didn't work with cocoa's NSString. I had to rewrite everything to
		// work with plain C char
		case NSStreamEventHasBytesAvailable:
        {
            if (data == nil) data = [[NSMutableData alloc] init]; 
			uint8_t buf[4096];
			char *size = malloc(10);
            unsigned int len = 0, skip = 0;

			//read a max 2k chunk from buffer
			len = [(NSInputStream *)stream read:buf maxLength:4096];
			//NSLog(@"In: %s", &buf);
			// check if there is an image
			if(currentState == 0) {
				char *substr = strstr((char *)buf, "IMAGE");
				if(substr  != NULL) { // Will receive image
					skip=substr-(char *)buf;
					//NSLog(@"DIFF: %d", skip);
					size[0] = 0;
					int i = 0;
					// Skip word "IMAGE" + space and parse for image size
					substr += 6; 
					while((i < strlen(substr)) && (substr[i] != 10)) size=append(size, substr[i++]);			
					skip += 7 + strlen(size); // 7 because of lf after size
					NSLog(@"Will receive image with size of %d bytes", atoi(size));
					imageSize = atoi(size);
					currentState = 1;
					if (imageData != nil) [imageData release];
					imageData = [[NSMutableData alloc] init];
					imageLoadedSize = 0;
				} 
				else if(strstr((char *)buf, "no") != NULL) {
					//NSLog(@"No news...");
				}
				else if(strstr((char *)buf, "yes") != NULL) {
					NSLog(@"There are news...");
					isNews = YES;
				}

			}
			// here we download the image
			if(currentState == 1) {
				int length = len - skip;
				//NSLog(@"Length: %d, is - ils: %d", length, (imageSize-imageLoadedSize));
				if(length > (imageSize - imageLoadedSize)) length = imageSize - imageLoadedSize;
				[imageData appendBytes:(const void *)&buf[skip] length:length];
				imageLoadedSize += length;
				//NSLog(@"Image read: %d. Total:%d/%d", length, imageLoadedSize, imageSize);
				skip=0;
				if(imageLoadedSize >= imageSize){
					currentState = 0;
					imageLoadedSize = 0;
					UIImage* downloadedImage = [[UIImage alloc] initWithData:imageData];
					NSLog(@"Image received, resolution %3.fx%3.f", downloadedImage.size.width, downloadedImage.size.height);
					[self updateImage:downloadedImage];
				}
			}
			
			[data release]; 
            data = nil;
        } break;
			
		case NSStreamEventErrorOccurred:
        {
            if(stream == oStream) {
				NSError *theError = [stream streamError];            			
				[self connectionError:[NSString stringWithFormat:@"Error %i: %@", [theError code], [theError localizedDescription]]];
            }
			break ;
        }   
			
		case NSStreamEventEndEncountered:
			{
				[stream close];
				[stream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
				[stream release];
				stream = nil; 
				[self connectionError:@"Stream end..."];
			} break;
			
        }
}

- (void)dealloc {
    [self disconnect];
    
    [iStream release];
    [oStream release];
    
    if (readStream) CFRelease(readStream);
    if (writeStream) CFRelease(writeStream);
	
    [super dealloc];
}

- (id)delegate {
	return _delegate;
}

- (void)setDelegate:(id)newDelegate {
	_delegate = newDelegate;
}

@end
