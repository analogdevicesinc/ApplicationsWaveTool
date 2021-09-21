/**
 * License Agreement
 * Copyright (c) 2017 Analog Devices Inc.All rights reserved.
 * This source code is intended for the recipient only under the guidelines of the non-disclosure agreement with Analog Devices Inc. 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#import "gen4sdkBLE.h"
#import "Gen4SDK_iosSample-Swift.h"
#import "gen4sdkWrapper.h"

static NSString *SERVICE_UUID = @"6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static NSString *TX_UDID   = @"6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static NSString *RX_UDID = @"6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

/**
 <#Description#> This BLE connection layer handles BLE scan,BLE Connect/Disconnect ,BLE data send/receive.
 */
gen4sdkWrapper *WRapperObjective;
ViewController *viewcallback ;

@implementation gen4sdkBLE


+ (id)sharedInstance {
  static gen4sdkBLE *sharedInstance = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedInstance = [[self alloc] init];
  });
  return sharedInstance;
}

-(id)init
{
    self = [super init];
    
    if (self)
    {
        ListedDevices = [[NSMutableArray alloc]init];
        viewcallback = [ViewController new];
        foundservices = [[NSMutableArray alloc]init];
        WRapperObjective = [gen4sdkWrapper sharedInstance];
        dispatch_queue_t   centralQueue = dispatch_queue_create("centralQueue", DISPATCH_QUEUE_CONCURRENT);
        MyCentral = [[CBCentralManager alloc] initWithDelegate:self queue:centralQueue options:@{CBCentralManagerOptionShowPowerAlertKey: @(YES)}];
        MyCentral.delegate = self;
    }
    return self;
}


/**
 <#Description#> Checking BLE feature is currently powered ON/OFF,if it is ON then start scan devices.

 @param central <#central description#>
 */
-(void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    
    
    CBManagerState currentstate = [central state];
    
    if (currentstate == CBCentralManagerStatePoweredOff)
    {
        NSLog(@"BLE Power OFF");
    }
    else if (currentstate == CBCentralManagerStatePoweredOn)
    {
        NSLog(@"BLE Power ON");
        [self ScanDevices];
    }
    
    
}


/**
 <#Description#> Establishing local connection with ADI BLE device.If connection is failure showing the alert.

 @param number <#number description#>
 */
-(void)connectperipheral : (NSInteger) number
{
    ConnnectedPeripheral = [ListedDevices objectAtIndex:number];
    
    [MyCentral connectPeripheral:ConnnectedPeripheral options:@{CBConnectPeripheralOptionNotifyOnDisconnectionKey:@(YES),CBCentralManagerOptionShowPowerAlertKey:@(YES)}];
    
}



/**
 <#Description#> Discover the BLE gatt services and checking the name starts with "ADI".If present then connect.

 @param central <#central description#>
 @param peripherals <#peripherals description#>
 @param advertisementData <#advertisementData description#>
 @param RSSI <#RSSI description#>
 */
