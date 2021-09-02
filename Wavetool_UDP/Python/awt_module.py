# -*- coding: utf-8 -*-
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
from ctypes import *

import DataParser.M2M2DataParserManager as m2m2_parser
import DataParser.GraphDataParser as graphData_parser

class awt_transfer_class():
    singleton = None

    def __new__(cls, *args, **kwargs):
        if not cls.singleton:
            cls.singleton = object.__new__(awt_transfer_class)
            cls.m2m2parser_inst = m2m2_parser.M2M2DataParserClass()
            cls.graphdata_inst = graphData_parser.GraphDataParser(args[0])
            cls.sock = ''
            cls.sync = [0xA0, 0xB0, 0xC0, 0xD0, 0xE0]
        return cls.singleton

    def opensock(self, Port=50007):
        IP = "0.0.0.0"
        status= awt_udp_response_ENUM_t.UDP_RESPONSE_SUCCESS
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
            self.sock.bind((IP, Port))
        except:
            print("socket Bind Error "+str(sys.exc_info()))
            self.sock = ''
            status = awt_udp_response_ENUM_t.UDP_TRANSFER_SOCKBIND_FAILURE
        return status
        
    def closesock(self):
        self.sock.close()
        
    def _rx_pkts_udp(self, sock, Size = 200):
        recv_data = ""
        try:
            recv_data, recv_addr = sock.recvfrom(Size)
        except socket.timeout:
            logstr = "socket Receive time out  "+str(sys.exc_info()) 
            print (logstr)
            sock = ''
            return sock
        except :
            logstr = "Socket Receive Error "+str(sys.exc_info())
            print (logstr)
            return recv_data
        return recv_data

    def process(self):
        '''
        Function to convert the data from Wavetool to double values 
        to be stored in a log file or used for further processing
        '''
        Size = 500
        ReceivedPkts = self._rx_pkts_udp(self.sock, Size)

        if len(ReceivedPkts) > 0:
            view_sync = ReceivedPkts[0]
            self.seqdatastr = None
            if view_sync in self.sync:
                self.seqdatastr = self.graphdata_inst.parse_GraphData(ReceivedPkts)
                #plotter

                #sys.stdout.write("\rReceived Packet Count: " + str(self.graphdata_inst.samplecount))
            else:
                if(ReceivedPkts != ""):
                    self.m2m2datastr = self.m2m2parser_inst.m2m2_data_parser(ReceivedPkts)
                    self.status = "Received M2M2 Packet"
    
