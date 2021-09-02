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


import time
import datetime
import struct
import codecs
from enum import Enum


class CoolidgeDataBuffers:
    def __init__(self, framesz):
        self.Slot_DataArr = [[[],[],[],[],[],[]],[[],[],[],[],[],[]],
                             [[],[],[],[],[],[]],[[],[],[],[],[],[]],
                             [[],[],[],[],[],[]],[[],[],[],[],[],[]],
                             [[],[],[],[],[],[]],[[],[],[],[],[],[]],
                             [[],[],[],[],[],[]],[[],[],[],[],[],[]],
                             [[],[],[],[],[],[]],[[],[],[],[],[],[]]]
        self.Slot_CntArr = [[],[],[],[],[],[],[],[],[],[],[],[]]
        self.slot_samplecount = [0,0,0,0,0,0,0,0,0,0,0,0]
        self.slot_frameSz = [framesz,framesz,framesz,framesz,framesz,framesz,framesz,framesz,framesz,framesz,framesz,framesz]
        self.isBuffReady = [False, False, False, False, False, False, False, False, False, False, False, False]
        
    def CheckBufferReady(self):
        self.isBuffReady = [False, False, False, False, False, False, False, False, False, False, False, False]
        for idx in range(len(self.isBuffReady)):
            #print(self.slot_samplecount[idx])
            if ((self.slot_samplecount[idx] % self.slot_frameSz[idx]) == 
                            (self.slot_frameSz[idx] - 1)):
                self.isBuffReady[idx] = True


class SPO2DataBuffers:
    def __init__(self, framesz):
        self.Slot_DataArr = [[[], []], [[], []],
                             [[], []], [[], []],
                             [[], []], [[], []],
                             [[], []], [[], []],
                             [[], []], [[], []],
                             [[], []], [[], []]]
        self.Slot_CntArr = [[], [], [], [], [], [], [], [], [], [], [], []]
        self.slot_samplecount = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.slot_frameSz = [framesz, framesz, framesz, framesz, framesz, framesz, framesz, framesz, framesz, framesz,
                             framesz, framesz]
        self.isBuffReady = [False, False, False, False, False, False, False, False, False, False, False, False]

    def CheckBufferReady(self):
        self.isBuffReady = [False, False, False, False, False, False, False, False, False, False, False, False]
        for idx in range(len(self.isBuffReady)):
            # print(self.slot_samplecount[idx])
            if ((self.slot_samplecount[idx] % self.slot_frameSz[idx]) ==
                    (self.slot_frameSz[idx] - 1)):
                self.isBuffReady[idx] = True

