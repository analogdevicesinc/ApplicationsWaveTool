/**
 * License Agreement
 * Copyright (c) 2017 Analog Devices Inc.All rights reserved.
 * This source code is intended for the recipient only under the guidelines of the non-disclosure agreement with Analog Devices Inc.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

import UIKit
import CoreBluetooth
let BLEWrapper = gen4sdkBLE();
let Wrapper = gen4sdkWrapper();
var peripheralarray : NSMutableArray!
var ConnectedDeviceName : String! = "";

/// Description BLEScancontroller is GUI which scan and display the available ADI BLE devices in a table view.
class BLEScanController: UITableViewController {
 
    override func viewDidLoad() {
        super.viewDidLoad()
        
        
       Timer.scheduledTimer(timeInterval: 0.35, target: self, selector: #selector(UpdateTableView), userInfo: nil, repeats: true)

        /// Refresh button to refresh the devices in  the list
        let button1 = UIBarButtonItem(image: UIImage(named: "Refresh"), style: .plain, target: self, action:#selector(RefreshData)) 
        self.navigationItem.rightBarButtonItem  = button1
        
        if !(peripheralarray != nil) {
            peripheralarray = NSMutableArray()
        }
        tableView.delegate = self;
        print("peripheral",peripheralarray.count)
      
      NotificationCenter.default.addObserver(self,
                                             selector: #selector(PushToNextPage),
                                             name: NSNotification.Name(rawValue: "updateBLEConnection"),
                                             object: nil)
      

        
        
    }
    
    /// Description Scan the all available ADI BLE devices after GUI appears.
    ///
    /// - Parameter animated: animated description
    override func viewDidAppear(_ animated: Bool) {
        if (peripheralarray != nil)
        {
            peripheralarray.removeAllObjects()
            self.tableView.reloadData()
            BLEWrapper.scanDevices()
            ;
        }
    }
    
   
    /// <#Description#> Reloads the ADI BLE devices displaying in rows and sections of the table view.
  @objc func RefreshData() {
        
        peripheralarray.removeAllObjects()
        BLEWrapper.scanDevices()
        DispatchQueue.main.async(execute: {
            
            self.tableView.reloadData()
            
        })

    }
    
    /// <#Description#> Upating the list of available BLE devices rows and sections of the table view.
  @objc func UpdateTableView(){

        let array = BLEWrapper.listDownPeripheral()
        if array?.count != peripheralarray.count {
            peripheralarray.removeAllObjects();
            peripheralarray.setArray(array!);

        }
        
        DispatchQueue.main.async(execute: {
            
            self.tableView.reloadData()
            
        })
        
    }
  
  /// <#Description#> Upating the list of available BLE devices rows and sections of the table view.
@objc func PushToNextPage(){
  DispatchQueue.main.async(execute: {
    let storyBoard : UIStoryboard = UIStoryboard(name: "Main", bundle:nil)
    
    let nextViewController = storyBoard.instantiateViewController(withIdentifier: "ViewController") as! ViewController
    self.navigationController?.pushViewController(nextViewController, animated: true)
  })
}
    

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source

    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return peripheralarray.count
    }

/// Description Used to display the updated ADI BLE device names in the table cell.
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "LabelCell", for: indexPath)
        let peripheral: CBPeripheral  =  peripheralarray.object(at: indexPath.row) as! CBPeripheral
        
        cell.textLabel?.text = peripheral.name
       self.tableView.separatorInset.right = 0
        cell.separatorInset.right = 0
        self.tableView.separatorInset.left = 0
        cell.separatorInset.left = 0
        // Configure the cell...

        return cell
    }
   
    override func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 10.0;
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        
        let peripheral: CBPeripheral  =  peripheralarray.object(at: indexPath.row) as! CBPeripheral
        
        ConnectedDeviceName = peripheral.name
        
        BLEWrapper.connectperipheral(indexPath.row);
      
       
    }


}