class awt_controller_class():
    
    def __init__(self):

        self.response = ""
        self.responseStatus = awt_udp_response_ENUM_t.UDP_RESPONSE_SUCCESS
        self.responsedata = ""
        self.dcfgdict = {}
        self.cloudDcfgDict = {}
        self.viewdict = {}
        self.command = ""
        self.requestopt = 0
        self.ipaddr = '0.0.0.0'
        self.txportnum = 50001
        self.rxportnum = 50002
        self.sock = ''
        
    def opensock(self, sock, IPaddr = 'localhost', TxPortNum = 50001, RxPortNum = 50002):
        try:
            self.txportnum = TxPortNum
            self.rxportnum = RxPortNum
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.sock.bind((self.ipaddr, self.rxportnum))
            self.sock.setblocking(0)
            self.sock.settimeout(60)
            self.ipaddr = IPaddr
        except:
            self.responseStatus = awt_udp_response_ENUM_t.UDP_CONTROLLER_SOCKBIND_FAILURE
            
    def closesock(self):
        self.sock.close()
        
    def _send_command_udp(self):
        size = self.sock.sendto(self.command, (self.ipaddr, self.txportnum));
        if size!= len(self.command) :
            self.responseStatus = awt_udp_response_ENUM_t.UDP_RESPONSE_SEND_FAILED
            
    def _rx_pkts_udp(self):
        recv_data = ""      # to return string object by default even if recvfrom is timedout and not returned any object/string for rec_data

        try:
            recv_data, recv_addr = self.sock.recvfrom(8192)    # size <- if size < UDP Packet size, only the partial data copied and rest of data in UDP packet will be ignored/lost

        except socket.timeout:
            self.responseStatus = awt_udp_response_ENUM_t.UDP_RESPONSE_FAILED

        return recv_data
        
    def regread(self, regaddr):
        '''
        The Controller Request API for Read register operation. 
        This function will:
            1. Create the message to READ register
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
            5. Register value is stored in self.responsedata
        '''
        str_array = bytearray(4)
        str_array[0] = int("00", 16)
        str_array[1] = int("02", 16)
        str_array[2] = regaddr.to_bytes(2, byteorder='big')[0]
        str_array[3] = regaddr.to_bytes(2, byteorder='big')[1]
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)
        
    def regwrite(self, regaddr, regval):
        '''
        The Controller Request Message Builder for Write register operation. 
        This function will:
            1. Create the message to WRITE register
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
            5. regaddr is one byte and regval is 2 bytes
        '''

        str_array = bytearray(6)
        str_array[0] = int("01", 16)
        str_array[1] = int("04", 16)
        str_array[2] = regaddr.to_bytes(2, byteorder='big')[0]
        str_array[3] = regaddr.to_bytes(2, byteorder='big')[1]
        str_array[4] = regval.to_bytes(2, byteorder='big')[0]
        str_array[5] = regval.to_bytes(2, byteorder='big')[1]
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)
        
    def play(self):
        '''
        The Controller Request API for Play ADPD sensor operation. 
        This function will:
            1. Create the message to Play ADPD Sensor data
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(2)
        str_array[0] = int("02", 16)
        str_array[1] = int("00", 16)
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def stop(self):
        '''
        The Controller Request API for Stop ADPD sensor operation. 
        This function will:
            1. Create the message to Stop ADPD Sensor data
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(2)
        str_array[0] = int("03", 16)
        str_array[1] = int("00", 16)
        self.command = bytes(memoryview(str_array))     
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)
        
    def start_udp_xfer(self, isEnable = 0):
        '''
        The Controller Request API for start the UDP Transfer operation.
        This function will:
            1. Create the message to start or stop data transfer
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(3)
        str_array[0] = int("06", 16)
        str_array[1] = int("01", 16)
        str_array[2] = isEnable
        self.command = bytes(memoryview(str_array))     
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def start_log_xfer(self, isEnable = 0):
        '''
        The Controller Request API for enable the Log operation.
        This function will:
            1. Create the message to start or stop data transfer
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(3)
        str_array[0] = int("09", 16)
        str_array[1] = int("01", 16)
        str_array[2] = isEnable
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def slot_plot_select(self, plot, slot):
        '''
        The Controller Request API for select the slot for the plotting.
        This function will:
            1. Create the message to select the slot data for a plot
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(4)
        str_array[0] = int("10", 16)
        str_array[1] = int("02", 16)
        str_array[2] = plot
        str_array[3] = slot
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def m2m2PacketEnable(self, m2m2Enable):
        '''
        The Controller Request API for enabling the M2M2 packet.
        This function will:
            1. Create the message to enable or disable M2M2 packet
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(3)
        str_array[0] = int("11", 16)
        str_array[1] = int("01", 16)
        str_array[2] = m2m2Enable
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def tabView(self, view):
        '''
        The Controller Request API for Tab View Select.
        This function will:
            1. Create the message to reset software
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''      
        str_array = bytearray(3)
        str_array[0] = int("13", 16)
        str_array[1] = int("01", 16)
        str_array[2] = view
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def softwareReset(self):
        '''
        The Controller Request API for software Reset.
        This function will:
            1. Create the message to reset software
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(2)
        str_array[0] = int("12", 16)
        str_array[1] = int("00", 16)
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)
        
    def connect(self, portnum):
        '''
        The Controller Request API for connecting the sensor board with the COM port given.
        This function will:
            1. Create the message to connect the ADPD sensor board
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(3)
        str_array[0] = int("0A", 16)
        str_array[1] = int("01", 16)
        str_array[2] = portnum
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def disconnect(self):
        '''
        The Controller Request API for disconnecting the sensor board. 
        This function will:
            1. Create the message to disconnect the ADPD sensor board
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(2)
        str_array[0] = int("0B", 16)
        str_array[1] = int("00", 16)
        self.command = bytes(memoryview(str_array))  
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)
        
    def listview(self, view = 0):
        '''
        The Controller Request API for listing the availale views.
        This function will:
            1. Create the message for opening the ADPD View
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(2)
        str_array[0] = int("0C", 16)
        str_array[1] = int("00", 16)
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def openview(self, viewsel=0, view=""):
        '''
        The Controller Request API for opening the ADPD View. 
        This function will:
            1. Create the message for opening the ADPD View
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(3)
        str_array[0] = int("0C", 16)
        str_array[1] = int("01", 16)
        if(view ==""):
            str_array[2] = viewsel
        else:
            try:
                viewsel = self.dcfgdict[view]
                str_array[2] = viewsel
            except:
                self.responseStatus = awt_udp_response_ENUM_t.UDP_RESPONSE_VIEWSPLUGIN_NOTFOUND_OR_INVALID
                return
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

            
    def listDCFGs(self):
        '''
        The Controller Request API for listing the available DCFG files. 
        This function will:
            1. Create the message for listing the available DCFG files
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
            5. The DCFG list is stored in a Dictionary and in 
               string format to be displayed to the user in console
        '''
        str_array = bytearray(2)
        str_array[0] = int("0D", 16)
        str_array[1] = int("00", 16)
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def loadDCFG(self, dcfgsel=0, dcfgfile=""):
        '''
        The Controller Request API for loading the selected DCFG file. 
        The selection can be either through the number corresponding to the file
        or by the dcfg filename itself.
        This function will:
            1. Create the message for opening the ADPD View
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
            5. the DCFG list is stored in a Dictionary and in 
               string format to be displayed to the user in console
        '''
        str_array = bytearray(3)
        str_array[0] = int("0D", 16)
        str_array[1] = int("01", 16)
        if(dcfgfile==""):
            str_array[2] = dcfgsel
        else:
            try:
                dcfgsel = self.dcfgdict[dcfgfile]
                str_array[2] = dcfgsel
            except:
                self.responseStatus = awt_udp_response_ENUM_t.UDP_RESPONSE_CONFIGFILE_NOTFOUND_OR_INVALID
                return
        self.command = bytes(memoryview(str_array)) 
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def listCloudDCFGs(self):
        '''
        The Controller Request API for listing the available DCFG files. 
        This function will:
            1. Create the message for listing the available DCFG files
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
            5. The DCFG list is stored in a Dictionary and in 
               string format to be displayed to the user in console
        '''
        str_array = bytearray(2)
        str_array[0] = int("14", 16)
        str_array[1] = int("00", 16)
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def loadCloudDCFG(self, cloudDcfgsel=0, cloudDcfgfile=""):
        '''
        The Controller Request API for loading the selected DCFG file. 
        The selection can be either through the number corresponding to the file
        or by the dcfg filename itself.
        This function will:
            1. Create the message for opening the ADPD View
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
            5. the DCFG list is stored in a Dictionary and in 
               string format to be displayed to the user in console
        '''
        str_array = bytearray(3)
        str_array[0] = int("15", 16)
        str_array[1] = int("01", 16)
        if(cloudDcfgfile==""):
            str_array[2] = cloudDcfgsel
        else:
            try:
                cloudDcfgsel = self.cloudDcfgDict[cloudDcfgfile]
                str_array[2] = cloudDcfgsel
            except:
                self.responseStatus = awt_udp_response_ENUM_t.UDP_RESPONSE_CONFIGFILE_NOTFOUND_OR_INVALID
                return
        self.command = bytes(memoryview(str_array)) 
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)

    def channel_select(self, plotNo, streamByte):
        '''
        The Controller Request API for select the Channel.
        This function will:
            1. Create the message to select the channel
            2. Send the message to the Wavetool
            3. Get the response from the Wavetool
            4. Parse the response to match the request
        '''
        str_array = bytearray(4)
        str_array[0] = int("16", 16)
        str_array[1] = int("02", 16)
        str_array[2] = plotNo
        str_array[3] = streamByte
        self.command = bytes(memoryview(str_array))
        self._send_command_udp();
        ReceivedPkts = self._rx_pkts_udp()
        self._udp_controller_response(ReceivedPkts)
    
    def _udp_controller_response(self, ReceivedPkts):
        '''
        The Controller Response Message Parser
        '''
        if len(ReceivedPkts)>0 :
           RegisterValue = [];
           Receiveddata = [];
           Receiveddata2 = [];
           Receiveddata,Receiveddata2=ReceivedPkts[:3],ReceivedPkts[3:]           

           arg1 = Receiveddata[0]
           arg2 = Receiveddata[1]
           self.requestopt = arg1
           self.response = Receiveddata
           self.responsedata = ""
           
           # If List views
           if (arg1 == awt_udp_response_ENUM_t.UDP_LISTVIEW_RESPONSE_ID):
               Receiveddata1 = Receiveddata2.decode('ascii')
               receivedfiles = Receiveddata1.split("\r\n")

               for i in range(len(receivedfiles)):
                   number = hex(i + 1)
                   self.viewdict[str(receivedfiles[i])] = int(number, 16)
                   number = number.replace(number[:2],'')
                   if (len(number) == 1):
                       number = "0" +number
                   self.responsedata = self.responsedata + ("" + number.upper() + " )" +receivedfiles[i] + "\n")

           # If List config files
           elif (arg1 == awt_udp_response_ENUM_t.UDP_LISTCONFIG_RESPONSE_ID):
               Receiveddata1 = Receiveddata2.decode('ascii')
               receivedfiles = Receiveddata1.split("\r\n")

               for i in range(len(receivedfiles)):
                   number = hex(i+1)
                   self.dcfgdict[str(receivedfiles[i])] = int(number, 16)
                   number = number.replace(number[:2],'')
                   if(len(number) == 1):
                       number = "0"+number                           
                   self.responsedata = self.responsedata + (""+number.upper()+" )"+receivedfiles[i] + "\n")

           elif (arg1 == awt_udp_response_ENUM_t.UDP_LISTCLOUDCONFIG_RESPONSE_ID):
               Receiveddata1 = Receiveddata2.decode('ascii')
               receivedfiles = Receiveddata1.split("\r\n")

               for i in range(len(receivedfiles)):
                   number = hex(i+1)
                   self.cloudDcfgDict[str(receivedfiles[i])] = int(number, 16)
                   number = number.replace(number[:2],'')
                   if(len(number) == 1):
                       number = "0"+number                           
                   self.responsedata = self.responsedata + (""+number.upper()+" )"+receivedfiles[i] + "\n")

           # If Register Read operation    
           elif (len(Receiveddata2) == 2 and arg1 == awt_udp_response_ENUM_t.UDP_READREG_RESPONSE_ID):

               RegisterValue = int.from_bytes(Receiveddata2, byteorder='big')
               #RegisterValue = hex(RegisterValue)
               self.responsedata = RegisterValue
           else:
               if len(Receiveddata2) > 0:
                   self.responsedata = Receiveddata2.decode('ascii')

           self.responseStatus = arg2
           Receiveddata = ""
           Receiveddata2 = ""
        else :
            self.responseStatus = "Received message size is zero"
            
