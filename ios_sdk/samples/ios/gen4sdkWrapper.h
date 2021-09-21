/**
 * License Agreement
 * Copyright (c) 2017 Analog Devices Inc.All rights reserved.
 * This source code is intended for the recipient only under the guidelines of the non-disclosure agreement with Analog Devices Inc.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * Warning: Do not Attempt to Refractor/Modify methods/variables of this file
 */

#import <Foundation/Foundation.h>


/**
 <#Description#> An objecvtive C wrapper which invokes native Gen4 SDK calls and interpret its callbacks. 
 */
@interface gen4sdkWrapper : NSObject
+ (id)sharedInstance;
-(void)Dispatchpacket : (NSData *)receivedbyte;
-(void)StartADXL;
-(void)StopADXL;
-(void)StopECG;
-(void)StartECG;
-(void)initializeSDK;

@end
