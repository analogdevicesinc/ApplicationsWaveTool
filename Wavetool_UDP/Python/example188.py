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
# Before run this example1.py script, 
# 1. Connect the Application Wavetool with ADPD4100 sensor 
# 2. Open ADPD view 
# 3. Load the required dcfg.
# 4. Play and enable the UDP Transfer in Application wavetool 
# The example receives sensor data and prints it in console
#
#***************************


DATA_TXPORT = 50007
Host_IP = "localhost"

AppVersion = 1.0
TIME_RUN_SECONDS = 10
TEST_SAMPLE_APPLN = True
FRAMESZ = 32
UDP_ENABLE = 1


def udp_transfer_init():
    awtTransferInst = awtmod.awt_transfer_class(FRAMESZ)
    try:            
        status = awtTransferInst.opensock(DATA_TXPORT)
    except:
        awtTransferInst.sock = ""
    return awtTransferInst

        
def run_udptransfer_data(awtTransferInst):
      '''
	  Slot_DataArr[SlotNum][ChNum]
	  SlotNum: A-L :: 0:11
	  ChNum: 0 - D1
			 1 - S1
			 2 - L1
			 3 - D2
			 4 - S2
			 5 - L2
      '''
      t_end = time.time() + TIME_RUN_SECONDS
      grphInst = awtTransferInst.graphdata_inst
      while time.time() < t_end:    
          awtTransferInst.process()
          if ((grphInst.samplecount % grphInst.frameSz) == 
                            (grphInst.frameSz - 1)):
              print(awtTransferInst.graphdata_inst.SlotA_DataArr[0]);
              print(awtTransferInst.graphdata_inst.SlotB_DataArr[0]);
        


if __name__ == "__main__":
    awttransInst = udp_transfer_init()
    run_udptransfer_data(awttransInst)
    awttransInst.closesock();
