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
import struct
import datetime
import time
import sys
from enum import Enum


class M2M2DataParserClass():

    def __init__(self):
        self.coolidge_parser_inst = CoolidgeDataParserClass()
        self.ppg_parser_inst = PPG_DataParserClass()
        self.adpdParseInit = AdpdDataParserClass()
        self.adpd4000_sources = [0xC211, 0xC212, 0xC213, 0xC214, 0xC215, 0xC216,
                                 0xC217, 0xC218, 0xC219, 0xC21A, 0xC21B, 0xC21C]
        self.adpd_stream = 0xC201
        self.adxl_stream = 0xC202
        self.sync_ppg_stream = 0xC405
        self.ppg_stream = 0xC400
        self.sync = 0xF0

    def m2m2_data_parser(self, m2m2_data):
        source = struct.unpack('>H', m2m2_data[:2])[0]

        if source in self.adpd4000_sources:
            return self.coolidge_parser_inst.parse_coolidge_stream(m2m2_data)
        elif source == self.adpd_stream:
            return self.adpdParseInit.parse_ADPD_stream(m2m2_data)
        elif source == self.sync_ppg_stream:
            return self.ppg_parser_inst.parse_syncPPG_stream(m2m2_data)
        elif source == self.ppg_stream:
            return self.ppg_parser_inst.parse_ppg_stream(m2m2_data)
        else:
            print("Invalid Source detected in parser")
            return ""

    def getCoolidgeParserInst(self):
        return self.coolidge_parser_inst

    def get_ppg_parser_inst(self):
        return self.ppg_parser_inst

    def getAdpdParserInst(self):
        return self.adpdParseInit

    class ADPD_SlotType(Enum):
        SLOT_A = 'SLOTA'
        SLOT_B = 'SLOTB'
        SLOT_C = 'SLOTC'
        SLOT_D = 'SLOTD'
        SLOT_E = 'SLOTE'
        SLOT_F = 'SLOTF'
        SLOT_G = 'SLOTG'
        SLOT_H = 'SLOTH'
        SLOT_I = 'SLOTI'
        SLOT_J = 'SLOTJ'
        SLOT_K = 'SLOTK'
        SLOT_L = 'SLOTL'

