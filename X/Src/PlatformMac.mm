//====================================================================================================
// Filename:	PlatformMac.mm
// Created by:	Platform abstraction for macOS
//====================================================================================================

#if defined(__APPLE__)

#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <cstring>
#include <dispatch/dispatch.h>

// Helper to convert file extension to UTType
static UTType* UTTypeFromExtension(NSString* ext) API_AVAILABLE(macos(11.0)) {
    return [UTType typeWithFilenameExtension:ext];
}

// These are declared in XEngine.cpp and implemented here for macOS
extern "C" {

bool X_OpenFileDialog(char* fileName, int maxPath, const char* title, const char* filter)
{
    @autoreleasepool {
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        [panel setTitle:[NSString stringWithUTF8String:title]];
        [panel setCanChooseFiles:YES];
        [panel setCanChooseDirectories:NO];
        [panel setAllowsMultipleSelection:NO];

        // Parse filter string (format: "Description\0*.ext\0...\0\0")
        if (filter) {
            NSMutableArray<UTType*>* contentTypes = [NSMutableArray array];
            const char* p = filter;
            bool isExtension = false;

            while (*p || *(p + 1)) {
                if (*p == '\0') {
                    isExtension = !isExtension;
                    p++;
                    continue;
                }

                if (isExtension) {
                    NSString* extString = [NSString stringWithUTF8String:p];
                    NSArray* parts = [extString componentsSeparatedByString:@";"];
                    for (NSString* part in parts) {
                        NSString* trimmed = [part stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
                        if ([trimmed hasPrefix:@"*."]) {
                            NSString* ext = [trimmed substringFromIndex:2];
                            if (![ext isEqualToString:@"*"]) {
                                if (@available(macOS 11.0, *)) {
                                    UTType* type = UTTypeFromExtension(ext);
                                    if (type) {
                                        [contentTypes addObject:type];
                                    }
                                }
                            }
                        }
                    }
                }

                while (*p) p++;
                p++;
            }

            if (@available(macOS 11.0, *)) {
                if ([contentTypes count] > 0) {
                    [panel setAllowedContentTypes:contentTypes];
                }
            }
        }

        NSModalResponse response = [panel runModal];

        if (response == NSModalResponseOK) {
            NSURL* url = [[panel URLs] firstObject];
            if (url) {
                const char* path = [[url path] UTF8String];
                strncpy(fileName, path, maxPath - 1);
                fileName[maxPath - 1] = '\0';
                return true;
            }
        }
    }

    return false;
}

bool X_SaveFileDialog(char* fileName, int maxPath, const char* title, const char* filter)
{
    @autoreleasepool {
        NSSavePanel* panel = [NSSavePanel savePanel];
        [panel setTitle:[NSString stringWithUTF8String:title]];

        // Parse filter for default extension
        if (filter) {
            const char* p = filter;
            while (*p) p++;
            p++;
            if (*p) {
                NSString* extString = [NSString stringWithUTF8String:p];
                NSArray* parts = [extString componentsSeparatedByString:@";"];
                NSString* firstExt = [parts firstObject];
                if ([firstExt hasPrefix:@"*."]) {
                    NSString* ext = [firstExt substringFromIndex:2];
                    if (![ext isEqualToString:@"*"]) {
                        if (@available(macOS 11.0, *)) {
                            UTType* type = UTTypeFromExtension(ext);
                            if (type) {
                                [panel setAllowedContentTypes:@[type]];
                            }
                        }
                    }
                }
            }
        }

        if (fileName[0] != '\0') {
            NSString* suggested = [NSString stringWithUTF8String:fileName];
            [panel setNameFieldStringValue:[suggested lastPathComponent]];
        }

        NSModalResponse response = [panel runModal];

        if (response == NSModalResponseOK) {
            NSURL* url = [panel URL];
            if (url) {
                const char* path = [[url path] UTF8String];
                strncpy(fileName, path, maxPath - 1);
                fileName[maxPath - 1] = '\0';
                return true;
            }
        }
    }

    return false;
}

} // extern "C"

#endif // __APPLE__
