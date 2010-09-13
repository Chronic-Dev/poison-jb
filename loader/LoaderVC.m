#import "LoaderVC.h"
#import "UIProgressHUD.h"
#import "Defines.h"
#import "Callback.h"
#import "MemLoaderOperation.h"
#import "NSURLDownload.h"
#import <unistd.h> 

@implementation LoaderVC

- (void)addHUDWithText:(NSString *)text {
	[self.view setUserInteractionEnabled:NO];
	[_myHud setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
	[_myHud setText:text];
	[_myHud showInView:[[UIApplication sharedApplication] keyWindow]];
}

- (void)removeHUD {
	[_myHud hide];
	[self.view setUserInteractionEnabled:YES];
}

- (void)viewDidLoad {
	[super viewDidLoad];

	self.title = kTitle;

	_queue = [[NSOperationQueue alloc] init];
	_myHud = [[UIProgressHUD alloc] initWithWindow:[[UIApplication sharedApplication] keyWindow]];
	[self addHUDWithText:@"Loading Sources"];
	
	// Loading Sources
	Callback *c = [[Callback alloc] initWithTarget:self action:@selector(updatedSources:) object:nil];
	MemLoaderOperation *o = [[MemLoaderOperation alloc] init];
	[o setUrl:[NSURL URLWithString:kLoaderURL]];
	[o setCallback:c];
	
	[_queue addOperation:o];
	[o release];
	[c release];
}

// -------
// Loader Delegates

- (void)updatedSources:(id)obj {
	if(obj == nil) {
		[self removeHUD];
		// TODO: UIAlertView "error"
	} else {
		NSDictionary *ref = (NSDictionary *)CFPropertyListCreateFromXMLData(kCFAllocatorDefault, (CFDataRef)obj, kCFPropertyListImmutable, NULL);
		if(ref != nil && [ref isKindOfClass:[NSDictionary class]]) {
			_sourceDict = [ref retain];
		}
		
		
		[self performSelector:@selector(removeHUD) withObject:nil afterDelay:1.0];
		[self.tableView performSelector:@selector(reloadData) withObject:nil afterDelay:1.0];
	}
}

// -------
// Table View Delegates

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return [[_sourceDict objectForKey:@"AvailableSoftware"] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	static NSString *CellIdentifier = @"Cell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
	if(cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
	}
	
	UIImage *img = [[[UIImage alloc] initWithContentsOfFile:[NSString stringWithFormat:@"/Applications/Loader.app/%@", [[[_sourceDict objectForKey:@"AvailableSoftware"] objectAtIndex:indexPath.row] objectForKey:@"Icon"]]] autorelease];
	cell.imageView.image = img;
	cell.textLabel.text = [[[_sourceDict objectForKey:@"AvailableSoftware"] objectAtIndex:indexPath.row] objectForKey:@"Name"];
	cell.detailTextLabel.text = [[[_sourceDict objectForKey:@"AvailableSoftware"] objectAtIndex:indexPath.row] objectForKey:@"Description"];
	
	return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
	return 55;
}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section {
	if(_sourceDict && [_sourceDict objectForKey:@"Version"]) {
		NSTimeInterval unixDate = (NSTimeInterval)[[_sourceDict objectForKey:@"LastUpdate"] longLongValue];
		NSDate *date = [NSDate dateWithTimeIntervalSince1970:unixDate];
		NSDateFormatter *form = [[[NSDateFormatter alloc] init] autorelease];
		[form setDateStyle:NSDateFormatterMediumStyle];
		[form setTimeStyle:NSDateFormatterShortStyle];
	
		return [NSString stringWithFormat:@"%@\nLast Update: %@\nVersion %@", [_sourceDict objectForKey:@"Message"], [form stringFromDate:date], [_sourceDict objectForKey:@"Version"]];
	}
	
	return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	NSDictionary *item = [[_sourceDict objectForKey:@"AvailableSoftware"] objectAtIndex:indexPath.row];
	_currentIndex = indexPath;

	UIActionSheet *a = [[UIActionSheet alloc] init];
	[a setDelegate:self];
	[a addButtonWithTitle:[NSString stringWithFormat:@"Install %@", [item objectForKey:@"Name"]]];
	[a addButtonWithTitle:@"Cancel"];
	[a setCancelButtonIndex:1];
	
	[a showInView:self.view];
	[a release];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	[self.tableView deselectRowAtIndexPath:_currentIndex animated:YES];

	if(buttonIndex != [actionSheet cancelButtonIndex]) {
		NSDictionary *item = [[_sourceDict objectForKey:@"AvailableSoftware"] objectAtIndex:_currentIndex.row];
		
		NSURLRequest *theRequest = [NSURLRequest requestWithURL:[NSURL URLWithString:[item objectForKey:@"URL"]]];
		NSURLDownload *theDownload = [[NSURLDownload alloc] initWithRequest:theRequest delegate:self];
		if(theDownload) {
			[theDownload setDestination:@"/tmp/loader_package.tar" allowOverwrite:YES];
			[self addHUDWithText:@"Downloading..."];
		} else {
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Unknown Error. Check Internet Connection." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alert show];
			[alert release];
		}
	}
}


- (void)download:(NSURLDownload *)download didReceiveDataOfLength:(unsigned)length {
	long long expectedLength = [downloadResponse expectedContentLength];
	bytesReceived = bytesReceived+length;
	
	float percentComplete = (bytesReceived/(float)expectedLength);
	NSString *str = [NSString stringWithFormat:@"Downloading... (%d", (int)(percentComplete*100)];
	str = [str stringByAppendingString:@"%)"];
	
	[_myHud setText:str];
}

- (void)download:(NSURLDownload *)download didReceiveResponse:(NSURLResponse *)response {
	bytesReceived = 0;
	[self setDownloadResponse:response];
}

- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error {
	[download release];
	
	[self removeHUD];
	
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error" message:[error localizedDescription] delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
}

- (void)setDownloadResponse:(NSURLResponse *)aDownloadResponse {
	[aDownloadResponse retain];
	[downloadResponse release];
	downloadResponse = aDownloadResponse;
}
 
- (void)removeStuff {
	NSFileManager *f = [NSFileManager defaultManager];
	[f removeItemAtPath:@"/tmp/loader_package.tar.gz" error:nil];
	[f removeItemAtPath:@"/bin/cdev-tar" error:nil];
}

- (void)suicide {
	[[NSFileManager defaultManager] removeItemAtPath:@"/Applications/Loader.app/" error:nil];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if(alertView.tag == 0xDEADBEEF) {
		if(buttonIndex != [alertView cancelButtonIndex]) {
			[self suicide];
		}
	}
}

- (void)extractPackage {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	//system("/bin/cdev-tar xf /tmp/loader_package.tar -C /");
	execl("/bin/cdev-tar", "xf", "/tmp/loader_package.tar", "-C", "/", NULL);	

	[self performSelectorOnMainThread:@selector(cleanUp) withObject:nil waitUntilDone:YES];

	[pool release];
}

- (void)cleanUp {
	[_myHud done];
	[_myHud setText:@"Success!"];

	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"Do you want to remove Loader from your device?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Remove", nil];
	[alert setTag:0xDEADBEEF];
	[alert show];
	[alert release];

	[self removeStuff];
	[self performSelector:@selector(removeHUD) withObject:nil afterDelay:2.0];
}

- (void)downloadDidFinish:(NSURLDownload *)download {
	[download release];
	[_myHud setText:@"Extracting..."];
	
	[NSThread detachNewThreadSelector:@selector(extractPackage) toTarget:self withObject:nil];
}

- (void)dealloc {
	if(_sourceDict) [_sourceDict release];
	
	[_queue release];
	[_myHud release];
	[super dealloc];
}


@end