class GraphDataParser():

    def __init__(self, nFrameSz=32):
        self.printdata = True
        self.device = 0

        self.datazize = 0
        self.samplecount = 0
        self.samplecount_adxl = 0
        self.samplecount_ppg = 0
        self.samplecount_ecg = 0
        self.samplecount_eda = 0
        self.samplecount_temperature = 0
        self.samplecount_spo2 = 0
        self.status = "Success"
        self.seqdatastr = ""
        self.m2m2datastr = ""
        self.frameSz = nFrameSz
        self.framesize = 30

        self.num = 0
        self.dataarr = []
        self.slotarr = []
        self.unitarr = []
        self.offsetarr = []
        self.slotinfoarr = []
        self.channel_infoarr = []
        self.DSLinfoarr = []
        self.interptStatus = []
        self.isLeadOnInfo = []

        # ADPD/SMOKE
        self.slot_a = ""
        self.slot_b = ""
        self.SlotA_DataArr = [[], [], [], []]
        self.SlotB_DataArr = [[], [], [], []]
        self.SlotA_CntArr = []  # no need of cnt array
        self.SlotB_CntArr = []
        self.samplecntarr = []
        self.isSmokeDataReady = False

        #Sync PPG and PPG
        self.PPG_ts = []
        self.HR_DataArr = []
        self.X_DataArr = []
        self.Y_DataArr = []
        self.Z_DataArr = []
        self.PPG_DataArr = []
        self.SyncPPG_CntArr = []
        self.samplecntarr = []
        self.isPPGDataReady = False

        #Coolidge
        self.adpd4000_inst = CoolidgeDataBuffers(self.frameSz)

        #SPO2
        self.spo2_inst = SPO2DataBuffers(self.frameSz)

        #Sync ADXL
        self.ADXL_ts = []
        self.ADXL_X_DataArr = []
        self.ADXL_Y_DataArr = []
        self.ADXL_Z_DataArr = []
        self.ADXL_samplecntarr = []
        self.isADXLDataReady = False

        #Sync ECG
        self.ECG_ts = []
        self.ECG_DataArr = []
        self.ECG_HRArr = []
        self.ECG_samplecntarr = []
        self.isECGDataReady = False

        #Sync EDA
        self.EDA_ts = []
        self.EDA_ModuleArr= []
        self.EDA_PhaseArr = []
        self.EDA_samplecntarr = []
        self.isEDADataReady = False

        #Sync TEMP
        self.TEMP_ts = []
        self.TEMP_TEMP1Arr = []
        self.TEMP_TEMP2Arr = []
        self.TEMP_samplecntarr = []
        self.isTempDataReady = False

        self.header_write = False

    def parse_GraphData(self, ReceivedPkts):
        self.dataarr.clear()
        self.slotarr.clear()
        self.unitarr.clear()
        self.offsetarr.clear()
        self.slotinfoarr.clear()
        self.channel_infoarr.clear()
        self.DSLinfoarr.clear()
        self.seqdatastr = ""

        sync = ReceivedPkts[0]

        if sync == 0xA0: #ADPD,SMOKE
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            ts = ReceivedPkts[5:9]
            slot_info = ReceivedPkts[9]
            plot1_info = ReceivedPkts[10]
            plot2_info = ReceivedPkts[11]

            self.num = struct.unpack('H', seq_num)[0]

            if ((self.samplecount % self.frameSz) == 0):
                self.clear_data_buffers()

            self.samplecount = self.samplecount + 1;
            self.samplecntarr.append(self.samplecount)

            timestamp = struct.unpack('I', ts)[0]
            timestamp = self.get_time(timestamp)

            slot1 = self.getBitValues(slot_info, 0, 3)
            slot2 = self.getBitValues(slot_info, 4, 7)
            unit1 = self.getBitValues(plot1_info, 0, 1)
            offset1 = self.getBitValues(plot1_info, 4, 5)
            unit2 = self.getBitValues(plot2_info, 0, 1)
            offset2 = self.getBitValues(plot2_info, 4, 5)

            self.slot_a = self.get_slotname(slot1)
            self.slot_b = self.get_slotname(slot2)
            self.slotarr.append(self.slot_a)
            self.slotarr.append(self.slot_b)

            unit_a = self.get_unitname(unit1)
            unit_b = self.get_unitname(unit2)
            self.unitarr.append(str(unit_a))
            self.unitarr.append(str(unit_b))

            offset_a = self.get_offsetname(offset1)
            offset_b = self.get_offsetname(offset2)
            self.offsetarr.append(str(offset_a))
            self.offsetarr.append(str(offset_b))

            data1 = ReceivedPkts[12:]  # skip prefix 12
            length = int(len(data1) / 8)
            increment = 8
            startbit = 12

            if len(data1) % 8 != 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d', data)[0]
                self.dataarr.append(data)
                startbit += increment

            if not self.header_write:
                self.write_header(self.slotarr,self.unitarr,self.offsetarr)

            data_index = 0
            if(self.slot_a is not Slotsize.SLOT_OFF):
                if self.slot_a == Slot.SLOT_SUM:
                    self.SlotA_DataArr[0].append(self.dataarr[data_index])
                    self.seqdatastr += str(self.dataarr[data_index]) + "\t"
                    data_index += 1
                elif self.slot_a == Slot.SLOT_4CH:
                    for i in range(4):
                        self.SlotA_DataArr[i].append(self.dataarr[i])
                    self.seqdatastr += str(self.dataarr[0]) + "\t" + str(self.dataarr[1]) + "\t" + str(self.dataarr[2]) + "\t" + str(self.dataarr[3]) + "\t"
                    data_index += 4

            if (self.slot_b is not Slotsize.SLOT_OFF):
                if self.slot_b == Slot.SLOT_SUM:
                    self.SlotB_DataArr[0].append(self.dataarr[data_index])
                    self.seqdatastr += str(self.dataarr[data_index]) + "\t"
                elif self.slot_b == Slot.SLOT_4CH:
                    for i in range(4):
                        self.SlotB_DataArr[i].append(self.dataarr[data_index])
                        data_index += 1
                    self.seqdatastr += str(self.dataarr[data_index - 4]) + "\t" + str(self.dataarr[data_index - 3]) + "\t" + str(self.dataarr[data_index - 2]) + "\t" + str(self.dataarr[data_index - 1]) + "\t"


            self.status = "Success"
            self.seqdatastr += str(timestamp) + "\t" + str(self.num) + "\n"
            self.m2m2datastr = ""

        elif sync == 0xA1: #ADPD4000
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            ts = ReceivedPkts[5:9]
            slot_info = ReceivedPkts[9:21]
            dataCount = 0
            dataBitsToRead = 0
            # print("Received Pkts : " + ReceivedPkts)

            length = int(len(slot_info))
            for x in range(length):
                info = slot_info[x]
                DSL_info = self.getBitValues(info, 0, 5)
                for i in range(0,6):
                    dataCount += self.getBitValues(info, i , i)
                
                channel_info = self.getBitValues(info, 6, 7)
                self.slotinfoarr.append(info)
                self.channel_infoarr.append(channel_info)
                self.DSLinfoarr.append(DSL_info)
            dataBitsToRead = ((dataCount * 8) + 24)
            view_info = ReceivedPkts[21]
            plot1_info = ReceivedPkts[22]
            plot2_info = ReceivedPkts[23]

            intrpt_flag = self.getBitValues(view_info, 4, 7)
            self.num = struct.unpack('H', seq_num)[0]

            self.samplecount = self.samplecount + 1
            self.samplecntarr.append(self.samplecount)

            timestamp = struct.unpack('I', ts)[0]
            timestamp = self.get_time(timestamp)

            unit1 = self.getBitValues(plot1_info, 0, 1)
            offset1 = self.getBitValues(plot1_info, 4, 5)
            unit2 = self.getBitValues(plot2_info, 0, 1)
            offset2 = self.getBitValues(plot2_info, 4, 5)

            unit_a = self.get_unitname(unit1)
            unit_b = self.get_unitname(unit2)
            self.unitarr.append(str(unit_a))
            self.unitarr.append(str(unit_b))

            offset_a = self.get_offsetname(offset1)
            offset_b = self.get_offsetname(offset2)
            self.offsetarr.append(str(offset_a))
            self.offsetarr.append(str(offset_b))

            data1 = ReceivedPkts[24:dataBitsToRead]  # skip prefix 24
            length = int(len(data1) / 8)
            increment = 8
            startbit = 24

            if len(data1) % 8 != 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d',data)[0]
                self.dataarr.append(data)
                startbit += increment

            length = int(len(slot_info))
            idx = 0
            ndatabuffindex = 0
            for x in range(length):
                if((self.adpd4000_inst.slot_samplecount[idx] % self.adpd4000_inst.slot_frameSz[idx]) == 0):
                    self.adpd4000_inst.Slot_DataArr[idx][0] = []
                    self.adpd4000_inst.Slot_DataArr[idx][1] = []
                    self.adpd4000_inst.Slot_DataArr[idx][2] = []
                    self.adpd4000_inst.Slot_DataArr[idx][3] = []
                    self.adpd4000_inst.Slot_DataArr[idx][4] = []
                    self.adpd4000_inst.Slot_DataArr[idx][5] = []
                    self.adpd4000_inst.Slot_CntArr[idx] = []
                if(self.DSLinfoarr[idx] != 0):
                    d1_data = 0
                    d2_data = 0
                    s1_data = 0
                    s2_data = 0
                    l1_data = 0
                    l2_data = 0
                    d1_bit = self.DSLinfoarr[idx] >> 0 & 1
                    d2_bit = self.DSLinfoarr[idx] >> 1 & 1
                    s1_bit = self.DSLinfoarr[idx] >> 2 & 1
                    s2_bit = self.DSLinfoarr[idx] >> 3 & 1
                    l1_bit = self.DSLinfoarr[idx] >> 4 & 1
                    l2_bit = self.DSLinfoarr[idx] >> 5 & 1
                    if d1_bit is 1:
                        d1_data = self.dataarr[ndatabuffindex]                    
                        ndatabuffindex += 1
                    if s1_bit is 1:
                        s1_data = self.dataarr[ndatabuffindex]
                        ndatabuffindex += 1
                    if l1_bit is 1:
                        l1_data = self.dataarr[ndatabuffindex]
                        ndatabuffindex += 1
                    if d2_bit is 1:
                        d2_data = self.dataarr[ndatabuffindex]
                        ndatabuffindex += 1
                    if s2_bit is 1:
                        s2_data = self.dataarr[ndatabuffindex]
                        ndatabuffindex += 1
                    if l2_bit is 1:
                        l2_data = self.dataarr[ndatabuffindex]
                        ndatabuffindex += 1
                    
                    #print(d1_data, s1_data, d2_data, s2_data)
                    self.adpd4000_inst.Slot_DataArr[idx][0].append(d1_data)
                    self.adpd4000_inst.Slot_DataArr[idx][1].append(s1_data)
                    self.adpd4000_inst.Slot_DataArr[idx][2].append(l1_data)
                    self.adpd4000_inst.Slot_DataArr[idx][3].append(d2_data)
                    self.adpd4000_inst.Slot_DataArr[idx][4].append(s2_data)
                    self.adpd4000_inst.Slot_DataArr[idx][5].append(l2_data)
                    self.adpd4000_inst.slot_samplecount[idx] += 1
                    self.adpd4000_inst.Slot_CntArr[idx].append(self.adpd4000_inst.slot_samplecount[idx])
                    #print (self.adpd4000_inst.Slot_DataArr[idx])
                idx += 1

            if not self.header_write:
                self.write_ADPDCL_header(view_info)

            length = len(self.dataarr)
            str_data = ""
            for x in range(length):
                str_data += str(self.dataarr[x]) + "\t"

            str_intdata = ""
            if intrpt_flag :
                intBitsToRead = (dataBitsToRead + 56)
                data1 = ReceivedPkts[dataBitsToRead : intBitsToRead] 
                length = int(len(data1) / 8)
                if len(data1) % 7 != 0:
                    print("InterruptByte: Not in a correct length")

                for x in range(length):
                    intdata = ReceivedPkts[dataBitsToRead:(dataBitsToRead + increment)]
                    intdata = struct.unpack('d',intdata)[0]
                    self.interptStatus.append(intdata)
                    dataBitsToRead += increment      

                length = len(self.interptStatus)

                for x in range(length):
                    str_intdata += str(self.interptStatus[x]) + "\t"

                str_intdata += str(ReceivedPkts[intBitsToRead])
                self.isLeadOnInfo.append(ReceivedPkts[intBitsToRead])
                print("Intstatus data = " + str_intdata)
                self.interptStatus.clear()

            self.seqdatastr += "ADPD\t" + str(self.num) + "\t" + str(timestamp) + "\t" + str_data + "\n"

        elif sync == 0xC1:  # SP02
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            slot_info = ReceivedPkts[9:21]
            dataCount = 0
            no_samples = ReceivedPkts[5]

            length = int(len(slot_info))
            for x in range(length):
                info = slot_info[x]
                DSL_info = self.getBitValues(info, 0, 1)
                for i in range(0, 2):
                    dataCount += self.getBitValues(info, i, i)

                self.slotinfoarr.append(info)
                self.DSLinfoarr.append(DSL_info)

            dataCount += 1   # Timestamp
            dataCount += 3   # SPO2 Data
            dataBitsToRead = ((dataCount * 8) + 25)  # add prefix 25

            self.num = struct.unpack('H', seq_num)[0]

            self.samplecount_spo2 = self.samplecount_spo2 + 1
            self.samplecntarr.append(self.samplecount_spo2)

            data1 = ReceivedPkts[25:dataBitsToRead]  # skip prefix 25
            length = int(len(data1) / 8)
            increment = 8
            startbit = 25

            if len(data1) % 8 != 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d', data)[0]
                self.dataarr.append(data)
                startbit += increment

            length = int(len(slot_info))
            idx = 0
            ndatabuffindex = 1
            for x in range(length):
                if ((self.spo2_inst.slot_samplecount[idx] % self.spo2_inst.slot_frameSz[idx]) == 0):
                    self.spo2_inst.Slot_DataArr[idx][0] = []
                    self.spo2_inst.Slot_DataArr[idx][1] = []
                    self.spo2_inst.Slot_CntArr[idx] = []
                if (self.DSLinfoarr[idx] != 0):
                    s1_data = 0
                    s2_data = 0
                    s1_bit = self.DSLinfoarr[idx] >> 0 & 1
                    s2_bit = self.DSLinfoarr[idx] >> 1 & 1
                    if s1_bit is 1:
                        s1_data = self.dataarr[ndatabuffindex]
                        ndatabuffindex += 1
                    if s2_bit is 1:
                        s2_data = self.dataarr[ndatabuffindex]
                        ndatabuffindex += 1

                    # print(d1_data, s1_data, d2_data, s2_data)
                    self.spo2_inst.Slot_DataArr[idx][0].append(s1_data)
                    self.spo2_inst.Slot_DataArr[idx][1].append(s2_data)
                    self.spo2_inst.slot_samplecount[idx] += 1
                    self.spo2_inst.Slot_CntArr[idx].append(self.spo2_inst.slot_samplecount[idx])
                idx += 1

            if not self.header_write:
                self.seqdatastr = "App" + "\t" + "Seq No" + "\t" + "Time Stamp" + "\t" + "Data\n"
                self.header_write = True

            i = 0
            length = int(len(slot_info))

            self.seqdatastr += "SPO2\t"
            self.seqdatastr += str(str(self.num)) + "\t"

            self.seqdatastr += self.get_time(self.dataarr[i]) + "\t"
            i += 1

            for x in range(length):
                if self.DSLinfoarr[x] == 1 or self.DSLinfoarr[x] == 2:
                    self.seqdatastr += str(self.dataarr[i]) + "\t"
                    i += 1
                elif self.DSLinfoarr[x] == 3:
                    self.seqdatastr += str(self.dataarr[i]) + "\t"
                    i += 1
                    self.seqdatastr += str(self.dataarr[i]) + "\t"
                    i += 1
            self.seqdatastr += str(self.dataarr[i]) + "\t"  # SPO2
            i += 1
            self.seqdatastr += str(self.dataarr[i]) + "\t"  # ROR
            i += 1
            self.seqdatastr += str(self.dataarr[i]) + "\n"  # MOTION
            i += 1

        elif sync == 0xB0:  #ADXL
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            no_samples = ReceivedPkts[5]

            self.num = struct.unpack('H', seq_num)[0]

            data1 = ReceivedPkts[11:]  # skip prefix 11
            length = int(len(data1) / 8)
            increment = 8
            startbit = 11

            if len(data1) % 8!= 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d', data)[0]
                self.dataarr.append(data)
                startbit += increment

            if not self.header_write:
                self.seqdatastr = "App" + "\t" + "Seq No" + "\t" + "Time Stamp" + "\t" + "Data_X" + "\t" + "Data_Y" + "\t" + "Data_Z\n"
                self.header_write = True

            if ((self.samplecount_adxl % self.framesize) == 0):
                if(self.samplecount_adxl != 0 and self.printdata):
                    print("ADXL" + str(self.ADXL_X_DataArr) + str(self.ADXL_Y_DataArr) + str(self.ADXL_Z_DataArr))
                self.clear_adxl_data_buffers()

            i = 0
            for x in range(no_samples):
                self.samplecount_adxl = self.samplecount_adxl + 1
                self.ADXL_samplecntarr.append(self.samplecount_adxl)

                self.seqdatastr += "ADXL\t"
                self.seqdatastr += str(str(self.num)) + "\t"

                self.ADXL_ts.append(self.dataarr[i])
                self.seqdatastr += self.get_time(self.dataarr[i]) + "\t"
                i += 1
                self.ADXL_X_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.ADXL_Y_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.ADXL_Z_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\n"
                i += 1

        elif sync == 0xC0:  #PPG
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            no_samples = ReceivedPkts[5]

            self.num = struct.unpack('H', seq_num)[0]

            data1 = ReceivedPkts[11:]  # skip prefix 11
            length = int(len(data1) / 8)
            increment = 8
            startbit = 11

            if len(data1) % 8!= 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d', data)[0]
                self.dataarr.append(data)
                startbit += increment

            if not self.header_write:
                self.seqdatastr = "App" + "\t" + "Seq No" + "\t" +"Time Stamp" + "\t" + "ADPD_Data" + "\t" + "Data_X" + "\t" + "Data_Y" + "\t" + "Data_Z" + "\t" + "HR\n"
                self.header_write = True

            if ((self.samplecount_ppg % self.frameSz) == 0):
                #if(self.samplecount_ppg != 0 and self.printdata):
                    #print("PPG" + str(self.PPG_DataArr))
                    #print("ADXL" + str(self.X_DataArr) + str(self.Y_DataArr) + str(self.Z_DataArr))
                    #print("HR" + str(self.HR_DataArr))
                self.clear_ppg_data_buffers()
                self.isPPGDataReady = False

            i = 0
            for x in range(no_samples):
                self.samplecount_ppg = self.samplecount_ppg + 1
                self.SyncPPG_CntArr.append(self.samplecount_ppg)

                self.seqdatastr += "PPG\t"
                self.seqdatastr += str(str(self.num)) + "\t"

                self.PPG_ts.append(self.dataarr[i])
                self.seqdatastr += self.get_time(self.dataarr[i]) + "\t"
                i += 1
                self.PPG_DataArr.append(self.dataarr[i])
                # print("PPG_DataArr: " + str(self.PPG_DataArr[-1]))
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.X_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.Y_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.Z_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.HR_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\n"
                i += 1
            if ((self.samplecount_ppg % self.frameSz) == 0):
                self.isPPGDataReady = True

        elif sync == 0xC2:  #ECG
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            no_samples = ReceivedPkts[5]

            self.num = struct.unpack('H', seq_num)[0]

            sensor_type = ReceivedPkts[9]
            algo_electrode_info = ReceivedPkts[10]
            lead_status = ReceivedPkts[11]

            data1 = ReceivedPkts[14:]  # skip prefix 14
            length = int(len(data1) / 8)
            increment = 8
            startbit = 14

            if len(data1) % 8!= 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d', data)[0]
                self.dataarr.append(data)
                startbit += increment

            if not self.header_write:
                self.seqdatastr = "App" + "\t" + "Seq No" + "\t" + "Time Stamp" + "\t" + "ECG" + "\t" + "HR\n"
                self.header_write = True

            if ((self.samplecount_ecg % self.framesize) == 0):
                if (self.samplecount_ecg != 0 and self.printdata):
                    print("ECG" + str(self.ECG_DataArr) + str(self.ECG_HRArr))
                self.clear_ecg_data_buffers()

            i = 0
            for x in range(no_samples):
                self.samplecount_ecg = self.samplecount_ecg + 1
                self.ECG_samplecntarr.append(self.samplecount_ecg)

                self.seqdatastr += "ECG\t"
                self.seqdatastr += str(str(self.num)) + "\t"

                self.ECG_ts.append(self.dataarr[i])
                self.seqdatastr += self.get_time(self.dataarr[i]) + "\t"
                i += 1
                self.ECG_DataArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.ECG_HRArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\n"
                i += 1

        elif sync == 0xC3:  #EDA
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            no_samples = ReceivedPkts[5]

            self.num = struct.unpack('H', seq_num)[0]

            imp_adm_selection = ReceivedPkts[9]

            data1 = ReceivedPkts[12:]  # skip prefix 11
            length = int(len(data1) / 8)
            increment = 8
            startbit = 12

            if len(data1) % 8!= 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d', data)[0]
                self.dataarr.append(data)
                startbit += increment

            if not self.header_write:
                self.seqdatastr = "App" + "\t" + "Seq No" + "\t" + "Time Stamp" + "\t" + "Module" + "\t" + "Phase\n"
                self.header_write = True

            if ((self.samplecount_eda % self.framesize) == 0):
                if(self.samplecount_eda != 0 and self.printdata):
                    print("EDA" + str(self.EDA_ModuleArr) + str(self.EDA_PhaseArr))
                self.clear_eda_data_buffers()

            i = 0
            for x in range(no_samples):
                self.samplecount_eda = self.samplecount_eda + 1
                self.EDA_samplecntarr.append(self.samplecount_eda)

                self.seqdatastr += "EDA" + "\t"
                self.seqdatastr += str(str(self.num)) + "\t"

                self.EDA_ts.append(self.dataarr[i])
                self.seqdatastr += self.get_time(self.dataarr[i]) + "\t"
                i += 1
                self.EDA_ModuleArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.EDA_PhaseArr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\n"
                i += 1

        elif sync == 0xC4:  #Temperature
            size_data = ReceivedPkts[1:3]
            seq_num = ReceivedPkts[3:5]
            no_samples = ReceivedPkts[5]

            self.num = struct.unpack('H', seq_num)[0]

            data1 = ReceivedPkts[11:]  # skip prefix 11
            length = int(len(data1) / 8)
            increment = 8
            startbit = 11

            if len(data1) % 8!= 0:
                print("Not in a correct length")

            for x in range(length):
                data = ReceivedPkts[startbit:(startbit + increment)]
                data = struct.unpack('d', data)[0]
                self.dataarr.append(data)
                startbit += increment

            if not self.header_write:
                self.seqdatastr = "App" + "\t" + "Seq No" + "\t" + "Time Stamp" + "\t" + "Temp 1" +"\t" +  "Temp 2\n"
                self.header_write = True

            if ((self.samplecount_temperature % self.frameSz) == 0):
                if(self.samplecount_temperature != 0 and self.printdata):
                    print("TEMPERATURE" + str(self.TEMP_TEMP1Arr) + str(self.TEMP_TEMP2Arr))
                self.clear_temp_data_buffers()

            i = 0
            for x in range(no_samples):
                self.samplecount_temperature = self.samplecount_temperature + 1
                self.TEMP_samplecntarr.append(self.samplecount_temperature)

                self.seqdatastr += "TEMP\t"
                self.seqdatastr += str(str(self.num)) + "\t"

                self.TEMP_ts.append(self.dataarr[i])
                self.seqdatastr += self.get_time(self.dataarr[i]) + "\t"
                i += 1
                self.TEMP_TEMP1Arr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\t"
                i += 1
                self.TEMP_TEMP2Arr.append(self.dataarr[i])
                self.seqdatastr += str(self.dataarr[i]) + "\n"
                i += 1

        return self.seqdatastr
    

    def getBitValues(self,value, startBit, endBit):
        '''
        To get the value of the bits specified
        '''
        Temp = value
        Temp <<= 15 - endBit
        Temp = Temp & 0xffff
        Temp >>= 15 - endBit + startBit
        Temp = Temp & 0xffff
        return Temp

    def clear_data_buffers(self):
        self.SlotA_DataArr = [[], [], [], []]
        self.SlotB_DataArr = [[], [], [], []]
        self.samplecntarr = []

    def clear_ppg_data_buffers(self):
        self.PPG_DataArr.clear()
        self.X_DataArr.clear()
        self.Y_DataArr.clear()
        self.Z_DataArr.clear()
        self.HR_DataArr.clear()
        self.SyncPPG_CntArr.clear()

    def clear_adxl_data_buffers(self):
        self.ADXL_X_DataArr.clear()
        self.ADXL_Y_DataArr.clear()
        self.ADXL_Z_DataArr.clear()
        self.ADXL_ts.clear()
        self.ADXL_samplecntarr.clear()

    def clear_ecg_data_buffers(self):
        self.ECG_ts.clear()
        self.ECG_DataArr.clear()
        self.ECG_HRArr.clear()
        self.ECG_samplecntarr.clear()

    def clear_eda_data_buffers(self):
        self.EDA_ts.clear()
        self.EDA_ModuleArr.clear()
        self.EDA_PhaseArr.clear()
        self.EDA_samplecntarr.clear()

    def clear_temp_data_buffers(self):
        self.TEMP_ts.clear()
        self.TEMP_TEMP1Arr.clear()
        self.TEMP_TEMP2Arr.clear()
        self.TEMP_samplecntarr.clear()

    def get_slotname(self, slot):
        if slot == 1:
            active_slot = Slot.SLOT_SUM
        elif slot == 2:
            active_slot = Slot.SLOT_4CH
        elif slot == 3:
            active_slot = Slot.SLOT_DIM
        else:
            active_slot = Slot.SLOT_OFF
        return active_slot

    def get_unitname(self, unit):
        if unit == 0:
            unit_name = Unit.COUNT.name
        elif unit == 1:
            unit_name = Unit.CTR.name
        elif unit == 2:
            unit_name = Unit.PTR.name
        elif unit == 3:
            unit_name = Unit.UV.name

        return unit_name

    def get_offsetname(self, offset):
        if offset == 0:
            offset_name = Offset.NO_OFFSET.name
        elif offset == 1:
            offset_name = Offset.NULL_OFFSET.name
        elif offset == 2:
            offset_name = Offset.STATS_OFFSET.name
        elif offset == 3:
            offset_name = Offset.NULL_STATS_OFFSET.name

        return offset_name

    def get_slotsize(self,slot):
        if slot == Slot.SLOT_OFF.value:
            size = 0
        elif slot == Slot.SLOT_SUM.value:
            size = 1
        elif slot == Slot.SLOT_4CH.value:
            size = 4
        elif slot == Slot.SLOT_DIM.value:
            size = 1

        return size

    def write_header(self,slot,unit,offset):
        str_header = ""
        if slot[0] != Slot.SLOT_OFF.name:
            if slot[0] == Slot.SLOT_SUM.name:
                str_header = "Slot A : SUM " + "\t"
            elif slot[0] == Slot.SLOT_4CH.name:
                str_header = "Slot A : 4CH " + "\t"

            str_header += "Plot 1 Unit : " + str(unit[0]) + "\t"
            str_header += "Plot 1 Offset : " + str(offset[0]) + "\t"

        if slot[1] != Slot.SLOT_OFF.name:
            if slot[1] == Slot.SLOT_SUM.name:
                str_header += "Slot B : SUM " + "\t"
            elif slot[1] == Slot.SLOT_4CH.name:
                str_header += "Slot B : 4CH " + "\t"

            str_header += "Plot 2 Unit : " + str(unit[1]) + "\t"
            str_header += "Plot 2 Offset : " + str(offset[1]) + "\t"

        str_header += "\n"

        if slot[0] != Slot.SLOT_OFF:
            if slot[0] == Slot.SLOT_SUM:
                str_header += "SUM" + "\t"
            elif slot[0] == Slot.SLOT_4CH:
                str_header += "CH1" + "\t" + "CH2" + "\t" + "CH3" + "\t" + "CH4" + "\t"

        if slot[1] != Slot.SLOT_OFF:
            if slot[1] == Slot.SLOT_SUM:
                str_header += "SUM" + "\t"
            elif slot[1] == Slot.SLOT_4CH:
                str_header += "CH1" + "\t" + "CH2" + "\t" + "CH3" + "\t" + "CH4" + "\t"

        self.seqdatastr += str_header + "Time Stamp" + "\t" + "Sequence Number\n"
        self.header_write = True

    def write_ADPDCL_header(self,view_info):
        str_header = ""

        self.seqdatastr = "App" + "\t" + "Seq No" + "\t" + "Time Stamp" + "\t" + "Data\n"
        # if view_info == 1:
        #     str_header += "View : MultiSlot View" + "\t"
        #     str_header += "Plot Unit : " + str(self.unitarr[0]) + "\t"
        #     str_header += "Plot Offset : " + str(self.offsetarr[0]) + "\n"
        # elif view_info == 0:
        #     str_header += "View : Time View" + "\t"
        #     str_header += "Plot 1 Unit : " + str(self.unitarr[0]) + "\t"
        #     str_header += "Plot 1 Offset : " + str(self.offsetarr[0]) + "\t"
        #
        #     str_header += "Plot 2 Unit : " + str(self.unitarr[1]) + "\t"
        #     str_header += "Plot 2 Offset : " + str(self.offsetarr[1]) + "\n"

        # length = len(self.slotinfoarr)
        # for x in range(length):
        #     if self.slotinfoarr[x] != 0:
        #         if self.channel_infoarr[x] == 1:
        #             str_header += ADPDCL_Slot(x).name + " CH1 " + "\t"
        #         elif self.channel_infoarr[x] == 3:
        #             str_header += ADPDCL_Slot(x).name + " CH1 " + "\t" +  ADPDCL_Slot(x).name + " CH2 " + "\t"

        #self.seqdatastr = str_header + "Time Stamp" + "\t" + "Sequence Number\n"
        #self.seqdatastr = str_header
        self.header_write = True

    def get_time(self, timestamp):
        str_time = ""

        if(self.device == 0):
            ts_sec = timestamp / 32000
        else:
            ts_sec = timestamp / 1000

        hours = (ts_sec / 3600)
        hours = int(hours)
        sec_to_remove = (ts_sec % 3600)
        ts_sec = sec_to_remove
        minutes = (ts_sec / 60)
        minutes = int(minutes)
        seconds = (ts_sec % 60)
        seconds = int(seconds)

        str_time = str(hours) + ":" + str(minutes) + ":" + str(seconds)

        return str_time

class Slot(Enum):
    SLOT_OFF = 0
    SLOT_SUM = 1
    SLOT_4CH = 2
    SLOT_DIM = 3

class Slotsize(Enum):
    SLOT_OFF = 0
    SLOT_SUM = 1
    SLOT_SUM_SUM = 2
    SLOT_4CH = 4
    SLOT_SUM_4CH = 5
    SLOT_4CH_4CH = 8

class Unit(Enum):
    COUNT = 0
    CTR = 1
    PTR = 2
    UV = 3

class Offset(Enum):
    NO_OFFSET = 0
    NULL_OFFSET = 1
    STATS_OFFSET = 2
    NULL_STATS_OFFSET = 3

class ADPDCL_Slot(Enum):
    SLOT_A = 0
    SLOT_B = 1
    SLOT_C = 2
    SLOT_D = 3
    SLOT_E = 4
    SLOT_F = 5
    SLOT_G = 6
    SLOT_H = 7
    SLOT_I = 8
    SLOT_J = 9
    SLOT_K = 10
    SLOT_L = 11


