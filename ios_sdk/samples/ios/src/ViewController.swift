/**
 * License Agreement
 * Copyright (c) 2017 Analog Devices Inc.All rights reserved.
 * This source code is intended for the recipient only under the guidelines of the non-disclosure agreement with Analog Devices Inc.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

import UIKit


/// <#Description#> This GUI page provides the infrastructure for managing the views used for all stream start/stop activities and display datas.
@objc class ViewController: UIViewController {
    
    
    @IBOutlet var devicename: UILabel!
    @IBOutlet var SetOutlet: UIButton!
    @IBOutlet var GetOutlet: UIButton!
    @IBOutlet var StartECGOutlet: UIButton!
    @IBOutlet var devicestatus: UILabel!
    @IBOutlet var Acc_X: UILabel?
    @IBOutlet var Acc_Y: UILabel?
    @IBOutlet var Acc_Z: UILabel?
    @IBOutlet var greentextfield: UITextField!
    @IBOutlet var StartButtonOutlet: UIButton!
    @IBOutlet var redTextfield: UITextField!
    
    @IBOutlet var ECGStatus: UILabel!
    @IBOutlet var ECGType: UILabel!
    @IBOutlet var ECGHR: UILabel!
    @IBOutlet var ECGDATA: UILabel!
    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.navigationItem.setHidesBackButton(true, animated:true);
        
        devicestatus.text = "Status             : Connected                 ";
        devicename.text = "DeviceName : " + ConnectedDeviceName;

      self.StartButtonOutlet.setTitle("Start", for: UIControl.State.normal)
        StartButtonOutlet.layer.borderWidth = 1.0;
        StartButtonOutlet.layer.borderColor = UIColor.blue.cgColor

        
      self.StartECGOutlet.setTitle("Start", for: UIControl.State.normal)
        StartECGOutlet.layer.borderWidth = 1.0;
        StartECGOutlet.layer.borderColor = UIColor.blue.cgColor

   
        //////////////////////ADD Refresh button///////////
        
        let button1 = UIBarButtonItem(title: "Disconnect", style: .plain, target: self, action:#selector(DisconnectWatch)) // action:#selector(Class.MethodName) for swift 3
       
        
        self.navigationItem.rightBarButtonItem  = button1
        
        // Do any additional setup after loading the view, typically from a nib.
    }

  override func viewDidAppear(_ animated: Bool) {
    Wrapper.initializeSDK()
  }
    
    
    
    /// <#Description#> To disconnect ADI BLE device and updates the display.
  @objc func DisconnectWatch()  {
        BLEWrapper.disconnectDevice();
        
        self.navigationController?.popViewController(animated: true);

    }
    
    /// <#Description#> Start ADXL button onclick event handler.
    ///
    /// - Parameter sender: <#sender description#>
    @IBAction func StartADXl(_ sender: Any) {
        
      if StartButtonOutlet.title(for: UIControl.State.normal) == "Start" {
           //Invokes startADXL command of Gen4 SDK library.
        NotificationCenter.default.addObserver(self,
                                                                                           selector: #selector(UPdateADXL),
                                               name: NSNotification.Name(rawValue: "UpdateADXL"),
                                               object: nil)
            Wrapper.startADXL();

            DispatchQueue.main.async(execute: {
              self.StartButtonOutlet.setTitle("Stop", for: UIControl.State.normal)
            })
        

        }
        else
        {
            //Invokes stopADXL command of Gen4 SDK library.
          NotificationCenter.default.removeObserver(self, name: NSNotification.Name(rawValue: "UpdateADXL"), object: nil)
            Wrapper.stopADXL();

            DispatchQueue.main.async(execute: {
              self.StartButtonOutlet.setTitle("Start", for: UIControl.State.normal)
                self.Acc_X?.text = "X :        _ ";
                self.Acc_Y?.text = "Y :        _ ";
                self.Acc_Z?.text = "Z :        _ ";


            })
          
        }

    }
    
    /// <#Description#> Start ECG button onclick event handler.
    ///
    /// - Parameter sender: <#sender description#>
    @IBAction func StartECGClicked(_ sender: Any) {
      if StartECGOutlet.title(for: UIControl.State.normal) == "Start" {
            //Invokes start ECG command of Gen4 SDK library.
        
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(UPdateECG),
                                               name: NSNotification.Name(rawValue: "UpdateECG"),
                                               object: nil)
        
          Wrapper.startECG();
          
          DispatchQueue.main.async(execute: {
            self.StartECGOutlet.setTitle("Stop", for: UIControl.State.normal)
          })
        
          
        }
        else
        {
          NotificationCenter.default.removeObserver(self, name: NSNotification.Name(rawValue: "UpdateECG"), object: nil)

            //Invokes stop ECG command of Gen4 SDK library.
            Wrapper.stopECG();
            
            DispatchQueue.main.async(execute: {
              self.StartECGOutlet.setTitle("Start", for: UIControl.State.normal)
                self.ECGDATA?.text =  "ECG DATA : _ ";
                self.ECGType?.text =  "TYPE : _ ";
                self.ECGHR.text =     "HR : _ ";
                self.ECGStatus.text = "LEAD Status: _ ";
                
            })
          }
        

    }

 
    /// <#Description#> This callback method receives command packets from native Gen4 SDK and sends to ADI BLE devices.
    ///
    /// - Parameter data: <#data description#>
    @objc func CallbackObjectiveCdata(data : NSData) {
        // Write BLE data through objective BLE wrapper
      BLEWrapper.writedata(data as Data?);
   
    }
     
    
    /// <#Description#> Update the view with Accelerometer x,y,z values
  @objc func UPdateADXL()  {
        
        DispatchQueue.main.async(execute: {
            self.Acc_X?.text = "X :   \(x_value) ";
            self.Acc_Y?.text = "Y :   \(y_value) ";
            self.Acc_Z?.text = "Z :   \(z_value) ";
        })
        
    }
    
    /// <#Description#>Update the view with ECG data,type,heart rate and lead status.
  @objc func UPdateECG()  {
        
        DispatchQueue.main.async(execute: {
            self.ECGDATA?.text =  "ECG DATA : \(ecg_data_Value) ";
            self.ECGType?.text =  "TYPE : "+ECGTYpe;
            self.ECGHR.text =     "HR : \(ECGHRVALUE)";
            self.ECGStatus.text = "LEAD Status :"+ECGleadsoff;
            
        })
    }
    
 }