class AdpdDataParserClass():
    def __init__(self):
        self.NumPacket = 10
        self.M2M_PKT_RAW_DATA_ADPD_A_SUM_32b = 0x03
        self.M2M_PKT_RAW_DATA_ADPD_B_SUM_32b = 0x0B
        self.M2M_PKT_RAW_DATA_ADPD_A_SUM_16b = 0x02
        self.M2M_PKT_RAW_DATA_ADPD_B_SUM_16b = 0x0A
        self.M2M_PKT_RAW_DATA_ADPD_A_4CH_32b = 0x01
        self.M2M_PKT_RAW_DATA_ADPD_B_4CH_32b = 0x09
        self.M2M_PKT_RAW_DATA_ADPD_B_4CH_16b = 0x08
        self.M2M_PKT_RAW_DATA_ADPD_A_4CH_16b = 0x00
        self.sampleData = 0
        self.Start_byte = 17
        self.int_data_size = 4
        self.short_data_size = 2
        self.no_of_samples = 0
        self.SlotA_DataArr = [[],[],[],[]]
        self.SlotB_DataArr = [[],[],[],[]]
        self.SlotA_CntArr = []
        self.SlotB_CntArr = []
        self.slotA_samplecount = 0
        self.slotB_samplecount = 0
        self.slot = 0
        self.slot_Type = ''

    def parse_ADPD_stream(self, m2m2data):
        self.slot_index_adpd = 12
        self.slot = m2m2data[self.slot_index_adpd]
        self.Start_byte = 17
        self._4ch_buff = []
        _4CH_max_length = 4
        skip_delta_ts = 2

        result = ""
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('%H:%M:%S')
        data_buffer = self.getdatabuff(self.slot)
        slot_name = self.getSlotType(self.slot)

        if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_32b or self.slot is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_32b:
            self.no_of_samples = 7
            nFrameSz = self.get_ADPD_FrameSz()  # add parameter to check slot

            for x in range(self.no_of_samples):
                self.sampleData = struct.unpack('I', m2m2data[self.Start_byte : self.Start_byte + self.int_data_size])[0]
                result += self.append_adpd_sum_data(self.sampleData, slot_name, timestamp, data_buffer)
                self.Start_byte += (self.int_data_size + skip_delta_ts)
                if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_32b:
                    if self.slotA_samplecount % nFrameSz == 0:
                        self.clearSlotAbuffers()
                    self.SlotA_DataArr[0].append(self.sampleData)
                    self.slotA_samplecount += 1
                    self.SlotA_CntArr.append(self.slotA_samplecount)
                    self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_A
                else:
                    if self.slotB_samplecount % nFrameSz == 0:
                        self.clearSlotBbuffers()
                    self.SlotB_DataArr[0].append(self.sampleData)
                    self.slotB_samplecount += 1
                    self.SlotB_CntArr.append(self.slotB_samplecount)
                    self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_B

        elif self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_16b or self.slot is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_16b:
            self.no_of_samples = 11
            nFrameSz = self.get_ADPD_FrameSz()

            for x in range(self.no_of_samples):
                self.sampleData = struct.unpack('H', m2m2data[self.Start_byte:self.Start_byte + self.short_data_size])[0]
                result = result + self.append_adpd_sum_data(self.sampleData, slot_name, timestamp, data_buffer)
                self.Start_byte += (self.short_data_size + skip_delta_ts)
                if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_16b:
                    if self.slotA_samplecount % nFrameSz == 0:
                        self.clearSlotAbuffers()
                    self.SlotA_DataArr[0].append(self.sampleData)
                    self.slotA_samplecount += 1
                    self.SlotA_CntArr.append(self.slotA_samplecount)
                    self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_A
                else:
                    if self.slotB_samplecount % nFrameSz == 0:
                        self.clearSlotBbuffers()
                    self.SlotB_DataArr[0].append(self.sampleData)
                    self.slotB_samplecount += 1
                    self.SlotB_CntArr.append(self.slotB_samplecount)
                    self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_B


        elif self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_32b or self.slot is self.M2M_PKT_RAW_DATA_ADPD_B_4CH_32b:
            self.no_of_samples = 2
            nFrameSz = self.get_ADPD_FrameSz()
            if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_32b:
                if self.slotA_samplecount % nFrameSz == 0:
                    self.clearSlotAbuffers()
            else:
                if self.slotB_samplecount % nFrameSz == 0:
                    self.clearSlotBbuffers()

            for x in range(self.no_of_samples):
                for ch in range(_4CH_max_length):
                    ch_data = struct.unpack('I', m2m2data[self.Start_byte : self.Start_byte + self.int_data_size])[0]
                    self._4ch_buff.append(ch_data)
                    self.Start_byte += self.int_data_size

                    if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_32b:
                        self.SlotA_DataArr[ch].append(ch_data)
                        if ch is 3:
                            self.slotA_samplecount += 1
                            self.SlotA_CntArr.append(self.slotA_samplecount)
                        self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_A
                    else:
                        self.SlotB_DataArr[ch].append(ch_data)
                        if ch is 3:
                            self.slotB_samplecount += 1
                            self.SlotB_CntArr.append(self.slotB_samplecount)
                        self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_B

                if len(self._4ch_buff) is _4CH_max_length:
                    result = result + self.append_adpd_4ch_data(self._4ch_buff[0], self._4ch_buff[1],
                                                             self._4ch_buff[2], self._4ch_buff[3], slot_name, timestamp, data_buffer)
                    self._4ch_buff = []
                    self.Start_byte += skip_delta_ts

        elif self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_16b or self.slot is self.M2M_PKT_RAW_DATA_ADPD_B_4CH_16b:
            self.no_of_samples = 4
            nFrameSz = self.get_ADPD_FrameSz()
            if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_16b:
                if self.slotA_samplecount % nFrameSz == 0:
                    self.clearSlotAbuffers()
            else:
                if self.slotB_samplecount % nFrameSz == 0:
                    self.clearSlotBbuffers()

            for x in range(self.no_of_samples):
                for ch in range(_4CH_max_length):
                    ch_data = struct.unpack('H', m2m2data[self.Start_byte: self.Start_byte + self.short_data_size])[0]
                    self._4ch_buff.append(ch_data)
                    self.Start_byte += self.short_data_size
                    if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_16b:
                        self.SlotA_DataArr[ch].append(ch_data)
                        if ch is 0:
                            self.slotA_samplecount += 1
                            self.SlotA_CntArr.append(self.slotA_samplecount)
                        self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_A
                    else:
                        self.SlotB_DataArr[ch].append(ch_data)
                        if ch is 0:
                            self.slotB_samplecount += 1
                            self.SlotB_CntArr.append(self.slotB_samplecount)
                        self.slot_Type = M2M2DataParserClass.ADPD_SlotType.SLOT_B

                if len(self._4ch_buff) is _4CH_max_length:
                    result = result + self.append_adpd_4ch_data(self._4ch_buff[0], self._4ch_buff[1],
                                                             self._4ch_buff[2], self._4ch_buff[3], slot_name, timestamp, data_buffer)
                    self._4ch_buff = []
                    self.Start_byte += skip_delta_ts

        return result

    def clearSlotAbuffers(self):
        self.SlotA_DataArr = [[],[],[],[]]
        self.SlotA_CntArr = []

    def clearSlotBbuffers(self):
        self.SlotB_DataArr = [[],[],[],[]]
        self.SlotB_CntArr = []

    def getSlotType(self,slot):
        if slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_32b:
            return "ADPD_A_SUM_32b"
        elif slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_16b:
            return "ADPD_A_SUM_16b"
        elif slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_32b:
            return "ADPD_A_4CH_32b"
        elif slot is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_32b:
            return "ADPD_B_SUM_32b"
        elif slot is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_16b:
            return "ADPD_B_SUM_16b"
        elif slot is self.M2M_PKT_RAW_DATA_ADPD_B_4CH_32b:
            return "ADPD_B_4CH_32b"
        elif slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_16b:
            return "ADPD_A_4CH_16b"
        elif slot is self.M2M_PKT_RAW_DATA_ADPD_B_4CH_16b:
            return "ADPD_B_4CH_16b"

    def getdatabuff(self, slotType):
        if slotType is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_32b or slotType is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_16b:
            return self.SlotA_DataArr[0]
        elif slotType is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_32b or slotType is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_16b:
            return self.SlotB_DataArr[0]
        elif slotType is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_32b or slotType is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_16b:
            return self.SlotA_DataArr
        elif slotType is self.M2M_PKT_RAW_DATA_ADPD_B_4CH_32b or slotType is self.M2M_PKT_RAW_DATA_ADPD_B_4CH_16b:
            return self.SlotB_DataArr

    def append_adpd_4ch_data(self, ch1, ch2, ch3, ch4, slot_name, timestamp, _4ch_data_buff):
        output_str = "{}\t{}\t{}\t{}\t{}\t{}".format(slot_name, timestamp, str(ch1), str(ch2), str(ch3), str(ch4))
        _4ch_data_buff.append(output_str)
        return output_str + "\n"

    def append_adpd_sum_data(self, samples, slot_name, timestamp, sum_data_buff):
        output_str = str(samples)
        output_str = "{}\t{}\t{}".format(slot_name, timestamp, output_str)
        sum_data_buff.append(output_str)
        return output_str + "\n"

    def get_ADPD_FrameSz(self):
        if self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_32b or self.slot is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_32b:
            self.no_of_samples = 7
        elif self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_SUM_16b or self.slot is self.M2M_PKT_RAW_DATA_ADPD_B_SUM_16b:
            self.no_of_samples = 11
        elif self.slot is self.M2M_PKT_RAW_DATA_ADPD_A_4CH_32b or self.slot is self.M2M_PKT_RAW_DATA_ADPD_B_4CH_32b:
            self.no_of_samples = 2
        else:
            self.no_of_samples = 4

        return self.NumPacket * self.no_of_samples


