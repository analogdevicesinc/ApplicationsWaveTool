"""
/******************************************************************************
* Copyright (c) 2019 Analog Devices, Inc.  All rights reserved. 
*  
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met:   
* - Redistributions of source code must retain the above copyright notice, this 
*   list of conditions and the following disclaimer.   
* - Redistributions in binary form must reproduce the above copyright notice, 
*   this list of conditions and the following disclaimer in the documentation 
*   and/or other materials provided with the distribution.     
* - Modified versions of the software must be conspicuously marked as such.   
* - This software is licensed solely and exclusively for use with 
*   processors/products manufactured by or for Analog Devices, Inc.   
* - This software may not be combined or merged with other code in any manner 
*   that would cause the software to become subject to terms and conditions 
*   which differ from those listed here.   
* - Neither the name of Analog Devices, Inc. nor the names of its contributors 
*   may be used to endorse or promote products derived from this software 
*   without specific prior written permission.   
* - The use of this software may or may not infringe the patent rights of one 
*   or more patent holders.  This license does not release you from the 
*   requirement that you obtain separate licenses from these patent holders to 
*   use this software. 
* 
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
* NONINFRINGEMENT, TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
* ARE DISCLAIMED. IN NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE 
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR 
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF 
* CLAIMS OF INTELLECTUAL PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF 
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
* POSSIBILITY OF SUCH DAMAGE. 
******************************************************************************/

"""
import sys
import socket
import time
import awt_module as awtmod
import datetime

#***************************
#
# Before running this example2.py script, 
# 1. Launch the Application Wavetool with ADPD4100 sensor connected to PC's serial port 
# 2. Mention that COM port number to COM_PORT variable
#
# Then running this script will control the Application wavetool to 
# receive sensor data and print it in console and also log that data in text file.
#
#***************************

UDP_RX_PORT = 50002
UDP_TX_PORT = 50001
DATA_TXPORT = 50007

COM_PORT = 3   #Replace with the COM port you are using
DCFG_FILE = 'ADPD4100_PPG_Default.dcfg' #Replace with the dcfg of your choice 
                                        #but the dcfg should be in the Wavetool cfg folder

Host_IP = "localhost"
AppVersion = 1.0
TIME_RUN_SECONDS = 20
TEST_SAMPLE_APPLN = True
DATAFILENAME = "udpdatafile"
UDP_ENABLE = 1
UDP_DISABLE = 0
LOG_ENABLE = 1
LOG_DISABLE = 0
ADPD_REG_OPMODECFG_ADDR = 0x10
ADPD_REG_CHIPID_ADDR = 0x08
ADPD_REG_OPMODECFG_VAL = 0x0
FRAMESZ = 32

def udp_control_init():
    awtcontrInst = awtmod.awt_controller_class()    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    awtcontrInst.opensock(sock, Host_IP, UDP_TX_PORT, UDP_RX_PORT)
    return awtcontrInst