class awt_udp_response_ENUM_t(c_ushort):
    '''
    Class defining the Enumerations for the response status.
    '''
    UDP_RESPONSE_SUCCESS = 0x0
    UDP_RESPONSE_FAILED = 0x01
    UDP_RESPONSE_SEND_FAILED = 0x02
    UDP_RESPONSE_NOVIEW_AVAILABLE = 0x04
    UDP_RESPONSE_PLAY_FAILED = 0x05
    UDP_RESPONSE_STOP_FAILED = 0x06
    UDP_RESPONSE_CALIBRATE_FAILED = 0x07
    UDP_RESPONSE_RESET_REQUIRED_FOR_CALIB = 0x08
    UDP_RESPONSE_NOT_APPLICABLE = 0x09
    UDP_RESPONSE_ALREADY_CONNECTED = 0x0A
    UDP_RESPONSE_INVALID_COMMAND = 0x0B
    UDP_RESPONSE_OPENVIEW_FAILED = 0x0C
    UDP_RESPONSE_LOADCONFIG_FAILED = 0x0D
    UDP_RESPONSE_INVALID_PARAMETER = 0x0E
    UDP_RESPONSE_LISTVIEWS_USE_OPENVIEW = 0x0F
    UDP_RESPONSE_VIEWSPLUGIN_NOTFOUND_OR_INVALID = 0xA0
    UDP_RESPONSE_CONFIGFILE_NOTFOUND_OR_INVALID = 0xB0
    UDP_RESPONSE_DEVICE_NOTCONNECTED_FAILURE = 0xC0
    UDP_RESPONSE_NO_CONFIGFILES_AVAILABLE = 0xE0
    UDP_RESPONSE_REG_OPERATION_FAILED_NO_DEVICE_CONNECTED = 0xF0
    UDP_RESPONSE_DEVICE_ALREADY_DISCONNECTED = 0x1A
    UDP_RESPONSE_LOADCONFIG_FAILED_DATAPLAYBACK_ON = 0x1B
    UDP_RESPONSE_CONNECTION_FAILED_HARDWARE_NOT_FOUND = 0x1C
    UDP_RESPONSE_CANNOT_ON_RAWDATA = 0x1E
    UDP_RESPONSE_CANNOT_OFF_RAWDATA = 0x1F
    UDP_RESPONSE_LISTCONFIGFILES_USE_LOADCONFIG = 0x2A
    UDP_RESPONSE_CANNOT_ON_LOGGING = 0x2B
    UDP_RESPONSE_CANNOT_OFF_LOGGING = 0x2C
    UDP_RESPONSE_SLOT_NOT_ENABLED = 0x2D
    UDP_RESPONSE_SLOTSELECT_FAILED_DATAPLAYBACK_ON = 0x2E
    UDP_RESPONSE_NORESET_STOP_PLAYBACK = 0x2F
    UDP_RESPONSE_FAIL_NOTSUPPORTED = 0x11
    UDP_RESPONSE_FAIL_CHANNELSELECT_PLAYMODE = 0x12
    UDP_RESPONSE_CHANNEL_NOT_ENABLED = 0x13
    
    UDP_TRANSFER_SOCKOPEN_FAILURE = 0x30
    UDP_TRANSFER_SOCKBIND_FAILURE = 0x31
    UDP_CONTROLLER_SOCKBIND_FAILURE = 0x32
    
    UDP_LISTCONFIG_RESPONSE_ID = 0x95
    UDP_READREG_RESPONSE_ID = 0x80
    UDP_DATATRANSFER_RESPONSE_ID = 0x86
    UDP_LISTVIEW_RESPONSE_ID = 0x94
    UDP_LISTCLOUDCONFIG_RESPONSE_ID = 0x98
    
    UDP_ADPDVIEW_ID = 0x40
    UDP_SMOKEVIEW_IP = 0x41


