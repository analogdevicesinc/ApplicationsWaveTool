/**
 * License Agreement
 * Copyright (c) 2017 Analog Devices Inc.All rights reserved.
 * This source code is intended for the recipient only under the guidelines of the non-disclosure agreement with Analog Devices Inc.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * Warning: Do not Attempt to Refractor/Modify methods/variables of this file
 */

#import "gen4sdkWrapper.h"
#import "gen4sdkBLE.h"
#import "iosSDKSample-Bridging-Header.h"
#import <Gen4SDK/Gen4SDK.h>

/**
 <#Description#> An objecvtive C wrapper which invokes native Gen4 SDK calls and interpret its callbacks.
 */
@implementation gen4sdkWrapper

gen4sdkBLE *wrapperswift = [gen4sdkBLE sharedInstance];


+ (id)sharedInstance {
  static gen4sdkWrapper *sharedInstance = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedInstance = [[self alloc] init];
  });
  return sharedInstance;
}

class phy_cb: public watch_phy_callback {
public:
    void call(std::vector<uint8_t> pkt) {

        std::cout << "This is PHY, got something to write:" << std::endl;
        NSMutableData *packetdata = [[NSMutableData alloc]init];

        for (int i = 0; i < pkt.size(); i++) {
            [packetdata appendBytes:&pkt[i] length:1];
            std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << (int) pkt[i];
        }
        std::cout << std::endl;
        
        [wrapperswift CallSwiftFunction:packetdata];
        
    };
  void sys_alert_call(uint8_t alert)
  {
    std::cout << "SDK Alert Message -" << alert << std::endl;

  };
};



class AdxlStreamCallback : public adxl_stream_callback {
public:
    void call(std::vector<adxl_stream_cb_data_t> pkt, uint16_t sequence_num) {
        
        std::cout << "--------------------------------------------" << std::endl;
#ifdef STREAMRAWPACKET
        std::cout << std::hex;
        std::cout << "Raw packet = ";
        for (uint32_t i = 0; i < pkt.size(); i++) {
            std::cout << " "
            << std::hex
            << std::setfill('0')
            << std::setw(2)
            << (int)pkt[i];
        }
        std::cout << std::endl;
#endif
        
        std::cout << std::dec;
        std::cout << "Seqnum = " << sequence_num << std::endl;
        for (int i = 0; i < pkt.size(); i++)
        {
            adxl_stream_cb_data_t *pAdxlItem = &pkt[i];
            std::cout << "TS, x, y, z = "
            << pAdxlItem->timestamp << ", "
            << pAdxlItem->x << ", "
            << pAdxlItem->y << ", "
            << pAdxlItem->z
            << std::endl;
            x_value = pAdxlItem->x;
            y_value = pAdxlItem->y;
            z_value = pAdxlItem->z;

            
            [[NSNotificationCenter defaultCenter]postNotificationName:@"UpdateADXL" object:nil];
            
        }
        std::cout << "--------------------------------------------" << std::endl;
    };
    
};

class EcgStreamCallback : public ecg_stream_callback{
public:
    void call(std::vector<ecg_stream_cb_data_t>pkt, uint16_t sequence_num) {
        
        std::cout << "--------------------------------------------" << std::endl;

        
        std::cout << std::dec;
        std::cout << "Seqnum = " << sequence_num << std::endl;
        for (int i = 0; i < pkt.size(); i++)
        {
            ecg_stream_cb_data_t *pECGItem = &pkt[i];
            std::cout << "TS, x, y, z = "
            << pECGItem->timestamp << ", "
            << pECGItem->ecg_data << ", "
            << pECGItem->HR << ", "
            << pECGItem->leadsoff
            << pECGItem->datatype
            << std::endl;
            
            
            ecg_data_Value = pECGItem->ecg_data;
            ECGHRVALUE = pECGItem->HR;
            
            if(pECGItem->leadsoff)
            {
                ECGleadsoff = @"ON";
            }
            else
            {
                ECGleadsoff = @"OFF";
            }
            
            
            if(pECGItem->datatype  == ecg_sport)
            {
                ECGTYpe = @"SPORT";
            }
            else
            {
                ECGTYpe = @"MONITOR";
            }        
            
            [[NSNotificationCenter defaultCenter]postNotificationName:@"UpdateECG" object:nil];
            
        }
        
        
        
    };

};

/**
 Initializing Callback to Write the data to BLE Layer

 */
phy_cb *g_phy_callback = new phy_cb();


/**
 Initializing Callback for Watch

 @param g_phy_callback defines the callback to write the data to BLE
 */
watch g_mw = watch(*g_phy_callback);


/**
 Set Platorm as ios in SDK
 */
-(void)initializeSDK
{
  
  g_mw.initialize_sdk(g_mw.ios);
  
  //set max packet count to 1 to receie single packet as a response
  g_mw.pm_app.setMaxPktCombineCnt(1);
  
}

/**
 Dispatching packet that we received from the watch to the Library

 @param receivedbyte Receiving Packet from the BLE Layer
 */
-(void)Dispatchpacket : (NSMutableData *)receivedbyte
{
    std::vector<uint8_t> g_received_pkt;

    Byte payloadadpdbuffer[receivedbyte.length];
    [receivedbyte getBytes:&payloadadpdbuffer length:receivedbyte.length];
    for (int i = 0; i < receivedbyte.length; i++) {
        g_received_pkt.push_back(payloadadpdbuffer[i]);
        std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << (int) g_received_pkt[i];
    }
  if(g_received_pkt.size() != 0)
  {
    g_mw.dispatch(g_received_pkt);

  }
    
}


/**
 Initializing Callback to receive Accelarometer from the library

 @return Returns the ADXL data containing timestamp,xValue,Yvalue,Zvalue
 */
AdxlStreamCallback adxl_stream_callback = AdxlStreamCallback();


/**
 Calling Accelerometer Stream to start from Wrapper to library

 */
-(void)StartADXL
{
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
  g_mw.adxl_app.adxl_stream.start();
  g_mw.adxl_app.adxl_stream.subscribe(adxl_stream_callback);
  });
}

/**
 Calling Accelerometer Stream to start from Wrapper to library

 */
-(void)StopADXL
{
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
  g_mw.adxl_app.adxl_stream.unsubscribe();
  g_mw.adxl_app.adxl_stream.stop();
  });
}

EcgStreamCallback ecgStreamCallback = EcgStreamCallback();

/**
Calling ECG Stream to start from Wrapper to library
 */
-(void)StartECG
{
  //start ecg using L2 API
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
  g_mw.start_ecg(ecgStreamCallback);
  });
}


/**
 Calling ECG Stream to start from Wrapper to library
 */
-(void)StopECG
{
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    g_mw.stop_ecg();
  });
}

@end