def run_sample_controller_appln(awtcontrInst):
    awtcontrInst.connect(COM_PORT)
    parse_response(awtcontrInst)
    if awtcontrInst.responseStatus == awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_SUCCESS:
        print ("Device Connected successfully")
        time.sleep(5)
        print ("Opening ADPD View")
        awtcontrInst.listview()
        parse_response(awtcontrInst)
        awtcontrInst.openview(1)
        parse_response(awtcontrInst)
        if awtcontrInst.responseStatus == awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_SUCCESS:
            print ("Modifying OPMODE Register")
            awtcontrInst.regwrite(ADPD_REG_OPMODECFG_ADDR, ADPD_REG_OPMODECFG_VAL)    
            parse_response(awtcontrInst)
            print ("Reading OPMODE Register")
            awtcontrInst.regread(ADPD_REG_OPMODECFG_ADDR)  
            parse_response(awtcontrInst)
            print ("Reading Chip Register")
            awtcontrInst.regread(ADPD_REG_CHIPID_ADDR)  
            parse_response(awtcontrInst)
            time.sleep(3)
            print ("\nAvailable DCFG File\n")
            awtcontrInst.listDCFGs()
            parse_response(awtcontrInst)
            time.sleep(2)
            print ("\nLoading the DCFG File " + DCFG_FILE + " \n")
            awtcontrInst.loadDCFG(0, DCFG_FILE)
            parse_response(awtcontrInst)
            print ("Selecting Plot 1 with Slot A")
            awtcontrInst.slot_plot_select(1, 1)
            parse_response(awtcontrInst)
            print ("Selecting Plot 2 with Slot B")
            awtcontrInst.slot_plot_select(2, 2)
            parse_response(awtcontrInst)
            print ("\nPlaying the data from the sensor for " + str(TIME_RUN_SECONDS) + " seconds \n")
            awtcontrInst.play()
            parse_response(awtcontrInst)
            if awtcontrInst.responseStatus == awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_SUCCESS:
                print ("\nEnabling the Log and UDP Data transfer for " + str(TIME_RUN_SECONDS) + " seconds \n")
                awtcontrInst.start_log_xfer(LOG_ENABLE)
                parse_response(awtcontrInst)
                awtcontrInst.start_udp_xfer(UDP_ENABLE)
                parse_response(awtcontrInst)
                if awtcontrInst.responseStatus == awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_SUCCESS:
                    run_udptransfer_data()
                print("\nDisabling the Log \n")
                awtcontrInst.start_log_xfer(LOG_DISABLE)
                parse_response(awtcontrInst)
                print ("\nDisabling the UDP Data transfer\n")
                awtcontrInst.start_udp_xfer(UDP_DISABLE)
                parse_response(awtcontrInst)    
                print ("\nStopping the Playback of the ADPD sensor data \n")
                awtcontrInst.stop()
                parse_response(awtcontrInst)
                print ("\nDisconnecting the device \n")
            awtcontrInst.disconnect()
            parse_response(awtcontrInst)
            awtcontrInst.closesock()
    else:
        awtcontrInst.closesock()
        
