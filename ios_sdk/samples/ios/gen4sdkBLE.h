/**
 * License Agreement
 * Copyright (c) 2017 Analog Devices Inc.All rights reserved.
 * This source code is intended for the recipient only under the guidelines of the non-disclosure agreement with Analog Devices Inc.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#define kNoResponse @"kNoResponse"

/**
 <#Description#> This BLE connection layer handles BLE scan,BLE Connect/Disconnect ,BLE data send/receive.
 */
@class ViewController;

@interface gen4sdkBLE : NSObject<CBCentralManagerDelegate,CBPeripheralDelegate>
{
    CBCentralManager *MyCentral;
    CBPeripheral *ConnnectedPeripheral;
    NSMutableArray *ListedDevices,*foundservices;

    
}
+ (id)sharedInstance;
-(void)CallSwiftFunction : (NSData *)data;
-(void)ScanDevices;
-(NSArray *)ListDownPeripheral;
-(void)connectperipheral : (NSInteger) number;
-(void)writedata : (NSData *)writeData;
-(void)RemoveAllDevices;
-(void)DisconnectDevice;

@end
