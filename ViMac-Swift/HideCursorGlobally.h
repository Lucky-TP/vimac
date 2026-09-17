//
//  HideCursorGlobally.h
//  Vimac
//
//  Created by Dexter Leng on 1/1/20.
//  Copyright © 2020 Dexter Leng. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

@interface HideCursorGlobally : NSObject
+ (void) hide;
+ (void) unhide;
@end

NS_ASSUME_NONNULL_END