class PPG_DataParserClass():

    def __init__(self):
        self.NumPacket = 10
        self.encoded = 1  # if ASCII Encoding enabled -> 2 else -> 1
        self.int_data_size = 4 * self.encoded
        self.short_data_size = 2 * self.encoded
        self.samples = []
        self.no_of_HR_samples = 16
        self.no_of_adpd_samples = 4
        self.no_of_adxl_samples = 12 # 4 * 3 (x,y,x)
        self.adpd_start_index = 32 * self.encoded
        self.adxl_start_index = 48 * self.encoded
        self.ppg_start_index = 18 * self.encoded
        self.ppg_adpd_queue = []
        self.ppg_adxl_queue = []
        #self.file_manager_Inst = logger.FileManagerClass()
        self.udpTransferInst = None
        self.loggerInst = None
        self.sampleData = 0
        self.Start_byte = 34
        self.X=-1
        self.Y=-1
        self.Z=-1
        self.PRM_DATA_VALUE_MASK = 0x3FFF
        self.PRM_SIGN_EXTENSION_MASK = 0x3000
        self.HR=0.0
        self.HR_DataArr = []
        self.X_DataArr = []
        self.Y_DataArr = []
        self.Z_DataArr = []
        self.PPG_DataArr = []
        self.SyncPPG_CntArr = []
        self.HR_CntArr = []
        self.syncppg_samplecount = 0
        self.hr_samplecount = 0

    def parse_ppg_stream(self, m2m2data):
        self.parse_HR(m2m2data)

    def parse_HR(self,m2m2data):
        try:
            self.ppg_start_index = 18 * self.encoded
            nFrameSz = self.get_hr_FrameSz()
            if self.hr_samplecount % nFrameSz  == 0:
                self.HR_DataArr = []
                self.HR_CntArr = []

            sample = struct.unpack('h', m2m2data[self.ppg_start_index:self.ppg_start_index + self.short_data_size])[0]
            if sample is not None:
                self.HR = sample/16

            self.HR_DataArr.append(self.HR)
            self.hr_samplecount += 1
            self.HR_CntArr.append(self.hr_samplecount)

        except:
            logstr = "HR Data Parse Error : " + str(sys.exc_info())
            print(logstr)

    def parse_adpd_samples(self, m2m2data,timeStamp):
        self.adpd_start_index = 32 * self.encoded
        for x in range(self.no_of_adpd_samples):
            sample = struct.unpack('I', m2m2data[self.adpd_start_index:self.adpd_start_index + self.int_data_size])[0]
            self.adpd_start_index += self.int_data_size

            if sample is not None:
                self.ppg_adpd_queue.append(sample)
            self.PPG_DataArr.append(sample)
            self.syncppg_samplecount += 1
            self.SyncPPG_CntArr.append(self.syncppg_samplecount)


    def parse_adxl_samples(self, m2m2data):
        self.adxl_start_index = 48 * self.encoded
        idx = 0
        for x in range(self.no_of_adxl_samples):
            sample = struct.unpack('H', m2m2data[self.adxl_start_index:self.adxl_start_index + self.short_data_size])[0]
            self.adxl_signed_data(sample) # adding to adxl queue
            if idx < 4:
                self.X_DataArr.append(sample)
            elif idx < 8:
                self.Y_DataArr.append(sample)
            else:
                self.Z_DataArr.append(sample)
            idx += 1
            self.adxl_start_index += self.short_data_size


    def clear_SyncPPG_Buffers(self):
        self.X_DataArr = []
        self.Y_DataArr = []
        self.Z_DataArr = []
        self.PPG_DataArr = []
        self.SyncPPG_CntArr = []

    def adxl_signed_data(self,sample):
        data = ((sample & self.PRM_DATA_VALUE_MASK) | ((sample & self.PRM_SIGN_EXTENSION_MASK) << 2))
        unsigned_adxl = struct.pack('H', data)  # Packing a ushort number.
        signed_adxl = struct.unpack('h', unsigned_adxl)[0]  # Unpacking a packed ushort number to signed short
        if sample is not None:
            self.ppg_adxl_queue.append(signed_adxl)

    def dataAppendAdpd(self,data,timeStamp):
        result = "{}\t{}\t{}".format(timeStamp, "ADPD", data)
        self.ppg_adpd_queue.append(result)

    def dataAppendAdxl(self,x,y,z):
        result = "\t{}\t{}\t{}\t{}".format( "ADXL", x,y,z)
        self.ppg_adxl_queue.append(result)

    def parse_syncPPG_stream(self, m2m2data):
        nFrameSz = self.get_syncPPG_FrameSz()
        if self.syncppg_samplecount % nFrameSz == 0:
            self.clear_SyncPPG_Buffers()
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('%H:%M:%S')
        self.parse_adpd_samples(m2m2data, timestamp)
        self.parse_adxl_samples(m2m2data)
        #if self.loggerInst is None:
           #self.loggerInst = logger.LogManagerClass()
        # write samples to a file
        #logfilepath = os.path.join(self.file_manager_Inst.user_file_path, self.file_manager_Inst.user_filename)
        #self.loggerInst.delegate_filewrite(logfilepath, util.STREAM.PPG)

    def get_syncPPG_FrameSz(self):
        nSyncPPGSampleSz = 4
        return self.NumPacket * nSyncPPGSampleSz

    def get_hr_FrameSz(self):
        return self.NumPacket * 3


