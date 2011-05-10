#import <Foundation/Foundation.h>

enum {
    NoStringEncoding = 0xFFFFFFFF
};

enum {
    WantsAutomaticTag = -1
};


@interface EncodingPopUpButtonCell : NSPopUpButtonCell {
}
@end


    
@interface EncodingManager : NSObject {
    @private
    IBOutlet NSMatrix *encodingMatrix;
    NSArray *encodings;
}

/* There is just one instance...
*/
+ (EncodingManager *)sharedInstance;

/* List of encodings that should be shown in encoding lists
*/
- (NSArray *)enabledEncodings;

/* Empties then initializes the supplied popup with the supported encodings.
*/
- (void)setupPopUpCell:(EncodingPopUpButtonCell *)button selectedEncoding:(NSStringEncoding)selectedEncoding withDefaultEntry:(BOOL)includeDefaultItem;

/* Action methods for bringing up and dealing with changes in the encodings list panel
*/
- (IBAction)showPanel:(id)sender;
- (IBAction)encodingListChanged:(id)sender;
- (IBAction)clearAll:(id)sender;
- (IBAction)selectAll:(id)sender;
- (IBAction)revertToDefault:(id)sender;
    
/* Internal method to save and communicate changes to the encoding list
*/
- (void)noteEncodingListChange:(BOOL)writeDefault updateList:(BOOL)updateList postNotification:(BOOL)post;


@end
