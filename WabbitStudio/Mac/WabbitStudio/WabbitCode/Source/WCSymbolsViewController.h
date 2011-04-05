//
//  WCSymbolsViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectNavigationViewController.h"


@interface WCSymbolsViewController : WCProjectNavigationViewController <NSTableViewDelegate> {
@private
    IBOutlet NSTableView *_tableView;
}
@property (readonly,nonatomic) NSTableView *tableView;
@end