class CoolidgeDataBuffers:
    def __init__(self):
        self.SLOTA_DataArr = [[], []]
        self.SLOTB_DataArr = [[], []]
        self.SLOTC_DataArr = [[], []]
        self.SLOTD_DataArr = [[], []]
        self.SLOTE_DataArr = [[], []]
        self.SLOTF_DataArr = [[], []]
        self.SLOTG_DataArr = [[], []]
        self.SLOTH_DataArr = [[], []]
        self.SLOTI_DataArr = [[], []]
        self.SLOTJ_DataArr = [[], []]
        self.SLOTK_DataArr = [[], []]
        self.SLOTL_DataArr = [[], []]
        self.SLOTA_CntArr = []
        self.SLOTB_CntArr = []
        self.SLOTC_CntArr = []
        self.SLOTD_CntArr = []
        self.SLOTE_CntArr = []
        self.SLOTF_CntArr = []
        self.SLOTG_CntArr = []
        self.SLOTH_CntArr = []
        self.SLOTI_CntArr = []
        self.SLOTJ_CntArr = []
        self.SLOTK_CntArr = []
        self.SLOTL_CntArr = []
        self.slotA_samplecount = 0
        self.slotB_samplecount = 0
        self.slotC_samplecount = 0
        self.slotD_samplecount = 0
        self.slotE_samplecount = 0
        self.slotF_samplecount = 0
        self.slotG_samplecount = 0
        self.slotH_samplecount = 0
        self.slotI_samplecount = 0
        self.slotJ_samplecount = 0
        self.slotK_samplecount = 0
        self.slotL_samplecount = 0