def run_udptransfer_data():
    '''
    Initiate the UDP Transfer class and start data transfer
    '''
    awtTransferInst = awtmod.awt_transfer_class(FRAMESZ)
    filemode = "w"; # default mode as 'write'
    
    # Open socket 
    try:            
        
        status = awtTransferInst.opensock(DATA_TXPORT)

        print ("UDP connection status :" + str(status))
    except:
        awtTransferInst.sock = ""

    if(awtTransferInst.sock == ""):
        print ("Exiting the application socket block")
        time.sleep(1);
        sys.stdin.close();
        sys.exit(0);
    else:
        print ("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
        print ("\t\t\tUDP TRANSFER TOOL\n")
        print ("UDP Port Number: 50007")
        print ("Version        : " + str(AppVersion))
        print ("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")                     
         # Open files to store data and error
        try:
            currentDT = datetime.datetime.now()

            filename = DATAFILENAME + "_" + (currentDT.strftime("%Y_%m_%d_%H_%M_%S")) + ".txt"
            file = open(filename,filemode);
        except:
            file = ""
            logstr = "Data File open Error : "+str(sys.exc_info())
            print(logstr)
        
        grphInst = awtTransferInst.graphdata_inst
        adpd4000bufferInst = grphInst.adpd4000_inst
        t_end = time.time() + TIME_RUN_SECONDS
        while time.time() < t_end:    
            awtTransferInst.process()                        
            adpd4000bufferInst.CheckBufferReady()
            if (adpd4000bufferInst.isBuffReady[0]):
              print(awtTransferInst.graphdata_inst.adpd4000_inst.Slot_DataArr[0][1]);
            if awtTransferInst.seqdatastr != "":
                file.write(awtTransferInst.seqdatastr);
            
        awtTransferInst.closesock();
        file.close()
        
def parse_response(awtcontrInst):        
        if( awtcontrInst.requestopt == awtmod.awt_udp_response_ENUM_t.UDP_LISTCONFIG_RESPONSE_ID):
            print (awtcontrInst.responsedata)
            print ("Enter load config command with any of above choice as shown below..\n")
            print ("\tEg.,0D 01 <01/02/03...>choice\n")
            print ("Response Data: ",awtcontrInst.response)
        elif awtcontrInst.requestopt == awtmod.awt_udp_response_ENUM_t.UDP_READREG_RESPONSE_ID:
            print ("Response Data: ",awtcontrInst.response,awtcontrInst.responsedata)
            print ("Register Value is ", awtcontrInst.responsedata)
        elif awtcontrInst.requestopt == awtmod.awt_udp_response_ENUM_t.UDP_LISTVIEW_RESPONSE_ID:
            print ("Enter OpenView command with any of above choice as shown below..\n")
            print (awtcontrInst.responsedata)
        else:
            print ("Response Data: ",awtcontrInst.response)
                      
        if(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_SUCCESS):
           print ("Response Status: Success")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_FAILED): 
          print ("Response Status: Failure")           
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_NOVIEW_AVAILABLE): 
          print ("Response Status: No View is available")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_PLAY_FAILED): 
          print ("Response Status: Cannot Play, Already in Play state")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_STOP_FAILED):                   
          print ("Response Status: Cannot Stop, Already in Stop state ")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_CALIBRATE_FAILED):
          print ("Response Status: Cannot Calibrate, Not in Stop state") 
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_RESET_REQUIRED_FOR_CALIB): 
          print ("Response Status: Reset is must for clock calibration")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_NOT_APPLICABLE): 
          print ("Response Status: Not Applicable")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_ALREADY_CONNECTED): 
          print ("Response Status: Invalid Command")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_INVALID_COMMAND): 
          print ("Response Status: Already Connected")           
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_OPENVIEW_FAILED): 
          print ("Response Status: Open Failed or View Already Opened")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_LOADCONFIG_FAILED): 
          print ("Response Status: Load Configuration Failed")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_INVALID_PARAMETER): 
          print ("Response Status: Invalid Parameter")           
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_VIEWSPLUGIN_NOTFOUND_OR_INVALID):
          print ("Response Status: Views Plugin removed or not found")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_CONFIGFILE_NOTFOUND_OR_INVALID):
          print ("Response Status: Load Configuration file not found or Invalid")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_DEVICE_NOTCONNECTED_FAILURE):
          print ("Response Status: Failed..Please Connect the device.")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_NO_CONFIGFILES_AVAILABLE):
          print ("Response Status: Failed..No Config files available")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_REG_OPERATION_FAILED_NO_DEVICE_CONNECTED):
          print ("Response Status: Failed..Connect device to Write/Read Register")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_DEVICE_ALREADY_DISCONNECTED): 
          print ("Response Status: Already Disconnected")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_LOADCONFIG_FAILED_DATAPLAYBACK_ON):
          print ("Response Status: Load Configuration Failed, Not in Stop State")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_CONNECTION_FAILED_HARDWARE_NOT_FOUND):
          print ("Response Status: Connection Failed, Target Hardware Not Found")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_CANNOT_ON_RAWDATA):
          print ("Response Status: Cannot ON the Raw data, Already in ON state")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_CANNOT_OFF_RAWDATA):
          print ("Response Status: Cannot OFF the Raw data, Already in OFF state")
        elif(awtcontrInst.responseStatus==awtmod.awt_udp_response_ENUM_t.UDP_RESPONSE_LISTCONFIGFILES_USE_LOADCONFIG):
          print ("Response Status: List the Configurations and use Load Config")


if __name__ == "__main__":
    awtcontrInst = udp_control_init()
    run_sample_controller_appln(awtcontrInst)

  