-(void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripherals advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI
{
    
    
    NSRange range = [peripherals.name rangeOfString:@"STUDY"];
    
    if (range.location == NSNotFound)
    {
        NSLog(@"not FOund---%@",peripherals);
        
    }
    else
    {
        if (![ListedDevices containsObject:peripherals])
        {
            if (peripherals.name != NULL)
            {
                [ListedDevices addObject:peripherals];

            }
            
        }
        
    }
    
    
    NSLog(@"discovered identifier---%@",[peripherals.identifier UUIDString]);
    
}



/**
 <#Description#> Checking whether ADI BLE device Connected or not.

 @param central <#central description#>
 @param peripherals <#peripherals description#>
 */
-(void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripherals
{
    
    ConnnectedPeripheral = peripherals;
    ConnnectedPeripheral.delegate = self;
    [MyCentral stopScan];
  

    [ConnnectedPeripheral discoverServices:nil];

    
    
}



/**
 <#Description#> This method is invoked when your app calls the discoverServices method. If the services of the ADI BLE device are successfully discovered, you can access them through the BLE services property. If successful, the error parameter is nil. If unsuccessful, the error parameter returns the cause of the failure.

 @param peripherals <#peripherals description#>
 @param error <#error description#>
 */
-(void)peripheral:(CBPeripheral *)peripherals didDiscoverServices:(NSError *)error
{
    
    for (CBService *service in peripherals.services)
    {
        if (![foundservices containsObject:service])
        {
            [foundservices addObject: service];
            [peripherals discoverCharacteristics:nil forService:service];
            NSLog(@"Discovered service-%@",service);
            
        }
    }
}


/**
 <#Description#> This method invoked when an existing connection with a ADI BLE device is turn down or disconnected.

 @param central <#central description#>
 @param peripheral <#peripheral description#>
 @param error <#error description#>
 */
-(void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSString *Peripheralname = peripheral.name;
    NSLog(@"--Disconnect-%@",Peripheralname);
    NSLog(@"disconnected___ERRROR___%@",error);
    
 
    
}

/**
 <#Description#> The delegate uses this protocol’s methods to monitor the discovery, exploration, and interaction of a remote ADI BLE services and properties.

 @param peripherals <#peripherals description#>
 @param service <#service description#>
 @param error <#error description#>
 */
-(void)peripheral:(CBPeripheral *)peripherals didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
    
    for (CBCharacteristic *characteristic in service.characteristics)
    {
        
        
        if ([characteristic.UUID isEqual:[CBUUID UUIDWithString:RX_UDID      ]])
        {
            [peripherals readValueForCharacteristic:characteristic];
            [peripherals setNotifyValue:YES forCharacteristic:characteristic];
            NSLog(@"Discovered characteristic-%@",characteristic);
          [[NSNotificationCenter defaultCenter]postNotificationName:@"updateBLEConnection" object:nil];
          
            
        }
        
        if ([characteristic.UUID isEqual:[CBUUID UUIDWithString:TX_UDID]])
        {
            
        }
    }
}


/**
 <#Description#> This method invoked when you retrieve a specified characteristic’s value, or when the ADI BLE device notifies your app that the characteristic’s value has changed.

 @param peripheral <#peripheral description#>
 @param characteristic <#characteristic description#>
 @param error <#error description#>
 */
-(void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    
    NSData *ReceivedPacket  = characteristic.value;
    
    [WRapperObjective Dispatchpacket:ReceivedPacket];
    NSLog(@"Packet-DATA-%@",characteristic.value);
    
}


/**
 <#Description#> Scans for ADI BLE devices that are advertising services.
 */
-(void)ScanDevices
{
    [MyCentral scanForPeripheralsWithServices:nil options:nil];

}

/**
 <#Description#> Write byte stream to ADI BLE device's bluetooth characteristics.

 @param writeData <#writeData description#>
 */
-(void)writedata : (NSData *)writeData
{
    for (CBService *service in ConnnectedPeripheral.services)
    {
        
        if ([service.UUID isEqual:[CBUUID UUIDWithString:SERVICE_UUID]]) {
            
            
            
            for (CBCharacteristic *charac in service.characteristics)
            {
                
                if ([charac.UUID isEqual:[CBUUID UUIDWithString:TX_UDID]]) {
                    NSLog(@"writedata-%@",writeData);
              
                    @try {
                        [ConnnectedPeripheral writeValue:writeData
                                forCharacteristic:charac type:CBCharacteristicWriteWithResponse];
                        
                        [ConnnectedPeripheral setNotifyValue:YES forCharacteristic:charac];
                        
                    } @catch (NSException *exception) {
                        NSLog(@"error_%@",exception);
                    }
                                        
                }
            }
        }
    }

}

/**
 <#Description#> Disconnect ADI BLE devices.
 */
-(void)DisconnectDevice
{
    for (CBService *Service in ConnnectedPeripheral.services)
    {
        for (CBCharacteristic *characteristic in Service.characteristics)
        {

        [ConnnectedPeripheral setNotifyValue:NO forCharacteristic:characteristic];
        }
    }
    [MyCentral stopScan];
    if (ConnnectedPeripheral) {
        [MyCentral cancelPeripheralConnection:ConnnectedPeripheral];
        
    }

}

-(NSArray *)ListDownPeripheral
{
    return ListedDevices;
    
}

-(void)RemoveAllDevices
{
    [ListedDevices removeAllObjects];
    
}

-(void)CallSwiftFunction : (NSData *)data
{
 
    [viewcallback CallbackObjectiveCdataWithData:data];
    NSLog(@"TX data - %@",data);
    
}
@end