class CoolidgeDataParserClass():

    def __init__(self):
        self.NumPackets = 10
        self.dataformat = 0
        self.dark = 0
        self.signal = 0
        self.datatype = 0
        self.channelType = 1
        self.sampleNum = 0
        self.totalSize = 0
        self.dataIndex = 20
        self.data = 0
        self.slotname = ""
        self.samples = []

        self.adpd4000ch1inst = CoolidgeDataBuffers()
        self.adpd4000ch2inst = CoolidgeDataBuffers()
        self.slotType = ''
        self.dict_coolidge_source_list = {0xc211: 'SlotA', 0xc212: 'SlotB', 0xc213: 'SlotC', 0xc214: 'SlotD',
                                          0xc215: 'SlotE', 0xc216: 'SlotF', 0xc217: 'SlotG', 0xc218: 'SlotH',
                                          0xc219: 'SlotI', 0xc21a: 'SlotJ', 0xc21b: 'SlotK', 0xc21c: 'SlotL'}

    def parse_coolidge_stream(self, m2m2data):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('%H:%M:%S')
        ds = m2m2data[12]

        self.dark = (ds >> 4) & 0x0f
        self.signal = (ds >> 0) & 0x0f

        self.channelType = m2m2data[14]
        self.sampleNum = m2m2data[19]

        self.totalNo_of_char_per_data_size = self.signal
        self.totalSize = self.dark + self.signal  # dark will always 0

        datasize = int(self.sampleNum) * int(self.totalSize)
        samples = m2m2data[self.dataIndex: (self.dataIndex + datasize)]

        source = struct.unpack('>H', m2m2data[:2])[0]
        self.slotname = self.getslotname(source)

        count = 1
        endbit = self.totalNo_of_char_per_data_size
        start_bit = 0

        while (count <= self.sampleNum):

            endbit = self.dark
            self.dark_data = 0

            if (self.dark == 1):
                self.dark_data = struct.unpack('B', samples[start_bit:endbit])[0]
            elif (self.dark == 2):
                self.dark_data = struct.unpack('H', samples[start_bit:endbit])[0]
            elif (self.dark == 3):
                data = samples[start_bit:endbit] + b'\x00'
                self.dark_data = struct.unpack('I', data)[0]
            elif (self.dark == 4):
                self.dark_data = struct.unpack('I', samples[start_bit:endbit])[0]

            start_bit = endbit
            endbit = self.signal
            self.signal_data = 0

            if (self.signal == 1):
                self.signal_data = struct.unpack('B', samples[start_bit:endbit])[0]
            elif (self.signal == 2):
                self.signal_data = struct.unpack('H', samples[start_bit:endbit])[0]
            elif (self.signal == 3):
                data = samples[start_bit:endbit] + b'\x00'
                self.signal_data = struct.unpack('I', data)[0]
            elif (self.signal == 4):
                self.signal_data = struct.unpack('I', samples[start_bit:endbit])[0]

            count = count + 1
            start_bit = endbit
            endbit = (count * self.totalNo_of_char_per_data_size)

            outputStr = str(self.data)
            outputStr = "{}\tCH{}\t{}\t{}".format(self.slotname, self.channelType, timestamp, outputStr)

            buff = self.getdatabuff(self.slotname, self.channelType)
            self.append_data(outputStr, buff)

            self.slotType = self.getslotname(source)
            nFrameSz = self.get_adpd4000_framesize()

            if self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_A:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotA_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTA_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTA_CntArr = []
                    self.adpd4000ch1inst.SLOTA_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTA_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotA_samplecount += 1
                    self.adpd4000ch1inst.SLOTA_CntArr.append(self.adpd4000ch1inst.slotA_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotA_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTA_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTA_CntArr = []
                    self.adpd4000ch2inst.SLOTA_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTA_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotA_samplecount += 1
                    self.adpd4000ch2inst.SLOTA_CntArr.append(self.adpd4000ch2inst.slotA_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_B:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotB_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTB_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTB_CntArr = []
                    self.adpd4000ch1inst.SLOTB_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTB_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotB_samplecount += 1
                    self.adpd4000ch1inst.SLOTB_CntArr.append(self.adpd4000ch1inst.slotB_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotB_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTB_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTB_CntArr = []
                    self.adpd4000ch2inst.SLOTB_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTB_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotB_samplecount += 1
                    self.adpd4000ch2inst.SLOTB_CntArr.append(self.adpd4000ch2inst.slotB_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_C:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotC_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTC_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTC_CntArr = []
                    self.adpd4000ch1inst.SLOTC_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTC_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotC_samplecount += 1
                    self.adpd4000ch1inst.SLOTC_CntArr.append(self.adpd4000ch1inst.slotC_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotC_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTC_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTC_CntArr = []
                    self.adpd4000ch2inst.SLOTC_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTC_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotC_samplecount += 1
                    self.adpd4000ch2inst.SLOTC_CntArr.append(self.adpd4000ch2inst.slotC_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_D:

                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotD_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTD_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTD_CntArr = []
                    self.adpd4000ch1inst.SLOTD_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTD_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotD_samplecount += 1
                    self.adpd4000ch1inst.SLOTD_CntArr.append(self.adpd4000ch1inst.slotD_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotD_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTD_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTD_CntArr = []
                    self.adpd4000ch2inst.SLOTD_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTD_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotD_samplecount += 1
                    self.adpd4000ch2inst.SLOTD_CntArr.append(self.adpd4000ch2inst.slotD_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_E:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotE_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTE_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTE_CntArr = []
                    self.adpd4000ch1inst.SLOTE_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTE_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotE_samplecount += 1
                    self.adpd4000ch1inst.SLOTE_CntArr.append(self.adpd4000ch1inst.slotE_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotE_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTE_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTE_CntArr = []
                    self.adpd4000ch2inst.SLOTE_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTE_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotE_samplecount += 1
                    self.adpd4000ch2inst.SLOTE_CntArr.append(self.adpd4000ch2inst.slotE_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_F:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotF_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTF_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTF_CntArr = []
                    self.adpd4000ch1inst.SLOTF_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTF_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotF_samplecount += 1
                    self.adpd4000ch1inst.SLOTF_CntArr.append(self.adpd4000ch1inst.slotF_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotF_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTF_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTF_CntArr = []
                    self.adpd4000ch2inst.SLOTF_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTF_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotF_samplecount += 1
                    self.adpd4000ch2inst.SLOTF_CntArr.append(self.adpd4000ch2inst.slotF_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_G:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotG_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTG_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTG_CntArr = []
                    self.adpd4000ch1inst.SLOTG_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTG_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotG_samplecount += 1
                    self.adpd4000ch1inst.SLOTG_CntArr.append(self.adpd4000ch1inst.slotG_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotG_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTG_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTG_CntArr = []
                    self.adpd4000ch2inst.SLOTG_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTG_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotG_samplecount += 1
                    self.adpd4000ch2inst.SLOTG_CntArr.append(self.adpd4000ch2inst.slotG_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_H:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotH_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTH_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTH_CntArr = []
                    self.adpd4000ch1inst.SLOTH_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTH_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotH_samplecount += 1
                    self.adpd4000ch1inst.SLOTH_CntArr.append(self.adpd4000ch1inst.slotH_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotH_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTH_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTH_CntArr = []
                    self.adpd4000ch2inst.SLOTH_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTH_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotH_samplecount += 1
                    self.adpd4000ch2inst.SLOTH_CntArr.append(self.adpd4000ch2inst.slotH_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_I:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotI_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTI_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTI_CntArr = []
                    self.adpd4000ch1inst.SLOTI_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTI_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotI_samplecount += 1
                    self.adpd4000ch1inst.SLOTI_CntArr.append(self.adpd4000ch1inst.slotI_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotI_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTI_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTI_CntArr = []
                    self.adpd4000ch2inst.SLOTI_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTI_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotI_samplecount += 1
                    self.adpd4000ch2inst.SLOTI_CntArr.append(self.adpd4000ch2inst.slotI_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_J:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotJ_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTJ_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTJ_CntArr = []
                    self.adpd4000ch1inst.SLOTJ_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTJ_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotJ_samplecount += 1
                    self.adpd4000ch1inst.SLOTJ_CntArr.append(self.adpd4000ch1inst.slotJ_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotJ_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTJ_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTJ_CntArr = []
                    self.adpd4000ch2inst.SLOTJ_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTJ_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotJ_samplecount += 1
                    self.adpd4000ch2inst.SLOTJ_CntArr.append(self.adpd4000ch2inst.slotJ_samplecount)
            elif self.slotType is M2M2DataParserClass.ADPD_SlotType.SLOT_K:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotK_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTK_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTK_CntArr = []
                    self.adpd4000ch1inst.SLOTK_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTK_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotK_samplecount += 1
                    self.adpd4000ch1inst.SLOTK_CntArr.append(self.adpd4000ch1inst.slotK_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotK_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTK_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTK_CntArr = []
                    self.adpd4000ch2inst.SLOTK_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTK_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotK_samplecount += 1
                    self.adpd4000ch2inst.SLOTK_CntArr.append(self.adpd4000ch2inst.slotK_samplecount)
            else:
                if self.channelType is 1:
                    if (self.adpd4000ch1inst.slotL_samplecount % nFrameSz) == 0:
                        self.adpd4000ch1inst.SLOTL_DataArr = [[], []]
                        self.adpd4000ch1inst.SLOTL_CntArr = []
                    self.adpd4000ch1inst.SLOTL_DataArr[0].append(self.dark_data)
                    self.adpd4000ch1inst.SLOTL_DataArr[1].append(self.signal_data)
                    self.adpd4000ch1inst.slotL_samplecount += 1
                    self.adpd4000ch1inst.SLOTL_CntArr.append(self.adpd4000ch1inst.slotL_samplecount)
                else:
                    if (self.adpd4000ch2inst.slotL_samplecount % nFrameSz) == 0:
                        self.adpd4000ch2inst.SLOTL_DataArr = [[], []]
                        self.adpd4000ch2inst.SLOTL_CntArr = []
                    self.adpd4000ch2inst.SLOTL_DataArr[0].append(self.dark_data)
                    self.adpd4000ch2inst.SLOTL_DataArr[1].append(self.signal_data)
                    self.adpd4000ch2inst.slotL_samplecount += 1
                    self.adpd4000ch2inst.SLOTL_CntArr.append(self.adpd4000ch2inst.slotL_samplecount)

        return "{}\n".format(outputStr)

    def getslotname(self, sourceAddress):
        return self.dict_coolidge_source_list[sourceAddress]

    def getdatabuff(self, slotname, channeltype):
        if slotname is "SlotA":
            return self.adpd4000ch1inst.SLOTA_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTA_DataArr
        elif slotname is "SlotB":
            return self.adpd4000ch1inst.SLOTB_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTB_DataArr
        elif slotname is "SlotC":
            return self.adpd4000ch1inst.SLOTC_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTC_DataArr
        elif slotname is "SlotD":
            return self.adpd4000ch1inst.SLOTD_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTD_DataArr
        elif slotname is "SlotE":
            return self.adpd4000ch1inst.SLOTE_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTE_DataArr
        elif slotname is "SlotF":
            return self.adpd4000ch1inst.SLOTF_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTF_DataArr
        elif slotname is "SlotG":
            return self.adpd4000ch1inst.SLOTG_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTG_DataArr
        elif slotname is "SlotH":
            return self.adpd4000ch1inst.SLOTH_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTH_DataArr
        elif slotname is "SlotI":
            return self.adpd4000ch1inst.SLOTI_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTI_DataArr
        elif slotname is "SlotJ":
            return self.adpd4000ch1inst.SLOTJ_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTJ_DataArr
        elif slotname is "SlotK":
            return self.adpd4000ch1inst.SLOTK_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTK_DataArr
        elif slotname is "SlotL":
            return self.adpd4000ch1inst.SLOTL_DataArr if channeltype is 2 else self.adpd4000ch2inst.SLOTL_DataArr

    def append_data(self, slot_data, data_list):
        data_list.append(slot_data)

    def get_adpd4000_framesize(self):
        return self.NumPackets * self.sampleNum


