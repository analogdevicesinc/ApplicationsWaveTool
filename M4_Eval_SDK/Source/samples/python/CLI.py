'''******************************************************************************
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
******************************************************************************'''
import sys, subprocess, serial, threading, cmd, time, datetime, re, glob, queue, struct
import socket, os, errno
import colorama as cr
import binascii

if sys.platform == "linux" or sys.platform == "linux2":
    sys.path.append('./../../bin/linux/python/')
elif sys.platform == "darwin":
    sys.path.append('./../../bin/macOS/python/')
elif sys.platform == "win32":
    if ((struct.calcsize("P") * 8 ) == 64):
        sys.path.append('./../../bin/windows/x64/python/') #32 bit python
    else:
        sys.path.append('./../../bin/windows/Win32/python/') #64 bit python
elif sys.platform == "win64":
    sys.path.append('./../../bin/windows/x64/python/')
import gen3_sdk

python_sample_version = "4.0.1"

Fileobject = None
isWiredConn = None
Totalbytes = 0
filename = ""
Waitfor_cb_complete = False
thread_rx= False
stream_start = False
stream_start_ppg =False
stream_start_ecg =False
stream_start_eda =False
stream_start_adxl =False
stream_start_adpd4000 = False
stream_start_temperature = False
stream_start_bcm =False
stream_start_ped =False
stream_start_hrv =False


rx_thread =None
ser = serial.Serial()
bool_stream = True
bool_HC = False
source_found = False
destination_found = False
source_found2 = False
destination_found2 = False
con_status= False
msgverbose =False
connect_ble =False

m4_Platform = False

IsPacketTimedOut = False


IsADPDSupported = False
IsPPGSupported = False
IsECGSupported = False
IsEDASupported = False
IsBCMSupported = False
IsTEMPSuppoerted = False
IsPEDSupported = False
IsADPD4000Supported = False
IsAGCSupported = False
IsHRVSupported = False

sdk_packet = None
device_id = 10
file_status = {0: 'M2M2_APP_COMMON_STATUS_OK',
               1: 'M2M2_APP_COMMON_STATUS_ERROR',
               2: 'M2M2_APP_COMMON_STATUS_INVALID',
               3: 'M2M2_APP_COMMON_STATUS_DEVICE_ERROR'}

dict = {0: 'M2M2_APP_COMMON_STATUS_OK',
        1: 'M2M2_APP_COMMON_STATUS_ERROR',
        2: 'M2M2_APP_COMMON_STATUS_STREAM_STARTED',
        3: 'M2M2_APP_COMMON_STATUS_STREAM_STOPPED',
        4: 'M2M2_APP_COMMON_STATUS_STREAM_IN_PROGRESS',
        5: 'M2M2_APP_COMMON_STATUS_STREAM_DEACTIVATED',
        6: 'M2M2_APP_COMMON_STATUS_STREAM_COUNT_DECREMENT',
        7: 'M2M2_APP_COMMON_STATUS_STREAM_NOT_STARTED',
        8: 'M2M2_APP_COMMON_STATUS_STREAM_NOT_STOPPED',
        9: 'M2M2_APP_COMMON_STATUS_SUBSCRIBER_ADDED',
        64: 'M2M2_FILE_SYS_ERR_LOWEST',
        65: 'M2M2_FILE_SYS_STATUS_OK',
        66: 'M2M2_FILE_SYS_STATUS_ERROR',
        67: 'M2M2_FILE_SYS_END_OF_FILE',
        68: 'M2M2_FILE_SYS_END_OF_DIR',
        69: 'M2M2_FILE_SYS_ERR_INVALID',
        70: 'M2M2_FILE_SYS_ERR_ARGS',
        71: 'M2M2_FILE_SYS_ERR_FORMAT',
        72: 'M2M2_FILE_SYS_ERR_MEMORY_FULL',
        73: 'M2M2_FILE_SYS_ERR_LOG_FORCE_STOPPED',
        74: 'M2M2_FILE_SYS_ERR_MAX_FILE_COUNT',
        75: 'M2M2_FILE_SYS_CONFIG_FILE_FOUND',
        76: 'M2M2_FILE_SYS_CONFIG_FILE_NOT_FOUND',
        255: 'M2M2_FILE_SYS_ERR_NOT_CHKD',
        }

stream_and_log_status = {0:'OPERATION_NOT_SUPPORTED_FOR_THIS_DEVICE',
                         1:'UNKNOWN_DEVICE',
                         2:'LOAD_CFG_FAILED',
                         3:'CLOCK_CALIBRATE_FAILED',
                         4:'SET_SLOT_FAILED',
                         5:'SUBSCRIBE_FAILED',
                         6:'SUBSCRIBED',
                         7:'STREAM_START_FAILED',
                         8:'STREAM_STARTED',
                         9:'STREAM_STOP_FAILED',
                         10:'STREAM_STOPPED',
                         11:'UNSUBSCRIBE_FAILED',
                         12:'UNSUBSCRIBED',
                         13:'PPG_LOAD_LCFG_FAILED',
                         14:'LCFG_WRITE_FAILED',
                         15:'AD7689_START_FAILED',
                         16:'AD7689_STOP_FAILED',
                         17:'AD5940_START_FAILED',
                         18:'AD5940_STOP_FAILED',
                         19:'SET_FREQ_FAILED',
                         20:'FS_SUBSCRIBE_FAILED',
                         21:'FS_UNSUBSCRIBE_FAILED',
                         22:'FS_STATUS_ERROR',
                         23:'APP_COMMON_ERROR',
                         24:'FS_ERR_INVALID',
                         25:'FS_ERR_CONFIG_FILE_POSITION',
                         26:'FS_ERR_MAX_FILE_COUNT',
                         27:'LOGGING_NOT_STARTED',
                         28:'LOGGING_STARTED',
                         29:'LOGGING_STOPPED',
                         30:'FILE_NOT_CLOSED',
                         31:'FILE_NOT_OPENED',
                         32:'STREAM_IN_PROGRESS'}


file_status = {0: 'M2M2_APP_COMMON_STATUS_OK',
               1: 'M2M2_APP_COMMON_STATUS_ERROR',
               2: 'M2M2_APP_COMMON_STATUS_INVALID',
               3: 'M2M2_APP_COMMON_STATUS_DEVICE_ERROR'}

lowtouch = {64: '_M2M2_PM_SYS_STATUS_LOWEST',
            65: 'M2M2_PM_SYS_STATUS_OK',
            66: 'M2M2_PM_SYS_STATUS_ERR_ARGS',
            67: 'M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_ALREADY_STARTED',
            68: 'M2M2_PM_SYS_STATUS_CONFIG_FILE_NOT_FOUND',
            69: 'M2M2_PM_SYS_STATUS_CONFIG_FILE_READ_ERR',
            70: 'M2M2_PM_SYS_STATUS_ENABLE_USER_CONFIG_LOG_FAILED',
            71: 'M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_ENABLED',
            72: 'M2M2_PM_SYS_STATUS_DISABLE_USER_CONFIG_LOG_FAILED',
            73: 'M2M2_PM_SYS_STATUS_USER_CONFIG_LOG_DISABLED',
            74: 'M2M2_PM_SYS_STATUS_LOG_STOPPED_THROUGH_BUTTON_A',
            75: 'M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_IN_PROGRESS',
            76: 'M2M2_PM_SYS_STATUS_LOW_TOUCH_LOGGING_NOT_STARTED',
            255: 'M2M2_PM_SYS_STATUS_ERR_NOT_CHKD'}

BatteryState = {1: 'M2M2_PM_SYS_BAT_STATE_NOT_AVAIL',
                2: 'M2M2_PM_SYS_BAT_STATE_NOT_CHARGING',
                3: 'M2M2_PM_SYS_BAT_STATE_CHARGING',
                4: 'M2M2_PM_SYS_BAT_STATE_COMPLETE',
                5: 'M2M2_PM_SYS_BAT_STATE_DETECTION',
                6: 'M2M2_PM_SYS_BAT_STATE_CHARGE_ERR'}

low_touch_start_status = {

    64: 'M2M2_FILE_SYS_ERR_LOWEST',
    65: 'M2M2_FILE_SYS_STATUS_OK',
    66: 'M2M2_FILE_SYS_STATUS_ERROR',
    67: 'M2M2_FILE_SYS_END_OF_FILE',
    68: 'M2M2_FILE_SYS_END_OF_DIR',
    69: 'M2M2_FILE_SYS_ERR_INVALID',
    70: 'M2M2_FILE_SYS_ERR_ARGS',
    71: 'M2M2_FILE_SYS_ERR_FORMAT',
    72: 'M2M2_FILE_SYS_ERR_MEMORY_FULL',
    73: 'M2M2_FILE_SYS_ERR_LOG_FORCE_STOPPED',
    74: 'M2M2_FILE_SYS_ERR_MAX_FILE_COUNT',
    75: 'M2M2_FILE_SYS_CONFIG_FILE_FOUND',
    76: 'M2M2_FILE_SYS_CONFIG_FILE_NOT_FOUND',
    255: 'M2M2_FILE_SYS_ERR_NOT_CHKD',
    7: 'FILE_SYS_STREAM_NOT_STARTED',
    4: 'FILE_SYS_STREAM_IN_PROGRESS',
    0: 'M2M2_APP_COMMON_STATUS_OK',
    1: 'M2M2_APP_COMMON_STATUS_ERROR',
}

adpd_stream = {
            1: 'ADPD_SENSOR_STREAM1',
            2: 'ADPD_SENSOR_STREAM2',
            3: 'ADPD_SENSOR_STREAM3',
            4: 'ADPD_SENSOR_STREAM4',
            5: 'ADPD_SENSOR_STREAM5',
            6: 'ADPD_SENSOR_STREAM6',
            7: 'ADPD_SENSOR_STREAM7',
            8: 'ADPD_SENSOR_STREAM8',
            9: 'ADPD_SENSOR_STREAM9',
            10: 'ADPD_SENSOR_STREAM10',
            11: 'ADPD_SENSOR_STREAM11',
            12: 'ADPD_SENSOR_STREAM12',
            13: 'ADPD_SENSOR_INTERRUPT_STREAM',

}

sys_alert = {
            1: 'FILE_SYS_LOG_STOPPED_MEMORY_FULL',
            2: 'FILE_SYS_LOG_STOPPED_LOW_BATTERY',
            3: 'USER_CONFIG_AUTO_LOGGING_STARTED',
            4: 'USER_CONFIG_AUTO_LOGGING_STOPPED',
            5: 'USER_CONFIG_AUTO_LOGGING_NOT_STARTED_MEMORY_FULL_ERR',
            6: 'USER_CONFIG_AUTO_LOGGING_NOT_STARTED__MAX_FILE_ERR',
            7: 'FILE_SYS_LOG_STOPPED_THROUGH_BUTTON_A',
            8: 'USER_CONFIG_AUTO_LOGGING_WRIST_OFF_DETECTED',
            9: 'USER_CONFIG_AUTO_LOGGING_STOPPED_LOW_BATTERY',
            10: 'ADI_SDK_PACKET_TIMED_OUT',
}

ADPD107WATCH = 0
ADPD185WATCH = 1
ADPD188WATCH = 2
ADPD107CHESTSTRAP = 3
SENSORBOARD3 = 4
OPTICAL_BOARD_103 = 5
OPTICAL_BOARD_108 = 6
OPTICAL_BOARD_4000 = 7
OPTICAL_BOARD_4100 = 8

class fileWrite(threading.Thread):

    def __init__(self):

        self.isHeaderWritten = False
        threading.Thread.__init__(self)
        # thread = threading.Thread(target=self.write_file, args=[data])
        threading.Thread.daemon = True  # Daemonize thread
        self.fw_queue = queue.Queue()
        self.IsdataAvailable = True
        self.outputDirectory = "OutputLogs/"
        if not os.path.exists(self.outputDirectory):
            os.mkdir(self.outputDirectory)
        # self.createFile(filename)


    def createFile(self, filename):
        # self.sys_date_time = gen3_sdk.sys_date_time_t()
        # self.sys_date_time = self.get_sys_date_Time()
        sys_date_time = datetime.datetime.now()
        self.filename = self.outputDirectory + filename
        # print("date - {}".format(sys_date_time.year))
        self.filename += "{}{}{}{}{}{}.csv".format(sys_date_time.year, sys_date_time.month, sys_date_time.day,
                                                    sys_date_time.hour, sys_date_time.minute, sys_date_time.second)
        # print("filename - {}".format(self.filename))

    def displayFileName(self):
        if self.filename:
            return self.filename
        else:
            return ""


    def run(self):
        while self.IsdataAvailable:
            # print("Queue -o/p - {}".format(self.adxlqueue.qsize()))
            while self.fw_queue.qsize() > 0 and self.isHeaderWritten:
                # print("Queue -o/p - {}".format(self.adxlqueue.get()))
                with open(self.filename, "a") as f:
                    f.write(self.fw_queue.get())

    def stopFileWrite(self):
        self.IsdataAvailable = False
        self.isHeaderWritten = False
        self.fw_queue.empty()
        print("File was written into a path ..",self.displayFileName())


def ref(self,filename,adpddata):
    while True:
        with open(self.filename, "a") as f:
            f.write(self.adpddata)

class serial_tx_cb(gen3_sdk.watch_phy_callback):
    def __init__(self, serial_object):
        gen3_sdk.watch_phy_callback.__init__(self)
        self.serial = serial_object

    def call(self, data):
        global IsPacketTimedOut,sdk_packet,msgverbose
        IsPacketTimedOut = False
        self.serial.write(data)
        sdk_packet = data
        if (msgverbose == True):
            vrb.write("TX : " + ''.join('{:02x}'.format(x) for x in data),3)
        # print (''.join('{:02x}'.format(x) for x in data))
        # print("TX data: {}".format(hex(i)))
    def sys_alert_call(self,alert):
        global IsPacketTimedOut
        IsPacketTimedOut = True
        print("SDK ALERT : {}".format(sys_alert[alert]))


class adxl_cb(gen3_sdk.adxl_stream_callback):
    def __init__(self):
        gen3_sdk.adxl_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("ADXL STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} x:{} y:{} z:{}\n".format(d.timestamp, d.x, d.y, d.z), 3)

class agc_cb(gen3_sdk.agc_stream_callback):
    def __init__(self):
        gen3_sdk.agc_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("AGC STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} mts0:{} mts1:{} mts2:{} mts3:{} mts4:{} mts5:{} setting0:{} setting1:{} setting2:{} setting3:{} setting4:{} setting5:{} setting6:{} setting7:{} setting8:{} setting9:{} \n".format(d.timestamp, d.mts0, d.mts1, d.mts2, d.mts3, d.mts4, d.mts5, d.setting0, d.setting1, d.setting2, d.setting3, d.setting4, d.setting5, d.setting6, d.setting7, d.setting8, d.setting9), 3)

class ecg_cb(gen3_sdk.ecg_stream_callback):
    def __init__(self):
        gen3_sdk.ecg_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("ECG STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} data:{} HR:{} leads_off:{} data_type:{}\n".format(d.timestamp, d.ecg_data, d.HR, d.leadsoff,
                                                                            d.datatype), 3)


class temperature_cb(gen3_sdk.temperature_stream_callback):
    def __init__(self):
        gen3_sdk.temperature_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("TEMP STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t: {} Skin temp: {} Ambient temp: {}\n".format(d.timestamp, d.temp_skin, d.temp_ambient), 3)


class eda_cb(gen3_sdk.eda_stream_callback):
    def __init__(self):
        gen3_sdk.eda_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("EDA STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} admittance_real: {} admittance_img: {} impedance_real:{} impedance_img: {} admittance_magnitude: {} admittance_phase: {} impedance_magnitude: {} impedance_phase: {}\n".format(d.timestamp, d.admittance_real, d.admittance_img, d.impedance_real, d.impedance_img, d.admittance_magnitude, d.admittance_phase, d.impedance_magnitude, d.impedance_phase), 3)

class adpd_cb(gen3_sdk.adpd_stream_callback):
    def __init__(self):
        gen3_sdk.adpd_stream_callback.__init__(self)
        self.seq_num_A = None
        self.seq_num_B = None

    def call(self, data, sequence_num):
        # print("ADPD STREAM CALLBACK::")
        for d in data:
            #sequence number check for slot A
            if (d.datatype == gen3_sdk.RAW_DATA_ADPD_A_SUM_16b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_A_SUM_32b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_A_4CH_16b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_A_4CH_32b):
                # Python has ints of unlimited precision, so we have to manually do a 16 bit rollover.
                if self.seq_num_A == 65535:
                    self.seq_num_A = -1
                if self.seq_num_A == None:
                    self.seq_num_A = sequence_num
                elif self.seq_num_A + 1 != sequence_num:
                    print("ADPD STREAM SLOT A SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num_A + 1, sequence_num))
                self.seq_num_A = sequence_num
                break
            # sequence number check for slot B
            elif (d.datatype == gen3_sdk.RAW_DATA_ADPD_B_SUM_16b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_B_SUM_32b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_B_4CH_16b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_B_4CH_32b):
                # Python has ints of unlimited precision, so we have to manually do a 16 bit rollover.
                if self.seq_num_B == 65535:
                    self.seq_num_B = -1
                if self.seq_num_B == None:
                    self.seq_num_B = sequence_num
                elif self.seq_num_B + 1 != sequence_num:
                    print(
                        "ADPD STREAM SLOT B SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num_B + 1, sequence_num))
                self.seq_num_B = sequence_num
                break

        for d in data:
            if(d.datatype == gen3_sdk.RAW_DATA_ADPD_B_SUM_32b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_A_SUM_32b) :
                vrb.write("datatype{} t:{} adpd:{}\n".format(d.datatype, d.sum_timestamp, d.sum32_adpd_data), 3)
            if (d.datatype == gen3_sdk.RAW_DATA_ADPD_A_SUM_16b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_B_SUM_16b):
                vrb.write("datatype{} t:{} adpd:{}\n".format(d.datatype, d.sum_timestamp, d.sum16_adpd_data), 3)
            if(d.datatype == gen3_sdk.RAW_DATA_ADPD_A_4CH_16b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_B_4CH_16b):
                vrb.write("\ndatatype:{} t:{}\n".format(d.datatype, d.ch_timestamp), 3)
                for x in range(0, d.ch_data.size()):
                    vrb.write("Chdata:{}\n".format(d.ch_data[x]), 3)
            if(d.datatype == gen3_sdk.RAW_DATA_ADPD_A_4CH_32b) or (d.datatype == gen3_sdk.RAW_DATA_ADPD_B_4CH_32b):
                vrb.write("\ndatatype:{} t:{}\n".format(d.datatype, d.ch_timestamp), 3)
                for x in range(0, d.ch_data.size()):
                    vrb.write("Chdata :{}\n".format(d.ch_data[x]), 3)

class ppg_cb(gen3_sdk.ppg_stream_callback):
    def __init__(self):
        gen3_sdk.ppg_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("PPG STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} adpdlibstate: {} hr: {} confidence:{} hr_type: {} rr interval: {}\n".format(d.timestamp,
                                                                                                       d.adpdlibstate,
                                                                                                       d.hr,
                                                                                                       d.confidence,
                                                                                                       d.hr_type,
                                                                                                       d.rr_interval), 3)

class syncppg_cb(gen3_sdk.syncppg_stream_callback):
    def __init__(self):
        gen3_sdk.syncppg_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("SYNCPPG STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} PPG: {} X: {} Y:{} Z: {}\n".format(d.timestamp, d.ppg, d.x, d.y, d.z), 3)


class pedometer_cb(gen3_sdk.pedometer_stream_callback):
    def __init__(self):
        gen3_sdk.pedometer_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("PEDOMETER STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("TimeStamp: {} NumSteps:{} AlgoStatus: {}\n".format(d.TimeStamp, d.NumSteps, d.AlgoStatus), 3)


class bcm_cb(gen3_sdk.bcm_stream_callback):
    def __init__(self):
        gen3_sdk.bcm_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("BCM STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} impedance_real:{} impedance_imgdata:{} is_finger_on_leads:{} signal_stability:{} impedance_module:{} impedance_phase:{} admittance_module:{} admittance_phase:{} \n".format(d.timestamp, d.impedance_real, d.impedance_img, d.is_finger_on_leads, d.signal_stability, d.impedance_magnitude, d.impedance_phase, d.admittance_magnitude, d.admittance_phase), 3)

class hrv_cb(gen3_sdk.hrv_stream_callback):
    def __init__(self):
        gen3_sdk.hrv_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        if self.seq_num == None:
            self.seq_num = sequence_num
        elif self.seq_num + 1 != sequence_num:
            print("HRV STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            vrb.write("t:{} rr_interval: {} gap: {}\n".format(d.timestamp, d.rr_interval, d.is_gap), 3)

class adpd4000_cb(gen3_sdk.adpd4000_stream_callback):
    def __init__(self):
        gen3_sdk.adpd4000_stream_callback.__init__(self)
        self.seq_num = None

    def call(self, data, sequence_num):
        # print("ADPD4000 STREAM CALLBACK::")
        for d in data:
            if d.adpd_stream == gen3_sdk.ADPD_SENSOR_INTERRUPT:
                vrb.write(
                        "Source: {}\n Timestamp: {}\ndata_int: {}\nLevel0_int: {}\nLevel1_int: {}\ntiaCH1_int: {}\ntiaCH2_int: {}\n".format(
                            adpd_stream[d.adpd_stream], d.timestamp, d.data_int, d.level0_int, d.level1_int, d.tia_ch1_int, d.tia_ch2_int), 3)

            else:

                if d.adpd_stream == 1: # Sequence Check for ADPD Stream 1
                    # Python has ints of unlimited precision, so we have to manually do a 16 bit rollover.
                    if self.seq_num == 65535:
                        self.seq_num = -1

                    if self.seq_num == None:
                        self.seq_num = sequence_num
                    elif self.seq_num + 1 != sequence_num:
                        print("adpd4000 STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1,
                                                                                                sequence_num))
                    self.seq_num = sequence_num
                vrb.write("source: {}\nDark: {}\nSignal: {}\nImpulse: {}\nChannel Number: {}\nTimestamp: {}\n".format(
                    adpd_stream[d.adpd_stream], d.dark, d.signal, d.impulse, d.channel_num, d.timestamp), 3)
                for x in range(0, d.adpddata_d.size()):
                    vrb.write("adpd_data_d[{}]:{}\n".format(x,d.adpddata_d[x]),3)
                for x in range(0, d.adpddata_s.size()):
                    vrb.write("adpd_data_s[{}]:{}\n".format(x,d.adpddata_s[x]),3)

class batteryStream_cb(gen3_sdk.battery_stream_callback):
    def __init__(self):
        gen3_sdk.battery_stream_callback.__init__(self)

    def call(self, data):
        for d in data:
            vrb.write("timestamp: {} bat_chrg_stat: {} bat_lvl:{} bat_mv: {} bat_temp: {}\n".format(d.timestamp,
                                                                                                  d.bat_chrg_stat,
                                                                                                  d.bat_lvl, d.bat_mv,
                                                                                                  d.bat_temp), 3)

class fs_ls_cb(gen3_sdk.fs_ls_callback):
    def __init__(self):
        gen3_sdk.fs_ls_callback.__init__(self)

    def call(self, data):
        global Waitfor_cb_complete
        for d in data:
            if d.status != gen3_sdk.fs_application.FILE_SYS_END_OF_DIR:
                if d.filetype == gen3_sdk.fs_application.FS_SYS_IS_FILE:
                    file = "FILE"
                else:
                    file = "DIRECTORY"

                print("full_file_name: {}\n"
                      "filetype      : {}\n"
                      "filesize      : {}\n".format(d.filename, file, d.filesize))
            else:
                print("END OF DIRECTORY")
                Waitfor_cb_complete = False


class fs_stream_cb(gen3_sdk.fs_stream_callback):
    def __init__(self):
        gen3_sdk.fs_stream_callback.__init__(self)

    def call(self, data):
        global Fileobject, Filename, Totalbytes
        for d in data:
            bytelist = []
            Totalbytes = Totalbytes + d.length_Stream
            for x in range(0, d.byte_Stream.size()):
                bytelist.append((d.byte_Stream[x]))
            bytelist = bytearray(bytelist[0:d.length_Stream])
            Fileobject.write((bytelist))
            if d.status == gen3_sdk.fs_application.FILE_SYS_END_OF_FILE:
                try:
                    Fileobject.close()
                    Fileobject = None
                except ValueError:
                    print("Error in Closing file")


class verboser():
    msg_formatters = {
        0: None,  # A placeholder for not printing anything
        1: {"fmt": "{}", "help": "Regular old prints"},
        2: {"fmt": cr.Back.WHITE + cr.Fore.BLACK + "{}", "help": "More chatter (i.e. Which command was just run)"},
        3: {"fmt": cr.Back.CYAN + cr.Fore.BLACK + "{}",
            "help": "Underlying transaction info (i.e. raw packet data)"},
        4: {"fmt": cr.Back.MAGENTA + cr.Fore.CYAN + "{}",
            "help": "CLI Interior workings (threads starting, sockets opened, etc)"},
    }
    err_formatter = cr.Back.RED + cr.Fore.GREEN + "ERR: {}"
    level = 1
    console_socket = None
    console = None
    port = 1069

    def __init__(self, console_level=2):
        self.console_level = console_level

    def __del__(self):
        self.stop_console()

    def write(self, msg, level=1):
        if ((self.level == 0) or (level == 0)):
            return
        if level <= self.level:
            outstr = ""
            whitespace = ""
            whitespace = msg[:len(msg) - len(msg.lstrip())]
            msg_str = msg[len(msg) - len(msg.lstrip()):]
            output_str = whitespace + self.msg_formatters[level]["fmt"].format(msg_str)
            if level >= self.console_level and self.console_socket != None:
                self.console_write(output_str)
            else:
                print(output_str)

    def console_write(self, text, level=1):
        if self.console != None:
            try:
                self.console_socket.send(text.encode())
            except socket.error as e:
                if e.errno == errno.ENETRESET or e.errno == errno.ECONNABORTED or e.errno == errno.ECONNRESET:
                    self.err("Socket error: {}".format(e), force_print=True)
                    self.err("Attempting to restart the console...", force_print=True)
                    self.stop_console()
                    self.start_console()
                else:
                    self.err("Socket error: {}".format(e), force_print=True)

    def err(self, msg, level=1, force_print=False):
        output_str = self.err_formatter.format(msg)
        if self.console_socket != None and self.level >= self.console_level and not force_print:
            self.console_write(output_str)
        else:
            print(output_str)

    def set_level(self, level):
        self.level = level
        if level >= self.console_level and self.console == None:
            self.start_console()
        elif level <= self.console_level and self.console_socket != None:
            self.stop_console()

    def start_console(self):
        if "nt" in os.name:
            self.console = subprocess.Popen("start python.exe console_window.py {}".format(self.port), shell=True)
        else:
            self.console = subprocess.Popen("exec xterm -e \"python console_window.py {}\"".format(self.port),
                                            shell=True)
        try:
            self.console_socket = socket.socket()
            self.console_socket.connect(('localhost', self.port))
        except socket.error as e:
            self.err("Couldn't open the socket to the secondary console: {}".format(e))

    def stop_console(self):
        if(self.console_socket):
            self.console_socket.close()
            self.console.terminate()
            self.console_socket = None
            self.console = None


vrb = verboser()


class sdk_shell(cmd.Cmd):
    # cr.init(autoreset=True)
    # Hackily instantiate an SDK object so we can get the version, then allow it to be garbage collected.
    if sys.platform == "linux" or sys.platform == "linux2":
        intro = 'Welcome to the ADI SDK testing CLI.  Type help or ? to list the available commands.  \nPython sample version : {} \nThe SDK version : {}\nSupported python version : 3.6(64 bit)\n'.format(
        python_sample_version, gen3_sdk.watch.SDK_version)
    # prompt = cr.Fore.GREEN + '#>' + cr.Style.RESET_ALL
    elif sys.platform == "win32" or sys.platform == "win64":
        intro = 'Welcome to the ADI SDK testing CLI.  Type help or ? to list the available commands.  \nPython sample version : {} \nThe SDK version : {}\nSupported python version : 3.7 (32 bit)\n'.format(
        python_sample_version, gen3_sdk.watch.SDK_version)
    # prompt = cr.Fore.GREEN + '#>' + cr.Style.RESET_ALL

    prompt = '#>'

    def __init__(self):
        cmd.Cmd.__init__(self)
        self.watch_connections = {}
        self.active_watch = None
        self.active_watch_name = None
        self.unconnected_commands = [
            "connect",
            "exit",
            "quit",
            "help",
            "?",
            "scan_ble",
            "connect_ble",
            "connect_bt",
            "msg_verbose"
        ]

    class watch_connection():
        def __init__(self, serial_obj):
            self.serial = serial_obj
            self.cb_obj = serial_tx_cb(serial_obj)
            self.watch = gen3_sdk.watch(self.cb_obj.__disown__())
            self.thread_run = True
            self.serial_rx_thread = threading.Thread(target=self.rx_fn)
            self.serial_rx_thread.setDaemon(True)
            self.serial_rx_thread.start()
            self.watch.initialize_sdk(gen3_sdk.watch.python)

        def close(self):
            self.thread_run = False
            self.serial_rx_thread.join()
            self.cb_obj.serial.close()

        def rx_fn(self):
            while (self.thread_run):
                hdr = self.serial.read(8)
                length = (hdr[4] << 8) + (hdr[5])
                body = self.serial.read(length - 8)
                pkt = hdr + body
                #print("RX THREAD:: DISPATCHING PACKET\n", binascii.hexlify(pkt))
                self.watch.dispatch(pkt)

    def do_exit(self, arg):
        """Exit the shell."""
        quit()

    def _parse_args(self, arg, num_args=None):
        """
        Check that an 'arg' string contains the correct number of arguments,
        and returns a list of those arguments. Set num_args = None to prevent
        checking.
        """
        args = arg.split()
        if num_args == None:
            return args
        if len(args) != num_args:
            # self.vrb.err("Incorrect number of arguments! You passed: '{}'".format(arg))
            return None
        else:
            return args

    def precmd(self, arg):
        if self.active_watch == None:
            args = arg.split()
            for cmd in self.unconnected_commands:
                for a in args:
                    if a == cmd or a == "?" + cmd:
                        return arg
            print('You must be connected to a device to use this command.')
            return 'help connect'
        else:
            return arg



    def do_connect(self, arg):
        args = self._parse_args(arg)
        global isWiredConn, device_id
        if len(args) < 1:
            print('Please provide a serial port to connect to.')
            return
        try:
            serial_port = args[0]
            if len(args) == 2:
                watch_key = args[1]
            else:
                watch_key = serial_port
            print('Connecting to watch...')

            # Add a timeout to serial reads?
            self.watch_connections[watch_key] = self.watch_connection(serial.Serial(serial_port, 921600))
            print('Connected to watch: ' + watch_key)
            isWiredConn = True
            self.active_watch = self.watch_connections[watch_key].watch

            self.GetDeviceid()
            if device_id <= SENSORBOARD3:
                self.onecmd("get_version ps")
            self.onecmd("get_version pm")
            self.do_help(0)

        except serial.serialutil.SerialException as e:
            print("Error opening the serial device!")
            print("The port might be already selected,or have given an incorrect serial device identifier.")
            print("Error was:\n\t{}".format(e))
            return

    def help_connect(self):
        print("Connect to a watch connected over a serial port")
        print("  Example usage:")
        print("  #>connect [serial port]")
        print("  #>connect /dev/tty.usbserial-DM003MI1")
        print("  #>connect COM33 ")
        print("Wait a moment, searching for available serial ports...")

        # Find a list of available serial ports
        result = []
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass

        print("Available serial ports are:")
        for p in result:
            print("==> {}".format(p))

    def do_connect_ble(self, arg):
        args = self._parse_args(arg, 2)
        global isWiredConn
        if args == None:
            print("Invalid arguments! You must provide a BLE dongle serial address and a device index given by the 'scan_ble' command!")
            self.help_scan_ble()
            return
        serial_port = args[0]
        try:
            ble_device_index = int(args[1])
        except:
            print(
                "Invalid arguments! You must provide a BLE dongle serial address and an index given by the 'scan_ble' command!")
            self.help_scan_ble()
            return
        with serial.Serial(serial_port, 115200, timeout=2) as s:
            s.reset_input_buffer()
            print("Connecting to dongle...")
            s.write(b"AT+RECN=1\r\n")
            resp = self._ble_serial_read(s)
            if resp == b"+OK":
                print("Connected! Connecting to BLE device...")
                connect_cmd = bytearray("AT+CONN={}\r\n".format(
                    self.ble_device_responses[ble_device_index]
                        .split(",")[0]
                        .strip("+")),
                    'ascii')
                s.write(connect_cmd)
                time.sleep(4)
                resp = self._ble_serial_read(s)
                if resp == b"+OK":
                    resp = self._ble_serial_read(s)
                    if resp == b"+DEVCONN":
                        print("BLE device with serial address '{}' is connected!".format(serial_port))
                        isWiredConn = False
                        s.close()
                        self.watch_connections[serial_port] = self.watch_connection(serial.Serial(serial_port, 115200))
                        self.active_watch = self.watch_connections[serial_port].watch
                        self.GetDeviceid()
                        if device_id <= SENSORBOARD3:
                            self.onecmd("get_version ps")
                        self.onecmd("get_version pm")
                        self.do_help(0)

    def help_connect_ble(self):
        print("Connect to an ADI BLE dongle instruct it to connect to a watch.")
        print("Sample scan_ble output:")
        print("Device index, device ID")
        print("0, +00,62c2b922e000,13,ADIVSM_B9C262,102")
        print("1, +01,91a06022e000,13,ADIVSM_60A091,129")
        print("  Example usage:")
        print("  #>connect_ble [dongle serial port] [BLE device index returned by scan_ble]")
        print("  #>connect_ble /dev/tty.usbserial-DM003MI1 0")

    def do_scan_ble(self, arg):
        ble_scan_time = 5
        args = self._parse_args(arg, 1)
        if args == None:
            print("You did not provide the BLE dongle's serial port address!")
            return
        serial_port = args[0]
        with serial.Serial(serial_port, 115200, timeout=2) as s:
            s.reset_input_buffer()
            # Scan for BLE devices
            print("Connecting to dongle...")
            s.write(b"AT+SCAN\r\n")
            resp = self._ble_serial_read(s)
            print("Dongle found!")
            if resp == b"+OK":
                print("Scanning for BLE devices for {} seconds...".format(ble_scan_time))
                # Give the dongle time to scan
                time.sleep(ble_scan_time)
                self.ble_device_responses = []
                while True:
                    resp = self._ble_serial_read(s)
                    if not resp:
                        break
                    print("Found device:{}".format(resp.decode("ascii")))
                    self.ble_device_responses.append(resp.decode("ascii"))
                if len(self.ble_device_responses) == 0:
                    print("No BLE devices found! Try again.")
                    return
                else:
                    print("{} devices found:".format(len(self.ble_device_responses)))
                    print("Device index, device ID")
                    for i, r in enumerate(self.ble_device_responses):
                        print("{}, {}".format(i, r))
            else:
                print("Error connecting to the dongle!")
                print("Dongle response: '{}'".format(resp.decode("ascii")))

    def help_scan_ble(self):
        print("Connect to an ADI BLE dongle and scan for watches.")
        print("  Example usage:")
        print("  #>scan_ble [dongle serial port]")
        print("  #>scan_ble /dev/tty.usbserial-DM003MI1")

    def opl_connect_bt(self, arg):
        args = self._parse_args(arg)
        if len(args) < 1:
            print('Please provide a serial port to connect to.')
            return

        try:
            serial_port = args[0]
            if len(args) >= 2:
                # watch_key = args[1]
                print("Invalid arguments! please type help <command>(help connect_bt) to know the usage.")
                return
            else:
                watch_key = serial_port
            if (ser.isOpen() == True):
                print("Port Already Connected,Please disconnect and try again")
                return

            print('Connecting to motherboard...')
            ser.baudrate = 115200
            ser.port = serial_port
            ser.open()
            # Add a timeout to serial reads?
            ser.write(b"START\r\n")
            self.watch_connections[watch_key] = self.watch_connection(ser)
            # self.watch_connection(ser).thread_start()
            print('Connected to motherboard: ' + watch_key)

            self.active_watch = self.watch_connections[watch_key].watch
            self.GetDeviceid()
            if device_id <= SENSORBOARD3:
                self.onecmd("get_version ps")
            self.onecmd("get_version pm")
            self.do_help(0)


        except serial.serialutil.SerialException as e:
            print("Error opening the serial device!")
            print("The port might be already selected,or have given an incorrect serial device identifier.")
            print("Error was:\n\t{}".format(e))
            return

    def helpOpl_connect_bt(self):
        print("Connect to a motherboard connected over bluetooth")
        print("  Example usage:")
        print("  #>connect_bt [BT port]")
        print("  #>connect_bt COM52 ")
        print("Wait a moment, searching for available Serial ports...")

        # Find a list of available serial ports
        result = []
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass

        print("Available serial ports are:")
        for p in result:
            print("==> {}".format(p))

    def opl_disconnect_bt(self, arg):
        global thread_rx, rx_thread, stream_start, stream_start_adxl, stream_start_adpd4000, adpdSlotA_FW, adpdSlotB_FW, adxlFileWrite, adpd4000FileWrite
        try:
            self.active_watch.pm_app.bluetooth_request()
            if stream_start == True:
                self.active_watch.stop_adpd()
                if adpdSlotA_FW != None and msgverbose == False:
                    adpdSlotA_FW.stopFileWrite()
                    adpdSlotA_FW = None
                if adpdSlotB_FW != None and msgverbose == False:
                    adpdSlotB_FW.stopFileWrite()
                    adpdSlotB_FW = None
                stream_start = False
            if stream_start_adxl == True:
                self.active_watch.stop_adxl()
                if adxlFileWrite != None and msgverbose == False:
                    adxlFileWrite.stopFileWrite()
                    adxlFileWrite = None
                stream_start_adxl = False
            if stream_start_adpd4000 == True:
                self.active_watch.stop_adpd4000()
                if adpd4000FileWrite != None and msgverbose == False:
                    adpd4000FileWrite.stopFileWrite()
                    adpd4000FileWrite = None
                stream_start_adpd4000 = False
            self.watch_connection.close(self)
            self.active_watch = None
            ser.close()
            print("BT disconnected")

        except serial.serialutil.SerialException as e:
            print("Error opening the serial device!")
            print("The port might be already selected,or have given an incorrect serial device identifier.")
            print("Error was:\n\t{}".format(e))
            return


    def helpOpl_disconnect_bt(self):
        print("Disconnect the device")
        print("Example usage:")
        print("disconnect_bt")

    def _ble_serial_read(self, serial_device):
        packet = bytearray()
        while True:
            c = serial_device.read(1)
            if not c:
                break
            elif c != b'\r' and c != b'\n':
                packet += c
            elif c == b'\r':
                c = serial_device.read(1)
                if c == b'\n':
                    break
        return packet

    def GetDeviceid(self):
        global device_id, IsADPDSupported, IsPPGSupported, IsECGSupported, IsEDASupported, IsBCMSupported, IsTEMPSuppoerted, IsPEDSupported, IsADPD4000Supported, IsAGCSupported, IsHRVSupported
        device_id = self.active_watch.getDeviceID()
        # device_id = SENSORBOARD3
        if device_id > SENSORBOARD3:
            self.check_adpd_or_adpd4000()
        if device_id == OPTICAL_BOARD_4000 or device_id == OPTICAL_BOARD_4100:
            IsADPD4000Supported = True
            IsPPGSupported = True
        elif device_id == OPTICAL_BOARD_103 or device_id == OPTICAL_BOARD_108:
            IsADPDSupported = True


    def check_adpd_or_adpd4000(self):
        global  m4_Platform, IsPPGSupported
        m4_Platform = False
        mcu_type = self.active_watch.pm_app.get_mcu_version()
        # Check Mother board Type
        if mcu_type == self.active_watch.pm_app.PM_SYS_MCU_M3:
            m4_Platform = False
        elif mcu_type == self.active_watch.pm_app.PM_SYS_MCU_M4:
            system_Info = gen3_sdk.pm_sys_info_t()
            self.active_watch.pm_app.get_system_info(system_Info);
            if system_Info.hw_id == 0x9031 or system_Info.hw_id == 0x9030 or system_Info.hw_id == 0x9010 or system_Info.hw_id == 0x9011 or  system_Info.hw_id == 0x9130 or system_Info.hw_id == 0x9131:
                IsPPGSupported = True
            else:
                IsPPGSupported = False
            m4_Platform = True


    def watch_set_led(self, arg):
        args = self._parse_args(arg, 2)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_led) to know the usage.")
            return
        try:
            r_set = int(args[0])
            g_set = int(args[1])
            if r_set > 255:
                r_set = 255
            if g_set > 255:
                g_set = 255
        except Exception as e:
            print("Error parsing arguments! Error was: " + e)
            print(e)
            return

        self.active_watch.led_app.set_led(r_set, g_set)

    def helpWatch_set_led(self):
        print("Set the LED pattern")
        print("  Example usage:")
        print("  #>set_led [red pattern] [green pattern]")
        print("  #>set_led 255 0")

    def watch_get_led(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help get_led) to know the usage.")
            return

        r_set, g_set = self.active_watch.led_app.get_led()
        print("r = 0b{0:08b}".format(r_set))
        print("g = 0b{0:08b}".format(g_set))

    def helpWatch_get_led(self):
        print("Gets the current status of the device LED.")
        print("  Example usage:")
        print("  #>get_led")

    def watch_set_decimation(self, arg):
        args = self._parse_args(arg, 2)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_decimation) to know the usage.")
            return

        try:
            decimationFactor = int(args[1])
            if (1 <= decimationFactor <= 9):

                if "syncppg" in args:
                    status = self.active_watch.syncppg_app.syncppg_stream.set_decimation_factor(decimationFactor)
                    print("Decimation factor set: {} \nStatus: {} ".format(decimationFactor, file_status[status]))

                elif "ecg" in args:
                    status = self.active_watch.ecg_app.ecg_stream.set_decimation_factor(decimationFactor)
                    print("Decimation factor set: {} \nStatus: {} ".format(decimationFactor, file_status[status]))

                elif "adpd" in args:
                    status = self.active_watch.adpd_app.adpd_stream.set_decimation_factor(decimationFactor)
                    print("Decimation factor set: {} \nStatus: {} ".format(decimationFactor, file_status[status]))

                elif "adxl" in args:
                    status = self.active_watch.adxl_app.adxl_stream.set_decimation_factor(decimationFactor)
                    print("Decimation factor set: {} \nStatus: {} ".format(decimationFactor, file_status[status]))

                elif "bcm" in args:
                    status = self.active_watch.bcm_app.bcm_stream.set_decimation_factor(decimationFactor)
                    print("Decimation factor set: {} \nStatus: {} ".format(decimationFactor, file_status[status]))
                else:
                    print("Error in parsing arguments! you did not provide a valid stream.")
            else:
                print("The decimation Factor limit between 1 to 9")

        except ValueError:
            print("Error parsing arguments! Please enter a valid integer number")


    def helpWatch_set_decimation(self):
        print("Set the decimation factor for stream samples. The argument is the stream and decimation factor:")
        print("Note that decimation of samples is limited to adpd, adxl, ecg, syncppg and bcm streams")
        print("  #>decimation factor value limit [1-9]")
        print("  Example usage:")
        print("  #>set_decimation [syncppg | ecg] [decimationFactor]")
        print("  #>set_decimation  syncppg 1")

    def watch_get_decimation(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help get_decimation) to know the usage.")
            return

        if "syncppg" in args:
            dec_factor = self.active_watch.syncppg_app.syncppg_stream.get_decimation_factor()
            print(dec_factor)

        elif "ecg" in args:
            dec_factor = self.active_watch.ecg_app.ecg_stream.get_decimation_factor()
            print(dec_factor)

        elif "adpd" in args:
            dec_factor = self.active_watch.adpd_app.adpd_stream.get_decimation_factor()
            print(dec_factor)

        elif "adxl" in args:
            dec_factor = self.active_watch.adxl_app.adxl_stream.get_decimation_factor()
            print(dec_factor)

        else:
            print("Error in parsing arguments! you did not provide a valid stream.")

    def helpWatch_get_decimation(self):
        print("Get the decimation factor for stream samples.The argument is the stream")
        print("Note that decimation of samples is limited to adpd, adxl, ecg and syncppg streams")
        print("  Example usage:")
        print("  #>get_decimation syncppg")
    def watch_powerboostm3(self, arg):
        args = self._parse_args(arg, 2)
        enable = None
        OVPValue = 0
        if args == None:
            print("Invalid arguments! please type help <command>(help powerboostm3) to know the usage.")
            return


        if "enable" in args[0]:
            enable = 0
        elif "disable" in args[0]:
            enable = 1

        str_length = len(args[1]);


        if "18.5" in args[1]:
            OVPValue = 0
        elif "15" in args[1]:
            OVPValue = 1
        elif"10" in args[1]:
            OVPValue = 2
        elif "5" in args[1]:
            OVPValue = 3
        else:
            print("Invalid arguments!")
            return
        print(OVPValue)

        mcutype = self.active_watch.pm_app.get_mcu_version()
        if mcutype == 1:
            status = self.active_watch.pm_app.powerboost_m3(enable, OVPValue)
            if status == 0:
                print("Application Status: 'M2M2_APP_COMMON_STATUS_OK'")
            else:
                print("Application Status: 'M2M2_APP_COMMON_STATUS_ERROR'")
        else:
            print("This command supports only for M3 Motherboard")
            return

    def helpWatch_powerboostm3(self):
        print("It's used to set the powerboost only for M3 MotherBoard:")
        print("  Example usage:")
        print("  OVPValue parameter value should be 18.5 or 15 or 10 or 5")
        print("  #> powerboostm3 enable OVPValue")

    def watch_get_algo_vendor_version(self, arg):
        args = self._parse_args(arg, 1)

        if args == None:
            print("Invalid arguments! please type help <command>(help get_algo_vendor_version) to know the usage.")
            return

        algo_versionInfo = gen3_sdk.common_app_version_t()
        if "ppg" in args:
            self.active_watch.ppg_app.get_algo_vendor_version(algo_versionInfo)
            print("major:{} \nminor: {} \npatch: {} \nversion:{} \nvendor: {}".format(algo_versionInfo.major,
                                                                                      algo_versionInfo.minor,
                                                                                      algo_versionInfo.patch,
                                                                                      algo_versionInfo.verstr,
                                                                                      algo_versionInfo.vendstr))

        elif "ecg" in args:
            self.active_watch.ecg_app.get_algo_vendor_version(algo_versionInfo)
            print("major:{} \nminor: {} \npatch: {} \nverstr:{} \nstr: {}".format(algo_versionInfo.major,
                                                                                  algo_versionInfo.minor,
                                                                                  algo_versionInfo.patch,
                                                                                  algo_versionInfo.verstr,
                                                                                  algo_versionInfo.vendstr))

        elif "pedometer" in args:
            self.active_watch.ped_app.get_algo_vendor_version(algo_versionInfo)
            print("major:{} \nminor: {} \npatch: {} \nverstr:{} \nstr: {}".format(algo_versionInfo.major,
                                                                                  algo_versionInfo.minor,
                                                                                  algo_versionInfo.patch,
                                                                                  algo_versionInfo.verstr,
                                                                                  algo_versionInfo.vendstr))
        else:
            print("Error in parsing arguments!")

    def helpWatch_get_algo_vendor_version(self):
        print("get algo vendor version for particular application")
        print("  Example usage:")
        print("   Available application ppg,ecg,pedometer")
        print("  #> get_algo_vendor_version ppg")
        print("  #> get_algo_vendor_version ecg")

    def do_get_version(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help get_version) to know the usage.")
            return

        version_Info = gen3_sdk.common_app_version_t()
        if "pm" in args:
            self.active_watch.pm_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_PM':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "ps" in args:
            self.active_watch.ps_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_PS':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "ppg" in args:
            self.active_watch.ppg_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_PPG':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "ecg" in args:
            self.active_watch.ecg_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_ECG':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "eda" in args:
            self.active_watch.eda_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_EDA':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "temperature" in args:
            self.active_watch.temperature_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_TEMPERATURE':\nmajor: {} \nminor: {} \npatch: {} \nversion:{} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "adpd" in args:
            self.active_watch.adpd_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_ADPD':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "adxl" in args:
            self.active_watch.adxl_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SYS_ADXL':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))
        elif "ad7689" in args:
            self.active_watch.adxl_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_SENSOR_AD7689':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))

        elif "pedometer" in args:
            self.active_watch.ped_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_MED_PED':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))
        elif "bcm" in args:
            self.active_watch.bcm_app.get_version(version_Info)
            print(
                "Version info from 'M2M2_ADDR_MED_PED':\nmajor: {} \nminor: {} \npatch: {} \nversion: {} \nvendor: {}".format(
                    version_Info.major, version_Info.minor, version_Info.patch, version_Info.verstr,
                    version_Info.vendstr))
        else:
            print("Error in parsing arguments!")

    def help_get_version(self):
        print("get version information for all application")
        if device_id == OPTICAL_BOARD_4100:
            print(" Available Application adpd/adxl/ppg/syncppg")
        elif device_id == SENSORBOARD3:
            print("  Available Application  pm,ps,ppg,ecg,eda,temperature,pedometer,ad7689,adpd,adxl,hrv,bcm")
        elif device_id < SENSORBOARD3:
            print("  Available Application  pm,ps,ppg,ecg,eda,temperature,pedometer,ad7689,adpd,adxl,hrv")
        elif IsPPGSupported:
            print("  Available Application adpd/adxl/ppg/syncppg")
        elif m4_Platform:
            print(" Available Application adpd/adxl")
        else:
            print(" Available Application adpd")
        print("  Example usage: get_version pm")

    def watch_get_lcfg(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help get_lcfg) to know the usage.")
            return

        if "ppg" in args:
            data = self.active_watch.ppg_app.get_lcfg();
            for d in data:
                print(hex(d))

        elif "ecg" in args:
            data = self.active_watch.ecg_app.get_lcfg();
            for d in data:
                print(hex(d))
        else:
            print("Error in parsing arguments!")

    def helpWatch_get_lcfg(self):
        print("get PPG lcfg")
        print("  Example usage:")
        print("  #> get_lcfg [ppg | ecg]")
        print("  #> get_lcfg ppg")
        print("  #> get_lcfg ecg")

    def do_set_date_time(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_date_time) to know the usage.")
            return

        sysdatetime = gen3_sdk.sys_date_time_t()
        now = datetime.datetime.now()
        is_dst = time.daylight and time.localtime().tm_isdst > 0
        utc_offset = - (time.altzone if is_dst else time.timezone)
        sysdatetime.year = now.year
        sysdatetime.month = now.month
        sysdatetime.day = now.day
        sysdatetime.hour = now.hour
        sysdatetime.minute = now.minute
        sysdatetime.second = now.second
        sysdatetime.TZ_sec = utc_offset
        if "pm" in args:
            status = self.active_watch.pm_app.set_date_time(sysdatetime)
            print("Status: ", file_status[status])
        elif "ps" in args and device_id <= SENSORBOARD3:
            status = self.active_watch.ps_app.set_date_time(sysdatetime)
            print("Status: ", file_status[status])
        else:
            print("Error in parsing arguments!")

    def help_set_date_time(self):
        print("set local date and time to watch")
        print("  Example usage:")
        if device_id <= SENSORBOARD3:
            print("  #> set_date_time [ pm | ps ] ")
            print("  #> set_date_time pm")
            print("  #> set_date_time ps")
        else:
            print("  #> set_date_time [ pm ] ")
            print("  #> set_date_time pm")


    def do_get_date_time(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help get_date_time) to know the usage.")
            return

        get_datetime = gen3_sdk.sys_date_time_t()
        self.active_watch.pm_app.get_date_time(get_datetime)
        print("year:{} \nmonth: {} \nday: {} \nhour:{}  \nminute:{} \nsecond:{} \nTZ_sec: {}".format(get_datetime.year,
                                                                                                     get_datetime.month,
                                                                                                     get_datetime.day,
                                                                                                     get_datetime.hour,
                                                                                                     get_datetime.minute,
                                                                                                     get_datetime.second,
                                                                                                     get_datetime.TZ_sec))

    def help_get_date_time(self):
        print("get date and time information")
        print("  Example usage:")
        print("  #> get_date_time")

    def watch_get_battery_info(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help get_battery_info) to know the usage.")
            return

        battery_Info = gen3_sdk.pm_sys_bat_info_t()
        self.active_watch.pm_app.get_battery_info(battery_Info)
        print(
            "PM Battery information :\ntimestamp:{} \nBattery Status: {} \nBattery Level: {} \nBattery Voltage:{}  \nBattery Temp:{}".format(
                battery_Info.timestamp,
                battery_Info.bat_chrg_stat,
                battery_Info.bat_lvl,
                battery_Info.bat_mv,
                battery_Info.bat_temp))

    def helpWatch_get_battery_info(self):
        print("get battery informations like battery status, level, voltage and temperature")
        print("  Battey Status is any one of the following: BAT_STATE_NOT_AVAIL = 0 BAT_STATE_NOT_CHARGING = 1 BAT_STATE_CHARGING = 2 BAT_STATE_COMPLETE = 3 BAT_STATE_DETECTION = 4 BAT_STATE_CHARGE_ERR = 5")
        print("  Example usage:")
        print("  #> get_battery_info")

    def watch_set_battery_threshold(self, arg):
        args = self._parse_args(arg, 2)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_battery_threshold) to know the usage.")
            return
        try:
            bat_low_lvl = int(args[0])
            bat_critical_lvl = int(args[1])

            if (bat_low_lvl >=1 and bat_low_lvl <=100 and bat_critical_lvl >= 1 and bat_critical_lvl <= 100):
                status = self.active_watch.pm_app.set_battery_threshold(bat_low_lvl, bat_critical_lvl)
                print("Status: ", file_status[status])
            else:
                print("bat_low_lvl and bat_critical_lvl range between 0 to 100")

        except ValueError:
            print("Error parsing arguments! Please enter a valid integer number")

    def helpWatch_set_battery_threshold(self):
        print("set battery Low level and Critical level in percentage(%)")
        print("  #> battery threshold value limit [0-100]")
        print("  Example usage:")
        print("  #> set_battery_threshold [bat_low_lvl  bat_critical_lvl]")
        print("  #> set_battery_threshold 10 15")

    def watch_set_power_state(self, arg):
        args = self._parse_args(arg, 2)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_power_state) to know the usage.")
            return
        try:
            powerState = int(args[1])
            if (0 <= powerState <= 3):
                if "pm" in args:
                    status = self.active_watch.pm_app.set_power_state(powerState)
                    print("Status: ", file_status[status])
                elif "ps" in args:
                    status = self.active_watch.ps_app.set_power_state(powerState)
                    print("Status: ", file_status[status])
                else:
                    print("Error in parsing arguments!")
            else:
                print("The power state range between 0 to 3")

        except ValueError:
            print("Error parsing arguments! Please enter a valid integer number")

    def helpWatch_set_power_state(self):
        print("set power state ")
        print("  Example usage:")
        print("  Power state:")
        print("  #> 0: STATE_ACTIVE, 1: STATE_FLEXI, 2: STATE_HIBERNATE 3: STATE_SHUTDOWN")
        print("  #> set_power_state [ pm | ps ] [power_state]")
        print("  #> set_power_state pm 0")
        print("  #> set_power_state ps 0")

    def do_get_system_info(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help get_system_info) to know the usage.")
            return

        system_Info = gen3_sdk.pm_sys_info_t()
        self.active_watch.pm_app.get_system_info(system_Info)
        print(
            "version:\t\t{}\nmac_addr:\t\t{}\ndevice_id:\t\t{}\nmodel_number:\t{}\nhw_id:\t\t\t0x{}\nbom_id:\t\t\t0x{}\nbatch_id:\t\t0x{}\ndate:\t\t\t{}".format(
                system_Info.version, system_Info.mac_addr, system_Info.device_id, system_Info.model_number,
                format(system_Info.hw_id,'04x'), format(system_Info.bom_id,'04x'), format(system_Info.batch_id,'04x'), system_Info.date))

    def help_get_system_info(self):
        print("get system information")
        print("  Example usage:")
        print("  #> get_system_info")

    def do_ping(self, arg):
        args = self._parse_args(arg, 2)
        cnt = 0
        if args == None:
            print("Invalid arguments! please type help <command>(help ping) to know the usage.")
            return
        try:
            iteration = int(args[1])
            if (iteration == 0):
                iteration = 1

            if "pm" or "ps" in args:

                if "pm" in args:
                    for x in range(0, iteration):
                        status = self.active_watch.pm_app.ping()
                        if status == 0:
                            cnt = cnt + 1

                elif "ps" in args and device_id <= SENSORBOARD3:
                    for x in range(0, iteration):
                        status = self.active_watch.ps_app.ping()
                        if status == 0:
                            cnt = cnt + 1
                else:
                    print("Invalid arguments! please type help <command>(help ping) to know the usage.")
                    return

                if cnt == iteration:
                    print("Status: 'M2M2_APP_COMMON_STATUS_OK'")
                else:
                    print("Status: 'M2M2_APP_COMMON_STATUS_ERROR'")
            else:
                print("Error in parsing arguments!")

        except ValueError:
            print("Error parsing arguments!")

    def help_ping(self):
        print("ping")
        print("  Example usage:")
        if device_id <= SENSORBOARD3:
            print("  #> ping [pm | ps] [iteration]")
            print("  #> ping pm 2")
            print("  #> ping ps 2")
        else:
            print("  #> ping [pm] [iteration]")
            print("  #> ping pm 2")

    def watch_bcm_freq_sweep(self,arg):
        args = self._parse_args(arg, 1)
        enableOption = None
        if args == None:
            print("Invalid arguments! please type help <command>(help bcm_freq_sweep) to know the usage.")
            return

        if "enable" in args:
            enableOption = True

        elif "disable" in args:
            enableOption = False

        if enableOption != None:
            status = self.active_watch.bcm_app.enable_or_disable_sweep_frequency(enableOption)
            print("Status: ", dict[status])
        else:
            print("Error in parsing arguments! Please provide a valid arguments")

    def helpWatch_bcm_freq_sweep(self):
        print(" Enable or Disable bcm frequenct sweep")
        print("  Example usage:")
        print("  #> bcm_freq_sweep [ enable | disable ]")
        print("  #> bcm_freq_sweep enable")
        print("  #> bcm_freq_sweep disable")

    def watch_bcm_set_dft_num(self, arg):
        args = self._parse_args(arg, 1)
        dft_num = 12
        if args == None:
            print("please provide valid argument")
            return
        dft_num = int(args[0])
        status = self.active_watch.bcm_app.set_dft_num(dft_num)
        print("Status: ", dict[status])

    def helpWatch_bcm_set_dft_num(self):
        print(" Set the BCM app DFT number")
        print("  '10' for 4096")
        print("  '11' for 8192")
        print("  '12' for 16384")
        print("  Example usage:")
        print("  #> bcm_set_dft_num [value]")
        print("  #> bcm_set_dft_num 12")

    def watch_set_ad5940_datarate(self, arg):
        args = self._parse_args(arg, 1)
        datarate = 0
        if args == None:
            print("please provide valid argument")
            return
        datarate = int(args[0])
        status = self.active_watch.ad5940_app.set_datarate(datarate)
        print("Status: ", dict[status])

    def helpWatch_set_ad5940_datarate(self):
        print("  Set the output data rate of Ad5940 device. The argument is the state:")
        print(  "'0' for 1Hz")
        print("  '1' for 4Hz")
        print("  '2'for 10Hz")
        print("  '3' for 20Hz")
        print("  Example usage:")
        print("  #> set_ad5940_datarate [value]")
        print("  #> set_ad5940_datarate 0")

    def watch_set_ad5940_wg_frequency(self, arg):
        args = self._parse_args(arg, 1)
        freq = 0
        if args == None:
            print("please provide valid argument")
            return
        freq = int(args[0])
        status = self.active_watch.ad5940_app.set_wg_freq(freq)
        print("Status: ", dict[status])

    def helpWatch_set_ad5940_wg_frequency(self):
        print("  Set the Waveform Generator Freq of Ad5940 device.. The argument is the freq value:")
        print("  '10000' for 10KHz")
        print("  '25000' for 25KHz")
        print("  '50000' for 50KHz")
        print("  '100000' for 1000KHz")
        print("  Example usage:")
        print("  #> set_ad5940_wg_frequency [freq]")
        print("  #> set_ad5940_wg_frequency 50000")

    def watch_set_battery_charging(self, arg):
        args = self._parse_args(arg, 1)
        batteryState = None
        if args == None:
            print("Invalid arguments! please type help <command>(help set_battery_charging) to know the usage.")
            return

        if "enable" in args:
            batteryState = True

        elif "disable" in args:
            batteryState = False

        if batteryState != None:
            status = self.active_watch.pm_app.enable_or_disable_battery_charger(batteryState)
            print("Status: ", file_status[status])
        else:
            print("Error in parsing arguments! Please provide a valid arguments")

    def helpWatch_set_battery_charging(self):
        print(" Enable or Disable battery charge")
        print("  Example usage:")
        print("  #> set_battery_charging [ enable | disable ]")
        print("  #> set_battery_charging enable")
        print("  #> set_battery_charging disable")

    def watch_set_usb_power(self, arg):
        args = self._parse_args(arg, 2)
        enable = None
        action = None
        if args == None:
            print("Invalid arguments! please type help <command>(help set_usb_power) to know the usage.")
            return

        if "enable" in args[0]:
            enable = True
        elif "disable" in args[0]:
            enable = False

        str_length = len(args[1]);

        if str_length == 3:

            if "set" in args[1]:
                action = True
            elif "get" in args[1]:
                action = False

        if enable != None and action != None:
            power_status = self.active_watch.pm_app.set_usb_power(enable, action)
            if power_status == 0:
                print("Status: 'M2M2_APP_COMMON_STATUS_OK'")
            else:
                print("Status: 'M2M2_APP_COMMON_STATUS_ERROR'")
        else:
            print("Error in parsing arguments! Please provide a vaild arguments")

    def helpWatch_set_usb_power(self):
        print("set USB power")
        print("  Example usage:")
        print("  #> set_usb_power [enable | disable] [set | get]")
        print("  #> set_usb_power enable set")

    def watch_thermistor_state_change(self, arg):
        args = self._parse_args(arg, 1)
        state = None
        if args == None:
            print("Invalid arguments! please type help <command>(help thermistor_state_change) to know the usage.")
            return

        if "enable" in args:
            state = True
        elif "disable" in args:
            state = False

        if state != None:
            status = self.active_watch.pm_app.enable_or_disable_thermistor_state_change(state);
            print("Status: ", file_status[status])
        else:
            print("Error in parsing arguments! Please provide a vaild arguments")

    def helpWatch_thermistor_state_change(self):
        print("Enable or Disable Thermistor state change")
        print("  Example usage:")
        print("  #> thermistor_state_change [enable | disable] ")
        print("  #> thermistor_state_change enable")
        print("  #> thermistor_state_change disable")

    def do_get_dcfg(self, arg):
        args = self._parse_args(arg, 1)

        if args == None:
            print("Invalid arguments! please type help <command>(help get_dcfg) to know the usage.")
            return

        if "adxl" in args:
            data = self.active_watch.adxl_app.get_dcfg();
            for d in data:
                print("address: {} value: {}".format(hex(d[0]), hex(d[1])))

        elif "adpd4000" in args:
            data = self.active_watch.adpd4000_app.get_dcfg();
            for d in data:
                print("address: {} value: {}".format(hex(d[0]), hex(d[1])))

        elif "adpd" in args:
            data = self.active_watch.adpd_app.get_dcfg();
            for d in data:
                print("address: {} value: {}".format(hex(d[0]), hex(d[1])))
        else:
            print("Error in parsing arguments!")

    def help_get_dcfg(self):
        print("get dcfg")
        print("  Example usage:")
        if IsADPD4000Supported:
            print("  #> get_dcfg [adpd/adxl/adpd4000]")
            print("  #> get_dcfg adpd")
        elif m4_Platform or device_id <= SENSORBOARD3:
            print("  #> get_dcfg [adpd/adxl]")
            print("  #> get_dcfg adpd")
        else:
            print("  #> get_dcfg [adpd]")
            print("  #> get_dcfg adpd")



    def watch_fs_format(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_format) to know the usage.")
            return
        print("FS-Format command will be sent, response will be received only when format is complete. Typical wait time varies from 4 to 20 seconds")
        format_status = self.active_watch.fs_app.fs_format()
        print("Status: ", file_status[format_status])

    def helpWatch_fs_format(self):
        print("get clear the data in the file system")
        print("  Example usage:")
        print("  #> fs_format")

    def watch_fs_keyvaluepair(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help fs_keyvaluepair) to know the usage.")
            return
        try:
            if len(args[0]) <= 16:
                subjectID = str(args[0])
                if not re.match("^[a-zA-Z0-9]*$", subjectID):
                    print("Error! Only alphanumeric character allowed!");
                else:
                    status = self.active_watch.fs_app.fs_keyvaluepair(subjectID)
                    print("Status: ", file_status[status])
            else:
                print("Error in parsing arguments! Maxmium 16 alphanumeric characters allowed.")

        except ValueError:
            print("Error parsing arguments!")

    def helpWatch_fs_keyvaluepair(self):
        print("add subject ID for the Log file")
        print("  Example usage:")
        print("  #> maximum subjectId length 16 alphanumeric characters")
        print("  #> fs_keyvaluepair subjectID")
        print("  #> fs_keyvaluepair 123 ")

    def watch_fs_refhr(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help fs_refhr) to know the usage.")
            return

        sysdatetime = gen3_sdk.sys_date_time_t()
        now = datetime.datetime.now()
        is_dst = time.daylight and time.localtime().tm_isdst > 0
        utc_offset = - (time.altzone if is_dst else time.timezone)
        sysdatetime.year = now.year
        sysdatetime.month = now.month
        sysdatetime.day = now.day
        sysdatetime.hour = now.hour
        sysdatetime.minute = now.minute
        sysdatetime.second = now.second
        sysdatetime.TZ_sec = utc_offset
        hr_value = 0;
        try:
            hr_value = int(args[0])
        except Exception as e:
            print(e)
            print("Type a valid HR value")

        self.active_watch.fs_app.fs_refhr(hr_value, sysdatetime)

    def helpWatch_fs_refhr(self):
        print("  reference hr logging in file system")
        print("  Note: A hr logging(ecg or syncppg) should have been started before executing fs_refhr")
        print("  Example usage:")
        print("  #> fs_refhr [heart rate in bpm]")
        print("  #> fs_refhr 78")

    def watch_fs_mount(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_mount) to know the usage.")
            return

        status = self.active_watch.fs_app.fs_mount()
        print("Status: ", low_touch_start_status[status])

    def helpWatch_fs_mount(self):
        print("Mount File System Command")
        print("  Example usage:")
        print("  #> fs_mount")

    def watch_fs_stream_start(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_stream_start) to know the usage.")
            return

        status = self.active_watch.fs_app.fs_stream.start()
        print("Status: ", low_touch_start_status[status])


    def helpWatch_fs_stream_start(self):
        print("This function requests that the stream to be started (i.e., opens a file to write logs)")
        print("  Example usage:")
        print("  #> fs_stream_start")

    def watch_fs_stream_stop(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_stream_stop) to know the usage.")
            return

        status = self.active_watch.fs_app.fs_stream.stop()
        print("Status: ", low_touch_start_status[status])


    def helpWatch_fs_stream_stop(self):
        print("This function requests that the stream to be stopped (i.e., closes a file)")
        print("  Example usage:")
        print("  #> fs_stream_stop")

    def watch_fs_status(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_status) to know the usage.")
            return

        fs_status = self.active_watch.fs_app.fs_status()
        if fs_status == gen3_sdk.m2m2_application.APP_COMMON_STATUS_STREAM_NOT_STARTED:
            print("File is not opened")
        elif fs_status == gen3_sdk.m2m2_application.APP_COMMON_STATUS_STREAM_IN_PROGRESS:
            print("File is opened")
        else:
            print("File System error")

    def helpWatch_fs_status(self):
        print("get status information whether filesystem open or close")
        print("  Example usage:")
        print("  #> fs_status")

    def watch_fs_volume_info(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_volume_info) to know the usage.")
            return

        total_Memory, used_Memory, free_Memory = self.active_watch.fs_app.fs_volume_info()
        print("total_Memory:{} Bytes\nused_Memory: {} Bytes\nfree_Memory: {}%".format(total_Memory, used_Memory, free_Memory))

    def helpWatch_fs_volume_info(self):
        print("get file system volume information")
        print("  Example usage:")
        print("  #> fs_volume_info")

    def watch_fs_get_log_list(self, arg):
        global Waitfor_cb_complete
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_volume_info) to know the usage.")
            return
        Waitfor_cb_complete = True
        self.active_watch.fs_app.fs_ls(fs_ls_cb().__disown__())
        while True:
            if Waitfor_cb_complete == False:
                break;

    def helpWatch_fs_get_log_list(self):
        print("display the list logs avilable in filesystem ")
        print("  Example usage:")
        print("  #> fs_get_log_list")


    # def do_fs_get(self, arg):
    #     global Fileobject, Filename, Totalbytes
    #     Fileobject = None
    #     Filename = ""
    #     Totalbytes = 0
    #     args = self._parse_args(arg, None)
    #     if len(args) != 1:
    #         print("Invalid arguments! please type help <command>(help fs_get) to know the usage.")
    #         return
    #     try:
    #         filename = str(args[0])
    #         if ".LOG" in filename:
    #             Filename = filename
    #             if Fileobject == None:
    #                 try:
    #                     Fileobject = open(filename, "wb")
    #                 except ValueError:
    #                     print("Error in opening file")
    #             else:
    #                 print("Error in opening file")
    #             print("Downloading FILE :'{}' from watch...".format(Filename))
    #             self.active_watch.fs_app.fs_get(filename, fs_stream_cb().__disown__())
    #             while True:
    #                 if Fileobject == None:
    #                     break;
    #             print("Total bytes received: {}".format(Totalbytes))
    #             print("FILE : {} downloaded successfully in current directory path".format(Filename))
    #         else:
    #             print("")
    #             print("Error parsing arguments.Invalid Fileformat! \nFilename with .LOG extension is expected.")
    #     except ValueError:
    #         print("Error parsing arguments!")
    #
    # def help_fs_get(self):
    #     print("getting stream from selected log files")
    #     print("  Example usage:")
    #     print("  #> fs_get [filename]")
    #     print("  #> fs_get 10304EA7.LOG")

    def watch_fs_get_stream_chunk(self, arg):
        global Fileobject, Filename, Totalbytes
        Fileobject = None
        Filename = ""
        Totalbytes = 0
        args = self._parse_args(arg, 3)
        if args == None:
            return
        if len(args) != 3:
            print("Invalid arguments! please type help <command>(help fs_get) to know the usage.")
            return
        try:
            Roll_over = int(args[0])
            chunk_number = int(args[1])
            filename, ext = args[2].split('.')
            if ".LOG" in args[2]:
                Filename = filename +'_chunk.LOG'
                if Fileobject == None:
                    try:
                        Fileobject = open(Filename, "wb")
                    except ValueError:
                        print("Error in opening file")
                else:
                    print("Error in opening file")
                print("Downloading chunk :'{}' from watch...".format(Filename))
                self.active_watch.fs_app.fs_get_stream_chunk(Roll_over, chunk_number, filename, fs_stream_cb().__disown__())
                #while True:
                if Fileobject == None:
                    return
                    #break;
                print("Total bytes received: {}".format(Totalbytes))
                print("FILE : {} downloaded successfully in current directory path".format(Filename))
            else:
                print("")
                print("Error parsing arguments.Invalid Fileformat! \nFilename with .LOG extension is expected.")
        except ValueError:
            print("Error parsing arguments!")

    def helpWatch_fs_get_stream_chunk(self):
        print("Get the particular chunk of data from file by specifying the chunk number.")
        print("  Example usage:")
        print("  #> fs_get_stream_chunk [Roll_over] [chunk_number] [filename]")
        print("  #> fs_get_stream_chunk 0 820 10304EA7.LOG")

    def watch_fs_get_file_count(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_get_file_count) to know the usage.")
            return

        maxFilecount = self.active_watch.fs_app.fs_get_file_count()
        print("Number of files: ", maxFilecount)

    def helpWatch_fs_get_file_count(self):
        print("get the number of files in file system")
        print("  Example usage:")
        print("  #> fs_get_file_count")

    def watch_fs_find_config_file(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_find_config_file) to know the usage.")
            return

        file_Status = self.active_watch.fs_app.fs_find_config_file()
        if (file_status[file_Status] == "M2M2_APP_COMMON_STATUS_OK"):
            print("Status: ", "M2M2_FILE_SYS_CONFIG_FILE_FOUND")
        else:
            print("Status: ", "M2M2_FILE_SYS_CONFIG_FILE_NOT_FOUND")

    def helpWatch_fs_find_config_file(self):
        print("To check the Config file exists in the filesystem")
        print("  Example usage:")
        print("  #> fs_find_config_file")

    def watch_delete_config_file(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help delete_config_file) to know the usage.")
            return

        file_Status = self.active_watch.fs_app.delete_config_file()
        print("Status: ", dict[file_Status])

    def helpWatch_delete_config_file(self):
        print("delete the Config file in the filesystem")
        print("  Example usage:")
        print("  #> delete_config_file")

    def watch_ps_activate_touch_sensor(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help ps_activate_touch_sensor) to know the usage.")
            return

        status = self.active_watch.ps_app.activate_touch_sensor();
        print("Status: ", dict[status])

    def helpWatch_ps_activate_touch_sensor(self):
        print("ps activate touch sensor")
        print("  Example usage:")
        print("  #> ps_activate_touch_sensor")

    def watch_ps_deactivate_touch_sensor(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help ps_deactivate_touch_sensor) to know the usage.")
            return

        status = self.active_watch.ps_app.deactivate_touch_sensor();
        print("Status: ", dict[status])

    def helpWatch_ps_deactivate_touch_sensor(self):
        print("ps deactivate touch sensor")
        print("  Example usage:")
        print("  #> ps_deactivate_touch_sensor")

    def watch_get_low_touch_status(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help get_low_touch_status) to know the usage.")
            return

        file_Status = self.active_watch.pm_app.get_low_touch_status()
        print("Status: ", lowtouch[file_Status])

    def helpWatch_get_low_touch_status(self):
        print("To check low touch logging status whether logging is inprogress or not.")
        print("  Example usage:")
        print("  #> get_low_touch_status")

    def watch_enable_config_log(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help enable_config_log) to know the usage.")
            return

        if "true" in args[0]:
            Status = self.active_watch.pm_app.enable_config_log(True)
        elif "false" in args[0]:
            Status = self.active_watch.pm_app.enable_config_log(False)
        elif (arg[0]!="true" and arg[0]!="false"):
            print("Invalid Syntax! please type help <command>(help enable_config_log) to know the usage.")
            return

        print("Status: ", lowtouch[Status])

    def helpWatch_enable_config_log(self):
        print("Enable or Disable the low touch logging")
        print("  Example usage:")
        print("  #> enable_config_log false")

    def watch_fs_config_log(self, arg):
        args = self._parse_args(arg, 1)
        start_req = None
        if args == None:
            print("Invalid arguments! please type help <command>(help fs_config_log) to know the usage.")
            return

        if "start" in args[0]:
            start_req = True
        elif "stop" in args[0]:
            start_req = False

        if start_req != None:
            Status = self.active_watch.fs_app.fs_config_log(start_req)
            print(Status)
            print("Status: ", lowtouch[Status])
        else:
            print("Error in parsing arguments!")

    def helpWatch_fs_config_log(self):
        print("open (create)/close config file in the filesystem")
        print("  Example usage:")
        print("  #> fs_config_log [ start | stop ]")
        print("  #> fs_config_log start")
        print("  #> fs_config_log stop")

    def watch_fs_push_config_file(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help fs_push_config_file) to know the usage.")
            return

        filePath = os.path.abspath("./0110347A.m2m2")

        fileContentBuffer = open(filePath, 'rb').read()
        # print("file content : ", fileContentBuffer)
        bufferlist = list(bytearray(fileContentBuffer))
        bufferlength = len(bufferlist)
        # print("bufferlength: ", bufferlength)
        if(isWiredConn != True):
            Status = self.active_watch.push_config_file(bufferlist)
            print("Status: ", low_touch_start_status[Status])
        else:
            print("LT push config file is supported only via BT / BLE")
        # print(Status)

    def helpWatch_fs_push_config_file(self):
        print("writing the buffer into the config file")
        print("opens the config file and writing the buffer into config file and then closes the file.")
        print("  Example usage:")
        print("fs_push_config_file")

    def do_get_stream_status(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help status) to know the usage.")
            return

        if "adpd4000" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream1.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd1" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream1.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd2" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream2.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd3" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream3.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd4" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream4.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd5" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream5.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd6" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream6.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd7" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream7.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd8" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream8.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd9" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream9.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd10" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream10.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd11" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream11.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd12" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd4000_app.adpd4000_stream12.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "ppg" in args:
            status, num_subscribers, num_start_requests = self.active_watch.ppg_app.ppg_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))

        elif "ecg" in args:
            status, num_subscribers, num_start_requests = self.active_watch.ecg_app.ecg_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "eda" in args:
            status, num_subscribers, num_start_requests = self.active_watch.eda_app.eda_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "pedometer" in args:
            status, num_subscribers, num_start_requests = self.active_watch.ped_app.pedometer_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "temperature" in args:
            status, num_subscribers, num_start_requests = self.active_watch.temperature_app.temperature_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "syncppg" in args:
            status, num_subscribers, num_start_requests = self.active_watch.syncppg_app.syncppg_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adpd" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adpd_app.adpd_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "adxl" in args:
            status, num_subscribers, num_start_requests = self.active_watch.adxl_app.adxl_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "bcm" in args:
            status, num_subscribers, num_start_requests = self.active_watch.bcm_app.bcm_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        elif "hrv" in args:
            status, num_subscribers, num_start_requests = self.active_watch.ppg_app.hrv_stream.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))

        elif "ad7689" in args:
            status, num_subscribers, num_start_requests = self.active_watch.ad7689_app.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))

        elif "ad5940" in args:
            status, num_subscribers, num_start_requests = self.active_watch.ad5940_app.get_status()
            print("status:{} \nnum_subscribers: {} \nnum_start_requests: {}".format(dict[status], num_subscribers,
                                                                                    num_start_requests))
        else:
            print("Error in parsing arguments!")

    def help_get_stream_status(self):
        print("Fetch the status of a stream.")
        print("  Example usage:")
        if device_id == OPTICAL_BOARD_4100:
            print(
                "  Available application status adxl/adpd4000/adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/syncppg/ppg")

            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adpd1")
        elif IsADPD4000Supported and IsPPGSupported:
            print(
                "  Available application status adxl/adpd4000/adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/syncppg/ppg")

            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adpd1")
        elif IsADPD4000Supported:
            print(
                "  Available application status adpd4000/adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12")

            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adpd1")
        elif device_id == SENSORBOARD3:
            print(
                "  Available application stream syncppg,ppg,ecg,eda,pedometer,temperature,syncppg,adpd,adxl,hrv,bcm,ad5940,ad7689")
            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adxl")

        elif device_id < SENSORBOARD3:
            print(
                "  Available application stream syncppg,ppg,ecg,eda,pedometer,temperature,syncppg,adpd,adxl,hrv,ad7689")
            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adxl")
        elif IsPPGSupported:
            print(
                "  Available application stream adpd/adxl/ppg")
            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adxl")

        elif m4_Platform:
            print(
                "  Available application stream adpd/adxl")
            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adxl")
        else:
            print(
                "  Available application stream adpd")
            print("  #>get_stream_status [device]")
            print("  #>get_stream_status adpd")


    def do_stream(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 2:
            print("Invalid arguments! please type help <command>(help stream) to know the usage.")
            return

        if "start" in args[1]:
            start_req = True
        elif "stop" in args[1]:
            start_req = False
        else:
            print("Invalid arguments! please type help <command>(help stream) to know the usage.")
            return

        if "adpd" in args:
            if start_req:
                # L2
                status = self.active_watch.start_adpd(adpd_cb().__disown__())

                # L1
                # self.active_watch.adpd_app.load_cfg()
                # self.active_watch.adpd_app.calibrate_clock()
                # self.active_watch.adpd_app.set_slot(self.active_watch.adpd_app.SLOTMODE_DISABLED,
                #                                     self.active_watch.adpd_app.SLOTMODE_SUM32b)
                # self.active_watch.adpd_app.adpd_stream.subscribe(adpd_cb().__disown__())
                # self.active_watch.adpd_app.adpd_stream.start()
            else:
                # L2
                status = self.active_watch.stop_adpd()

                # L1
                # self.active_watch.adpd_app.adpd_stream.stop()
                # self.active_watch.adpd_app.adpd_stream.unsubscribe()
            print("Status: ", stream_and_log_status[status])

        elif "adxl" in args:
            if start_req:
                # L2
                status = self.active_watch.start_adxl(adxl_cb().__disown__())

                # L1
                # self.active_watch.adxl_app.adxl_stream.start()
                # self.active_watch.adxl_app.adxl_stream.subscribe(adxl_cb().__disown__())

            else:
                # L2
                status = self.active_watch.stop_adxl()

                # L1
                # self.active_watch.adxl_app.adxl_stream.stop()
                # self.active_watch.adxl_app.adxl_stream.unsubscribe()
            print("Status: ", stream_and_log_status[status])

        elif "syncppg" in args:
            if start_req:
                # L2
                status = self.active_watch.start_syncppg(ppg_cb().__disown__(), syncppg_cb().__disown__())

                # L1
                # Load default device configs
                #self.active_watch.adpd_app.load_cfg()
                #self.active_watch.ppg_app.load_lcfg()
                #self.onecmd("lcfg_read_or_write ppg r 0x03")
                #self.onecmd("lcfg_read_or_write ppg w 0x03:A77")
                #self.onecmd("lcfg_read_or_write ppg r 0x03")
                #self.active_watch.adpd_app.calibrate_clock()
                #self.active_watch.adpd_app.adpd_stream.start()
                #self.active_watch.adxl_app.adxl_stream.start()
                #self.active_watch.ppg_app.ppg_stream.start()
                #self.active_watch.ppg_app.agc_stream.subscribe(agc_cb().__disown__())

            else:
                # L2
                status = self.active_watch.stop_syncppg()

                # L1
                # self.active_watch.ppg_app.ppg_stream.unsubscribe()
                # self.active_watch.ppg_app.ppg_stream.stop()
                # self.active_watch.adpd_app.adpd_stream.stop()
                # self.active_watch.adxl_app.adxl_stream.stop()

            print("Status: ", stream_and_log_status[status])

        elif "ecg" in args:
            if start_req:
                # L2
                status = self.active_watch.start_ecg(ecg_cb().__disown__())

                # L1
                # self.active_watch.ecg_app.ecg_stream.start()
                # self.active_watch.ad7689_app.start()
                # self.active_watch.ecg_app.ecg_stream.subscribe(ecg_cb().__disown__())
            else:
                # L2
                status = self.active_watch.stop_ecg()

                # L1
                # self.active_watch.ecg_app.ecg_stream.stop()
                # self.active_watch.ad7689_app.stop()
                # self.active_watch.ecg_app.ecg_stream.unsubscribe()
            print("Status: ", stream_and_log_status[status])

        elif "temperature" in args:
            if start_req:
                # L2
                status = self.active_watch.start_temperature(temperature_cb().__disown__())

                # L1
                # self.active_watch.temperature_app.temperature_stream.start()
                # self.active_watch.ad7689_app.start()
                # self.active_watch.temperature_app.temperature_stream.subscribe(temperature_cb().__disown__())
            else:
                # L2
                status = self.active_watch.stop_temperature()

                # L1
                # self.active_watch.temperature_app.temperature_stream.stop()
                # self.active_watch.ad7689_app.stop()
                # self.active_watch.temperature_app.temperature_stream.unsubscribe()
            print("Status: ", stream_and_log_status[status])

        elif "eda" in args:
            if start_req:
                # L2
                status = self.active_watch.start_eda(eda_cb().__disown__())

                # L1
                # self.active_watch.eda_app.eda_stream.start()
                # self.active_watch.ad7689_app.start()
                # self.active_watch.eda_app.eda_stream.subscribe(eda_cb().__disown__())
            else:
                # L2
                status = self.active_watch.stop_eda()

                # L1
                # self.active_watch.eda_app.eda_stream.stop()
                # self.active_watch.ad7689_app.stop()
                # self.active_watch.eda_app.eda_stream.unsubscribe()
            print("Status: ", stream_and_log_status[status])

        elif "pedometer" in args:
            if start_req:
                # L2
                status = self.active_watch.start_pedometer(pedometer_cb().__disown__())

                # L1
                # self.active_watch.ped_app.pedometer_stream.subscribe(pedometer_cb().__disown__());
                # self.active_watch.adxl_app.adxl_stream.start();
                # self.active_watch.ped_app.pedometer_stream.start();

            else:
                # L2
                status = self.active_watch.stop_pedometer()

                # L1
                # self.active_watch.ped_app.pedometer_stream.stop();
                # self.active_watch.ped_app.pedometer_stream.unsubscribe();
                # self.active_watch.adxl_app.adxl_stream.stop();
            print("Status: ", stream_and_log_status[status])
        elif "bcm" in args:
            if start_req:
                # L2
                status = self.active_watch.start_bcm(bcm_cb().__disown__())

                # L1


            else:


                # L2
                status = self.active_watch.stop_bcm()

                # L1

            print("Status: ", stream_and_log_status[status])
        elif "hrv" in args:
            if start_req:
                # L2
                status = self.active_watch.start_hrv(hrv_cb().__disown__())

                # L1

            else:
                # L2
                status = self.active_watch.stop_hrv()

                # L1

            print("Status: ", stream_and_log_status[status])
        elif "battery" in args:
            if start_req:
                # L2

                status = self.active_watch.start_battery(batteryStream_cb().__disown__())

                # L1
                # self.active_watch.pm_app.battery_stream.subscribe(batteryStream_cb().__disown__())
            else:
                # L2
                status = self.active_watch.stop_battery()

                # L1
                # self.active_watch.eda_app.eda_stream.unsubscribe()
            print("Status: ", stream_and_log_status[status])
        elif "adpd4000" in args:
            if start_req:
                # L2

                status = self.active_watch.start_adpd4000(adpd4000_cb().__disown__())

                # L1
                # self.active_watch.pm_app.battery_stream.subscribe(batteryStream_cb().__disown__())
            else:
                # L2
                status = self.active_watch.stop_adpd4000()

                # L1
                # self.active_watch.eda_app.eda_stream.unsubscribe()
            print("Status: ", stream_and_log_status[status])
        else:
            print("Error in parsing arguments!")

    def help_stream(self):
        print("Start or stop a data stream.")
        print("  Example usage:")
        if device_id == OPTICAL_BOARD_4100:
            print("  #>Available data stream adpd4000/syncppg/adxl")
            print("  #>stream adpd4000 start")
            print("  #>stream adpd4000 stop")
        elif IsADPD4000Supported and IsPPGSupported:
            print("  #>Available data stream adpd4000/syncppg/adxl")
            print("  #>stream adpd4000 start")
            print("  #>stream adpd4000 stop")
        elif IsADPD4000Supported:
            print("  #>Available data stream adpd4000")
            print("  #>stream adpd4000 start")
            print("  #>stream adpd4000 stop")
        elif device_id == SENSORBOARD3:
            print("  Available data stream syncppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv,bcm,battery")
            print("  Note: EDA stream should be started only when ECG is in 'Monitor' mode.")
            print("  Note: Wear the chest strap / Touch the electrode to get ECG data.")
            print("  #>stream [stream] [start|stop]")
            print("  #>stream syncppg start")
            print("  #>stream syncppg stop")
        elif device_id < SENSORBOARD3:
            print("  Available data stream syncppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv,battery")
            print("  Note: EDA stream should be started only when ECG is in 'Monitor' mode.")
            print("  Note: Wear the chest strap / Touch the electrode to get ECG data.")
            print("  #>stream [stream] [start|stop]")
            print("  #>stream syncppg start")
            print("  #>stream syncppg stop")
        elif IsPPGSupported:
            print("  #>Available data stream adpd/adxl/syncppg")
            print("  #>stream syncppg start")
            print("  #>stream syncppg stop")
        elif m4_Platform:
            print("  #>Available data stream adpd/adxl")
            print("  #>stream adpd start")
            print("  #>stream adpd stop")
        else:
            print("  #>Available data stream adpd")
            print("  #>stream adpd start")
            print("  #>stream adpd stop")
    def watch_logging(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 2:
            print("Invalid arguments! please type help <command>(help logging) to know the usage.")
            return

        if "start" in args:
            start_req = True
        elif "stop" in args:
            start_req = False
        else:
            print("Invalid arguments! please type help <command>(help logging) to know the usage.")
            return

        if "adpd" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_adpd()

                # L1
                # self.active_watch.adpd_app.load_cfg();
                # self.active_watch.adpd_app.calibrate_clock();
                # self.active_watch.adpd_app.set_slot(adpd_application::SLOTMODE_DISABLED,
                #                                                      adpd_application::SLOTMODE_SUM32b);
                # self.active_watch.fs_app.fs_subscribe(adpd_app);
                # self.active_watch.adpd_app.adpd_stream.start();

            else:
                # L2
                status = self.active_watch.stop_log_adpd()

                # L1
                # self.active_watch.adpd_app.adpd_stream.stop();
                # self.active_watch.fs_app.fs_unsubscribe(adpd_app);
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])

        elif "adxl" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_adxl()

                # L1
                # self.active_watch.fs_app.fs_subscribe(adxl_app);
                # self.active_watch.fs_app.fs_stream.start();
                # self.active_watch.adxl_app.adxl_stream.start();

            else:
                # L2
                status = self.active_watch.stop_log_adxl()

                # L1
                # self.active_watch.adxl_app.adxl_stream.stop();
                # self.active_watch.fs_app.fs_unsubscribe(adxl_app);
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])

        elif "syncppg" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_syncppg()

                # L1
                # Load default device configs
                # self.active_watch.adpd_app.load_cfg();
                #
                # self.active_watch.adpd_app.calibrate_clock();
                # self.active_watch.fs_app.fs_subscribe(syncppg_app);
                # self.active_watch.adpd_app.adpd_stream.start();
                # self.active_watch.adxl_app.adxl_stream.start();
                # self.active_watch.fs_app.fs_stream.start();

            else:
                # L2
                status = self.active_watch.stop_log_syncppg()

                # L1
                # self.active_watch.fs_app.fs_unsubscribe(syncppg_app);
                # self.active_watch.adpd_app.adpd_stream.stop();
                # self.active_watch.adxl_app.adxl_stream.stop();
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])

        elif "ecg" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_ecg()

                # L1
                # self.active_watch.fs_app.fs_subscribe(ecg_app);
                # self.active_watch.fs_app.fs_stream.start();
                # self.active_watch.ecg_app.ecg_stream.start();
                # self.active_watch.ad7689_app.start();

            else:
                # L2
                status = self.active_watch.stop_log_ecg()

                # L1
                # self.active_watch.ecg_app.ecg_stream.stop();
                # self.active_watch.fs_app.fs_unsubscribe(ecg_app);
                # self.active_watch.ad7689_app.stop();
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])

        elif "temperature" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_temperature()

                # L1
                # self.active_watch.temperature_app.temperature_stream.start();
                # self.active_watch.ad7689_app.start();
                # self.active_watch.fs_app.fs_subscribe(temperature_app);
                # self.active_watch.fs_app.fs_stream.start();

            else:
                # L2
                status = self.active_watch.stop_log_temperature()

                # L1
                # self.active_watch.temperature_app.temperature_stream.stop();
                # self.active_watch.ad7689_app.stop();
                # self.active_watch.fs_app.fs_unsubscribe(temperature_app);
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])

        elif "eda" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_eda()

                # L1
                # self.active_watch.fs_app.fs_subscribe(eda_app);
                # self.active_watch.fs_app.fs_stream.start();
                # self.active_watch.eda_app.eda_stream.start();
                # self.active_watch.ad7689_app.start();

            else:
                # L2
                status = self.active_watch.stop_log_eda()

                # L1
                # self.active_watch.eda_app.eda_stream.stop();
                # self.active_watch.fs_app.fs_unsubscribe(eda_app);
                # self.active_watch.ad7689_app.stop();
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])

        elif "pedometer" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_pedometer()

                # L1
                # self.active_watch.fs_app.fs_subscribe(ped_app);
                # self.active_watch.adxl_app.adxl_stream.start();
                # self.active_watch.ped_app.pedometer_stream.start();
                # self.active_watch.fs_app.fs_stream.start();

            else:
                # L2
                status = self.active_watch.stop_log_pedometer()

                # L1
                # self.active_watch.fs_app.fs_unsubscribe(ped_app);
                # self.active_watch.adxl_app.adxl_stream.stop();
                # self.active_watch.ped_app.pedometer_stream.stop();
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])

        elif "bcm" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_bcm()

            else:
                # L2
                status = self.active_watch.stop_log_bcm()


            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])
        elif "hrv" in args:
            if start_req:
                # L2
                status = self.active_watch.start_log_hrv()

            else:
                # L2
                status = self.active_watch.stop_log_hrv()


            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])
        elif "battery" in args:
            if start_req:

                # L2
                status = self.active_watch.start_log_battery()

                # L1
                # self.active_watch.fs_app.fs_subscribe();
                # self.active_watch.fs_app.fs_stream.start();

            else:
                # L2
                status = self.active_watch.stop_log_battery()

                # L1
                # self.active_watch.fs_app.fs_unsubscribe();
                # self.active_watch.fs_app.fs_stream.stop();

            #print("Status: ", dict[status])
            print("Status: ", stream_and_log_status[status])
        else:
            print("Error in parsing arguments!")

    def helpWatch_logging(self):
        print("Start or stop a data logging.")
        print("  Example usage:")
        print("  #>Streams available for logging adpd,adxl,syncppg,ecg,eda,temperature,pedometer,battery,bcm,hrv")
        print("  Note: EDA stream logging should be started only when ECG is in 'Monitor' mode.")
        print("  Note: Wear the chest strap / Touch the electrode to log ECG data.")
        print("  #>logging [stream] [start|stop]")
        print("  #>logging syncppg start")
        print("  #>logging syncppg stop")

    def watch_fs_sub_status(self, arg):
        args = self._parse_args(arg, None)
        set_stream = None

        if len(args) != 1:
            print("Invalid arguments! please type help <command>(help fs_sub_status) to know the usage.")
            return

        if "adpd" in args:
            set_stream = self.active_watch.fs_app.ADPD_Stream
        elif "adxl" in args:
            set_stream = self.active_watch.fs_app.ADXL_Stream
        elif "ppg" in args:
            set_stream = self.active_watch.fs_app.PPG_Stream
        elif "syncppg" in args:
            set_stream = self.active_watch.fs_app.SYNCPPG_Stream
        elif "ecg" in args:
            set_stream = self.active_watch.fs_app.ECG_Stream
        elif "temperature" in args:
            set_stream = self.active_watch.fs_app.TEMPERATURE_Stream
        elif "eda" in args:
            set_stream = self.active_watch.fs_app.EDA_Stream
        elif "pedometer" in args:
            set_stream = self.active_watch.fs_app.PEDOMETER_Stream
        elif "bcm" in args:
            set_stream = self.active_watch.fs_app.BCM_Stream
        elif "hrv" in args:
            set_stream = self.active_watch.fs_app.HRV_Stream
        elif "battery" in args:
            set_stream = self.active_watch.fs_app.BATTERY_Stream

        if set_stream != None:

            status = self.active_watch.fs_app.fs_stream_subscription_status(set_stream)
            if status == 0:
                print("FS_SUBS_STATUS: 'M2M2_FILE_SYS_UNSUBSCRIBED'")
            else:
                print("FS_SUBS_STATUS: 'M2M2_FILE_SYS_SUBSCRIBED'")
        else:
            print("Error in parsing arguments! Please provide a vaild arguments")

    def helpWatch_fs_sub_status(self):
        print("get application subscription status.")
        print("  Example usage:")
        print("  #>Streams available for subscription adpd,adxl,ppg,syncppg,ecg,eda,temperature,pedometer,battery,bcm,hrv")
        print("  #>fs_sub_status [stream]")
        print("  #>fs_sub_status ppg ")

    def watch_fs_debug_info(self, arg):
        args = self._parse_args(arg, None)
        set_stream = None

        if len(args) != 1:
            print("Invalid arguments! please type help <command>(help fs_debug_info) to know the usage.")
            return

        if "adpd" in args:
            set_stream = self.active_watch.fs_app.ADPD_Stream
        elif "adxl" in args:
            set_stream = self.active_watch.fs_app.ADXL_Stream
        elif "ppg" in args:
            set_stream = self.active_watch.fs_app.PPG_Stream
        elif "syncppg" in args:
            set_stream = self.active_watch.fs_app.SYNCPPG_Stream
        elif "ecg" in args:
            set_stream = self.active_watch.fs_app.ECG_Stream
        elif "temperature" in args:
            set_stream = self.active_watch.fs_app.TEMPERATURE_Stream
        elif "eda" in args:
            set_stream = self.active_watch.fs_app.EDA_Stream
        elif "pedometer" in args:
            set_stream = self.active_watch.fs_app.PEDOMETER_Stream
        elif "battery" in args:
            set_stream = self.active_watch.fs_app.BATTERY_Stream

        if set_stream != None:

            packet_missed, packets_received = self.active_watch.fs_app.fs_debug_info(set_stream)

            print("Total Packets received:{} \nTotal Packets missed: {} ".format(packets_received, packet_missed))
        else:
            print("Error in parsing arguments! Please provide a vaild arguments")

    def helpWatch_fs_debug_info(self):
        print("File system debug information, Command to get the packet loss and count information.")
        print("  Example usage:")
        print(
            "  #>Streams available for get debug information: adpd,adxl,ppg,syncppg,ecg,eda,temperature,pedometer,battery")
        print("  #>fs_debug_info [stream]")
        print("  #>fs_debug_info ppg ")

    def do_reg_read_or_write(self, arg):
        args = self._parse_args(arg, None)
        read = None
        reg_op_fn = None

        if len(args) < 3:
            print("Invalid arguments! please type help <command>(help reg_read_or_write) to know the usage.")
            return

        if ("r" or "read") in args:
            read = True
        elif ("w" or "write") in args:
            read = False
        else:
            print("Invalid arguments! please type help <command>(help reg) to know the usage.")
            return

        if "adxl" in args:
            reg_op_app = self.active_watch.adxl_app
        elif "adpd" in args:
            reg_op_app = self.active_watch.adpd_app
        elif "adpd4000" in args[0] and IsADPD4000Supported == True:
            reg_op_app = self.active_watch.adpd4000_app
        elif "ad5940" in args:
            reg_op_app = self.active_watch.ad5940_app
        else:
            print("Invalid arguments! please type help <command>(help reg) to know the usage.")
            return

        reg_ops = []
        for arg in args:
            if ":" in arg:
                reg_ops.append([int(arg.split(":")[0], 16), int(arg.split(":")[1], 16)])
            elif "0x" in arg:
                if read:
                    reg_ops.append(int(arg, 16))
                else:
                    reg_ops.append((int(arg, 16), 0))
        if read:
            ret = reg_op_app.register_read(reg_ops)
        else:
            ret = reg_op_app.register_write(reg_ops)
        for i in ret:
            print("address: {} value: {}".format(hex(i[0]), hex(i[1])))

    def help_reg_read_or_write(self):
        print("Read/write to/from a device's hardware register")
        print("  Example usage:")
        print("  #>Streams available for Read/write adpd,adxl,ad5940")

        print("  #>reg_read_or_write [device] [(r)ead] [address in hex] ...")
        print("  #>reg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
        if IsADPD4000Supported:
            print("  #>Streams available for Read/write adpd4000")
            print("  #>reg_read_or_write adpd4000 r 0x00 0x01 0x02")
            print("  #>reg_read_or_write adpd4000 w 0x00:0xAB 0x01:0x69 0x02:0x01")
        elif device_id == SENSORBOARD3:
            print("  #>Streams available for Read/write adpd,adxl,ad5940")
            print("  #>reg_read_or_write adxl r 0x00 0x01 0x02")
            print("  #>reg_read_or_write adxl w 0x00:0xAB 0x01:0x69 0x02:0x01")
            print("  #>reg_read_or_write ad5940 w 0x000021E0:0x00000002")
        elif device_id < SENSORBOARD3:
            print("  #>Streams available for Read/write adpd,adxl")
            print("  #>reg_read_or_write adxl r 0x00 0x01 0x02")
            print("  #>reg_read_or_write adxl w 0x00:0xAB 0x01:0x69 0x02:0x01")
        elif m4_Platform:
            print("  #>Streams available for Read/write adpd,adxl")
            print("  #>reg_read_or_write adxl r 0x00 0x01 0x02")
            print("  #>reg_read_or_write adxl w 0x00:0xAB 0x01:0x69 0x02:0x01")
        else:
            print("  #>Streams available for Read/write adpd")
            print("  #>reg_read_or_write adpd r 0x00 0x01 0x02")
            print("  #>reg_read_or_write adpd w 0x00:0xAB 0x01:0x69 0x02:0x01")



    def watch_lcfg_read_or_write(self, arg):
        args = self._parse_args(arg, None)
        read = None
        if ("r" or "read") in args:
            read = True
        elif ("w" or "write") in args:
            read = False

        if "ppg" in args:
            lcfg_op_app = self.active_watch.ppg_app
        elif "ecg" in args:
            lcfg_op_app = self.active_watch.ecg_app
        elif "eda" in args:
            lcfg_op_app = self.active_watch.eda_app
        else:
            print("Error! You did not specify a device to operate on!")
            return

        lcfg_ops = []
        for arg in args:
            if ":" in arg:
                lcfg_ops.append([int(arg.split(":")[0], 16), int(arg.split(":")[1], 16)])
            elif "0x" in arg:
                if read:
                    lcfg_ops.append(int(arg, 16))
                else:
                    lcfg_ops.append((int(arg, 16), 0))
        if read:
            ret = lcfg_op_app.lcfg_read(lcfg_ops)
        else:
            ret = lcfg_op_app.lcfg_write(lcfg_ops)
        print(ret)
        for i in ret:
            print("address: {} value: {}".format(hex(i[0]), hex(i[1])))

    def helpWatch_lcfg_read_or_write(self):
        print("Read or write an application's LCFG")
        print("  Sample usage:")

        if device_id <= SENSORBOARD3:
            print("  Available for ECG and PPG")
            print("  #>lcfg_read_or_write [device] [(r)ead] [address in hex] ...")
            print("  #>lcfg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
            print("  #>lcfg_read_or_write ppg w 0x00:0xAB 0x01:0xAB")
            print("  #>lcfg_read_or_write ppg r 0x00 0x01 0x02")
        elif device_id == OPTICAL_BOARD_4100:
            print("  Available for PPG")
            print("  #>lcfg_read_or_write [device] [(r)ead] [address in hex] ...")
            print("  #>lcfg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
            print("  #>lcfg_read_or_write ppg w 0x00:0xAB 0x01:0xAB")
            print("  #>lcfg_read_or_write ppg r 0x00 0x01 0x02")
        elif IsPPGSupported:
            print("  Available for PPG")
            print("  #>lcfg_read_or_write [device] [(r)ead] [address in hex] ...")
            print("  #>lcfg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
            print("  #>lcfg_read_or_write ppg w 0x00:0xAB 0x01:0xAB")
            print("  #>lcfg_read_or_write ppg r 0x00 0x01 0x02")
        else:
            print(" Command not supported for this device")

    def watch_ppg_load_lcfg(self, arg):
        args = self._parse_args(arg, None)
        PPG_LCFG_ID_ADPD107 = 107
        PPG_LCFG_ID_ADPD185 = 185
        PPG_LCFG_ID_ADPD108 = 108
        PPG_LCFG_ID_ADPD188 = 188
        if args == None:
            print("Invalid arguments!")
            return
        try:
            lcfgId = int(args[0])
        except Exception as ex:
            print(ex)
            return
        status = 1
        if (lcfgId) == PPG_LCFG_ID_ADPD107:
            status = self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD107)
        elif (lcfgId) == PPG_LCFG_ID_ADPD185:
            status = self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD185)
        elif (lcfgId) == PPG_LCFG_ID_ADPD108:
            status = self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD108)
        elif (lcfgId) == PPG_LCFG_ID_ADPD188:
            status = self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD188)
        else:
            print('Type a Valid lcfg Id')

        if status == 0:
            print("SDK APP:Status: 'M2M2_APP_COMMON_STATUS_OK'")
        else:
            print("SDK APP:Status: 'M2M2_APP_COMMON_STATUS_ERROR'")

    def helpWatch_ppg_load_lcfg(self):
        print("Load a pre-defined PPG LCFG. (lcfgIds: 107,185,108,188)")
        print("  Sample usage:")
        print("  #>ppg_load_lcfg lcfg_id")
        print("  #>ppg_load_lcfg")


    def watch_set_slot_adpd(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_slot_adpd) to know the usage.")
            return
        for arg in args:
            if ":" in arg:
                slot_ops = [arg.split(":")[0],
                            arg.split(":")[1]]
                for i, val in enumerate(slot_ops):
                    if ("4ch" in val) and ("16" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_4CH_16b
                    elif ("4ch" in val) and ("32" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_4CH_32b
                    elif ("sum" in val) and ("16" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_SUM16b
                    elif ("sum" in val) and ("32" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_SUM32b
                    else:
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_DISABLED
                slot_a = slot_ops[0]
                slot_b = slot_ops[1]
        self.active_watch.adpd_app.set_slot(slot_a, slot_b)

    def helpWatch_set_slot_adpd(self):
        print("Set the ADPD slot mode.")
        print("  Sample usage:")
        print("  #>set_slot_adpd slot_a_mode:slot_b_mode")
        print("  #>set_slot_adpd 4ch16:sum32")

    def do_active_watch(self, arg):
        """Displays the currently active watch being controlled by the CLI"""
        args = self._parse_args(arg, None)
        for key in self.watch_connections:
            if self.watch_connections[key].watch == self.active_watch:
                print("Currently active watch is: " + key)
                return
        print("No active watch found!")

    def help_active_watch(self):
        print("Displays the currently selected watch")
        print("  Sample usage:")
        print("  #>active_watch")

    def do_select_watch(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            self.help_select_watch()
            return
        self.active_watch = self.watch_connections[args[0]].watch

    def help_select_watch(self):
        print('Select a connected watch to control with the CLI')
        print('Available watches: ')
        for key in self.watch_connections:
            print("  {}".format(key))

    def watch_adpd_calibrate_clock(self, arg):
        status = self.active_watch.adpd_app.calibrate_clock()
        print("Status: ", dict[status])

    def helpWatch_adpd_calibrate_clock(self):
        print("Calibrate the ADPD clock")
        print("  Sample usage:")
        print("  #>adpd_calibrate_clock")

    def watch_eepromRead(self, arg):
        args = self._parse_args(arg, None)
        byte_stream = []
        eeprom_info = gen3_sdk.eeprom_info_t()
        Eepromdetails = self.active_watch.pm_app.read_EEPROM(eeprom_info)
        print(
            "Manufacture id : {}\nHardware id : {}\nBom id : {}\nBatch id : {}\nDate : {}\nAdditional Details : {}".format(
                hex(eeprom_info.manufacture_id), hex(eeprom_info.hw_id), hex(eeprom_info.bom_id),
                hex(eeprom_info.batch_id), hex(eeprom_info.date), hex(eeprom_info.additional_detail)))

    def helpWatch_eepromRead(self):
        print("Read the EEPROM Details")
        print("  Sample usage:")
        print("  #>eepromRead")

    def do_self_test(self, arg):
        """Run the self_test function on the currently active watch"""
        args = self._parse_args(arg, None)
        self.active_watch.self_test()

    def do_msg_verbose(self, arg):
        args = self._parse_args(arg, 1)
        global msgverbose
        if args == None:
            print("Invalid arguments! please type help <command>(help msg_verbose) to know the usage.")
            return
        try:
            if "enable" in args[0]:
                lvl = 3
                vrb.set_level(lvl)
                msgverbose = True
            elif "disable" in args[0]:
                lvl = 2
                vrb.set_level(lvl)
                msgverbose= False
            else:
                print("Invalid argument! ")

        except:
            vrb.err("Invalid argument!", 1)

    def help_msg_verbose(self):
        print("Set the seperate console window of the CLI.")
        print("msg_verbose Enable => will open a separate console window and display the prints")
        print("msg_verbose disable =>  will close the console Window")
        print("  Sample usage:")
        print("  #> msg_verbose [enable|disable]")
        print("  #> msg_verbose enable ")

###################################################################################################################################
#######                                                 ADPD4000                                                                ###
###################################################################################################################################


    def adpd4000_load_cfg_adpd4000(self, arg):
        args = self._parse_args(arg, None)

        ADPD_LCFG_ID_ADPD4000 = 40
        ADPD_LCFG_ID_ADPD4100 = 41


        if args == None:
            print("Invalid arguments!")
            return
        try:
            adpdId = int(args[0])
        except Exception as ex:
            print(ex)
            return
        if (adpdId) == ADPD_LCFG_ID_ADPD4000:
            Load_cfg_status = self.active_watch.adpd4000_app.load_cfg(ADPD_LCFG_ID_ADPD4000)
            print("Status: ", file_status[Load_cfg_status])
        elif (adpdId) == ADPD_LCFG_ID_ADPD4100:
            Load_cfg_status = self.active_watch.adpd4000_app.load_cfg(ADPD_LCFG_ID_ADPD4100)
            print("Status: ", file_status[Load_cfg_status])
        else:
            print('Type a Valid lcfg Id')

    def helpAdpd4000_load_cfg_adpd4000(self):
        print("Load condifiguration for ADPD4000. (adpdId: 40,41)")
        print("Sample Usage:")
        print(" #>load_cfg_adpd4000")

    def adpd4000_set_slot_adpd4000(self, arg):
        args = self._parse_args(arg, 4)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_slot_adpd4000) to know the usage.")
            return
        # set slot active for adpd4000
        try:
            slot_number = int(args[0])
            slot_enable = int(args[1])
            slot_format = int(args[2], 16)
            channel_number = int(args[3])

            dark = (slot_format & 0xF0) >> 4
            signal = slot_format & 0x0F

            if slot_number not in range(1, 13):
                print("Invalid slot number! please type help <command>(help set_slot_adpd4000) to know the usage.")
                return
            elif slot_enable not in range(0, 2):
                print("Invalid slot enable! please type help <command>(help set_slot_adpd4000) to know the usage.")
                return
            elif dark not in range (0,5) or signal not in range(0,5):
                print("Invalid slot format! please type help <command>(help set_slot_adpd4000) to know the usage.")
                return
            elif channel_number != 1 and channel_number != 3:
                print("Invalid Channel Number! please type help <command>(help set_slot_adpd4000) to know the usage.")
                return

        except Exception as e:
            print("Error parsing arguments! Error was: " + e)
            print(e)
            return

        set_slot_status = self.active_watch.adpd4000_app.set_slot(slot_number, slot_enable, slot_format,
                                                                channel_number)
        print("Status: ", file_status[set_slot_status])

    def helpAdpd4000_set_slot_adpd4000(self):
        print("Set the Adpd device slot.")
        print("Sample Usage:")
        print(" #>set_slot_adpd4000 SLOT_NUM ENABLE SLOT_FORMAT CHANNEL_NUM")
        print("  SLOT_NUM limit : 1 - 12")
        print("  0 = Disable, 1 = Enable")
        print("  SLOT_FORMAT in 0x0IDS - {OI - Dont Care ,D : 0 - 4 , S : 0 - 4}")
        print("  CHANNEL_NUM : 1 or 3(CH1 - 1, CH2 - 3)")

    def adpd4000_get_slot_adpd4000(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            i_start = 1
            i_stop = 13
        else:
            i_start = int(args[0])
            i_stop = int(args[0]) + 1
            slot_num = int(args[0])
            if slot_num not in range(1, 13):
                print("Invalid Slot Number! please type help <command>(help get_slot_adpd4000) to know the usage.")
                return

            # print("Invalid arguments! please type help <command>(help get_slot_active) to know the usage.")
            # return


        for i in range(i_start, i_stop):
            getSlot_Info = gen3_sdk.get_slot_info_t()
            getSlot_status = self.active_watch.adpd4000_app.get_slot(i, getSlot_Info)
            print(
                "Slot Num:{} \nSlot Enable: {} \nSlot Format: {} \nChannel Number:{}".format(
                    getSlot_Info.slot_num, getSlot_Info.slot_enable, hex(getSlot_Info.slot_format),
                    getSlot_Info.channel_num))
            print("Status: ", file_status[getSlot_status])


    def helpAdpd4000_get_slot_adpd4000(self):
        print("Get the Adpd device slot.")
        print("Sample Usage:")
        print(" #>get_slot_adpd4000 -> show the all slots ")
        print(" #>get_slot_adpd4000 SLOT_NUM")
        print("  SLOT_NUM limit : 1 - 12")

    def adpd4000_set_slot_active(self, arg):
        args = self._parse_args(arg, 2)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_slot_active) to know the usage.")
            return
        # set slot active with slot number and slot_active
        try:
            slot_number = int(args[0])
            slot_active = int(args[1])
            if slot_number not in range(1, 13) or slot_active not in range(0, 2):
                print("Invalid number! please type help <command>(help set_slot_active) to know the usage.")
                return

        except Exception as e:
            print("Error parsing arguments! Error was: " + e)
            print(e)
            return

        set_slot_active_status = self.active_watch.adpd4000_app.set_slot_active(slot_number, slot_active)
        print("Status: ", file_status[set_slot_active_status])

    def helpAdpd4000_set_slot_active(self):
        print("set the ADPD slot to active / inactive ")
        print("  Sample usage:")
        print("  #>set_slot_active SLOT_NUM Active/Inactive")
        print("  SLOT_NUM limit : 1 - 12")
        print("  0 = Inactive, 1 = Active")

    def adpd4000_get_slot_active(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            i_start = 1
            i_stop = 13
        else:
            i_start = int(args[0])
            i_stop = int(args[0]) + 1
            getSlotNumber = int(args[0])
            if getSlotNumber not in range(1, 13):
                print("Invalid Slot number! please type help <command>(help get_slot_active) to know the usage.")
                return

        for i in range(i_start, i_stop):
            getSlotStatus, slot_num, slot_active = self.active_watch.adpd4000_app.get_slot_active(i)

            print("Slot Num:='{}'\nActive:='{}'".format(slot_num, slot_active))
            print("Status: ", file_status[getSlotStatus])


    def helpAdpd4000_get_slot_active(self):
        print("Get the Adpd slots Active/Inactive state. ")
        print("  Sample usage:")
        print("  #>get_slot_active -> Show all active slots")
        print("  #>get_slot_active SLOT_NUM")
        print("  SLOT_NUM limit : 1 - 12")
        print("  0 = Inactive, 1 = Active")

    def adpd4000_adpd4000_calibrate_clock(self, arg):
        args = self._parse_args(arg, 1)
        if (args == None):
            print("Invalid arguments! please type help <command>(help adpd4000_calibrate_clock) to know the usage.")
            return
        if (stream_start_adpd4000 == True or stream_start == True):
            print("Stop the playback and try the clock_cal command")
            return
        clockcalid = int(args[0])
        if clockcalid not in range(0,7) or clockcalid == 3:
            print("Invalid arguments! please type help <command>(help adpd4000_calibrate_clock) to know the usage.")
            return
        resp = self.active_watch.adpd4000_app.calibrate_clock(clockcalid)
        if (resp == 0):
            print("SDK APP:Calibration Done Successfully")
        else:
            print("SDK APP:Calibration Failed")

    def helpAdpd4000_adpd4000_calibrate_clock(self):
        print("Calibrate the adpd4000 clock")
        print("  Sample usage:")
        print("  #>adpd4000_calibrate_clock clockcalid")
        print( " '0' for no calibration\n '1' for 32K clock calibration\n '2' for 1M clock calibration\n '4' for 32M clock calibration\n '5' for 32K and 32M clock calibration\n '6' for 1M and 32M clock calibration")

    ###################################################################################################################################
    #######                                                 ADPD                                                                   ###
    ###################################################################################################################################

    def adpd_set_slot_adpd(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help set_slot_adpd) to know the usage.")
            return
        now = datetime.datetime.now()
        #print(now)
        for arg in args:
            if ":" in arg:
                slot_ops = [arg.split(":")[0],
                            arg.split(":")[1]]
                for i, val in enumerate(slot_ops):
                    if ("4ch" in val) and ("16" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_4CH_16b
                    elif ("4ch" in val) and ("32" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_4CH_32b
                    elif ("sum" in val) and ("16" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_SUM16b
                    elif ("sum" in val) and ("32" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_SUM32b
                    elif("off" in val):
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_DISABLED
                    else:
                        slot_ops[i] = self.active_watch.adpd_app.SLOTMODE_DISABLED
                slot_a = slot_ops[0]
                slot_b = slot_ops[1]
            else:
                print("Invalid arguments! please type help <command>(help set_slot_adpd) to know the usage.")
                return
        self.active_watch.adpd_app.set_slot(slot_a, slot_b)
        print("set slots are : ")
        print(slot_a,slot_b)
        now = datetime.datetime.now()
        #print(now)

    def helpAdpd_set_slot_adpd(self):
        print("Set the ADPD slot mode.")
        print("  Sample usage:")
        print("  Available Slot mode are 4ch16,4ch32,sum16,sum32,off")
        print("  #>set_slot_adpd slot_a_mode:slot_b_mode")
        print("  #>set_slot_adpd 4ch16:sum32")

    def adpd_darkoffset(self, arg):
        args = self._parse_args(arg, 1)
        if args != None:
            print("Invalid arguments! please type help <command>(help darkoffset) to know the usage.")
            return
        if (stream_start == True or stream_start_adxl == True):
            print("Stop the playback and try the darkoffset command")
            return
        value1, value2, value3, value4 = self.active_watch.adpd_app.darkoffset()
        print(value1)
        print(value2)
        print(value3)
        print(value4)

    def helpAdpd_darkoffset(self):
        print("Gets the Four 16-bit outputs darkoffset value.")
        print("Example usage:")
        print("#>darkoffset")

    def adpd_floatconfig(self, arg):
        args = self._parse_args(arg, 1)
        if args != None:
            print("Invalid arguments! please type help <command>(help floatconfig) to know the usage.")
            return
        if (stream_start == True or stream_start_adxl == True):
            print("Stop the playback and try the floatconfig command")
            return
        value1, value2, value3 = self.active_watch.adpd_app.floatconfig()
        print(value1)
        print(value2)
        print(value3)

    def helpAdpd_floatconfig(self):
        print("Gets the Three 16-bit output values.")
        print("Example usage:")
        print("#>floatconfig")

    def adpd_adpd_calibrate_clock(self, arg):
        args = self._parse_args(arg, 1)
        if (args != None):
            print("Invalid arguments! please type help <command>(help adpd_calibrate_clock) to know the usage.")
            return
        if (stream_start == True or stream_start_adxl == True):
            print("Stop the playback and try the clock_cal command")
            return
        resp = self.active_watch.adpd_app.calibrate_clock()
        if (resp == 0):
            print("SDK APP:Calibration Done Successfully")
        else:
            print("SDK APP:Calibration Failed")

    def helpAdpd_adpd_calibrate_clock(self):
        print("Calibrate the ADPD clock")
        print("  Sample usage:")
        print("  #>adpd_calibrate_clock")



    def adpd_get_testcommandA(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help get_testcommandA) to know the usage.")
            return
        if int(args[0]) > 65535 or int(args[0]) < 0:
            print('Please provide value between 0 to 65535.')
            return
        deviceid = int(args[0]);
        print(deviceid)
        value1,value2,value3 = self.active_watch.adpd_app.get_testcommandA(deviceid)

        print(value1)
        print(value2)
        print(value3)

    def helpAdpd_get_testcommandA(self):
        print("General Test Command: Gets the values of test command A.")
        print("  Example usage:")
        print("  Input value allow between 0 to 65535")
        print("  #>get_testcommandA 1")

    def adpd_get_testcommandB(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help get_testcommandB) to know the usage.")
            return
        if int(args[0]) > 65535 or int(args[0]) < 0:
            print('Please provide value between 0 to 65535.')
            return

        deviceid = int(args[0]);
        print(deviceid)
        value1,value2,value3 = self.active_watch.adpd_app.get_testcommandB(deviceid)

        print(value1)
        print(value2)
        print(value3)

    def helpAdpd_get_testcommandB(self):
        print("General Test Command: Gets the values of test command B.")
        print("  Example usage:")
        print("  Input value allow between 0 to 65535")
        print("  #>get_testcommandB 1")

    def adpd_get_testcommandC(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help get_testcommandC) to know the usage.")
            return
        if int(args[0]) > 65535 or int(args[0]) < 0:
            print('Please provide value between 0 to 65535.')
            return

        deviceid = int(args[0]);
        print(deviceid)
        value1, value2, value3 = self.active_watch.adpd_app.get_testcommandC(deviceid)

        print(value1)
        print(value2)
        print(value3)

    def helpAdpd_get_testcommandC(self):
        print("General Test Command: Gets the values of test command C.")
        print("  Example usage:")
        print("  Input value allow between 0 to 65535")
        print("  #>get_testcommandC 1")


    def opl_boostvoltagem4(self, arg):
        args = self._parse_args(arg, 2)
        enable = None
        Boostvoltage = 2.8
        if args == None:
            print("Invalid arguments! please type help <command>(help boostvoltagem4) to know the usage.")
            return

        Boostvoltage = float(args[1]);

        if ((Boostvoltage <= 2.8) or (Boostvoltage >= 5.2)):
            print("Enter the Voltage Value Between 2.9 to 5.1")
            return
        fraction = (((0.5 * 129.7) / (Boostvoltage)) - 12.7) / 10;
        f = fraction * 64;
        voltage = round(f, 0);
        if "enable" in args[0]:
            enable = 0

        elif "disable" in args[0]:
            enable = 1
            return
        print(Boostvoltage)
        mcutype = self.active_watch.pm_app.get_mcu_version()
        if(mcutype == 2):
            status = self.active_watch.pm_app.boost_m4(enable, int(voltage))
            if status == 0:
                print("SDK APP:Status: 'M2M2_APP_COMMON_STATUS_OK'")
            else:
                print("SDK APP:Status: 'M2M2_APP_COMMON_STATUS_ERROR'")
        else:
            print("This Command supports only for M4 MotherBoard")
            return

    def helpOpl_boostvoltagem4(self):
            print("It's used to to vary the voltage level in M4 MotherBoard:")
            print("  Example usage:")
            print("  Voltage parameter value between 2.9 to 5.1")
            print("  #>boostvoltagem4 enable voltage")


    def opl_powerboostm4(self, arg):
        args = self._parse_args(arg, 1)
        enable = None
        if args == None:
            print("Invalid arguments! please type help <command>(help powerboostm4) to know the usage.")
            return
        if "enable" in args[0]:
            enable = 0
        elif "disable" in args[0]:
            print("powerboost disabled successfully")
            enable = 1
            return
        print(enable)
        mcutype= self.active_watch.pm_app.get_mcu_version()
        print(mcutype)
        if (mcutype == 2):
            status = self.active_watch.pm_app.powerboost_m4(enable)
            if status == 0:
                print("Application Status: 'M2M2_APP_COMMON_STATUS_OK'")
            else:
                print("Application Status: 'M2M2_APP_COMMON_STATUS_ERROR'")
        else:
            print("This Command supports only for M4 MotherBoard")
            return


    def helpOpl_powerboostm4(self):
        print("It's used to store the RDAC value into EEPROM AD5258")
        print("  Example usage:")
        print("  #> powerboostm4 enable")


    ###################################################################################################################################
    #######                                                 PPG                                                                   ###
    ###################################################################################################################################


    def ppg_get_lcfg(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help get_lcfg) to know the usage.")
            return

        if "ppg" in args:
            self.active_watch.ppg_app.setTimeout(5000)
            data = self.active_watch.ppg_app.get_lcfg();
            for d in data:
                print(hex(d))
        else:
            print("Error in parsing arguments!")

    def helpppg_get_lcfg(self):
        print("get PPG lcfg")
        print("  Example usage:")
        print("  #> get_lcfg ppg")


    def ppg_get_algo_vendor_version(self, arg):
        args = self._parse_args(arg, 1)

        if args == None:
            print("Invalid arguments! please type help <command>(help get_algo_vendor_version) to know the usage.")
            return

        algo_versionInfo = gen3_sdk.common_app_version_t()
        if "ppg" in args:
            self.active_watch.ppg_app.get_algo_vendor_version(algo_versionInfo)
            print("major:{} \nminor: {} \npatch: {} \nversion:{} \nvendor: {}".format(algo_versionInfo.major,
                                                                                      algo_versionInfo.minor,
                                                                                      algo_versionInfo.patch,
                                                                                      algo_versionInfo.verstr,
                                                                                      algo_versionInfo.vendstr))
        else:
            print("Error in parsing arguments!")

    def helpppg_get_algo_vendor_version(self):
        print("get algo vendor version for particular application")
        print("  Example usage:")
        print("   Available application ppg")
        print("  #> get_algo_vendor_version ppg")

    def ppg_lcfg_read_or_write(self, arg):
        args = self._parse_args(arg, None)
        read = None
        if ("r" or "read") in args:
            read = True
        elif ("w" or "write") in args:
            read = False

        if "ppg" in args:
            lcfg_op_app = self.active_watch.ppg_app
        else:
            print("Error! You did not specify a device to operate on!")
            return

        lcfg_ops = []
        for arg in args:
            if ":" in arg:
                lcfg_ops.append([int(arg.split(":")[0], 16), int(arg.split(":")[1], 16)])
            elif "0x" in arg:
                if read:
                    lcfg_ops.append(int(arg, 16))
                else:
                    lcfg_ops.append((int(arg, 16), 0))
        if read:
            ret = lcfg_op_app.lcfg_read(lcfg_ops)
        else:
            ret = lcfg_op_app.lcfg_write(lcfg_ops)
        print(ret)
        for i in ret:
            print("address: {} value: {}".format(hex(i[0]), hex(i[1])))

    def helpppg_lcfg_read_or_write(self):
        print("Read or write an application's LCFG")
        print("  Sample usage:")
        print("  Available for PPG")
        print("  #>lcfg_read_or_write [device] [(r)ead] [address in hex] ...")
        print("  #>lcfg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
        print("  #>lcfg_read_or_write ppg w 0x00:0xAB 0x01:0xAB")
        print("  #>lcfg_read_or_write ppg r 0x00 0x01 0x02")

    def ppg_ppg_load_lcfg(self, arg):
        args = self._parse_args(arg, None)
        PPG_LCFG_ID_ADPD107 = 107
        PPG_LCFG_ID_ADPD185 = 185
        PPG_LCFG_ID_ADPD108 = 108
        PPG_LCFG_ID_ADPD188 = 188
        PPG_LCFG_ID_ADPD4000 = 40

        if args == None:
            print("Invalid arguments!")
            return
        try:
            lcfgId = int(args[0])
        except Exception as ex:
            print(ex)
            return

        if (lcfgId) == PPG_LCFG_ID_ADPD107:
            self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD107)
        elif (lcfgId) == PPG_LCFG_ID_ADPD185:
            self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD185)
        elif (lcfgId) == PPG_LCFG_ID_ADPD108:
            self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD108)
        elif (lcfgId) == PPG_LCFG_ID_ADPD188:
            self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD188)
        elif (lcfgId) == PPG_LCFG_ID_ADPD4000:
            self.active_watch.ppg_app.load_lcfg(PPG_LCFG_ID_ADPD4000)
        else:
            print('Type a Valid lcfg Id')

    def helpppg_ppg_load_lcfg(self):
        print("Load a pre-defined PPG LCFG. (lcfgIds: 107,185,108,188,40)")
        print("  Sample usage:")
        print("  #>ppg_load_lcfg lcfg_id")
        print("  #>ppg_load_lcfg")

    def ppg_lcfg_read_or_write(self, arg):
        args = self._parse_args(arg, None)
        read = None
        if ("r" or "read") in args:
            read = True
        elif ("w" or "write") in args:
            read = False

        if "ppg" in args:
            lcfg_op_app = self.active_watch.ppg_app
        elif "ecg" in args:
            lcfg_op_app = self.active_watch.ecg_app
        elif "eda" in args:
            lcfg_op_app = self.active_watch.eda_app
        else:
            print("Error! You did not specify a device to operate on!")
            return

        lcfg_ops = []
        for arg in args:
            if ":" in arg:
                lcfg_ops.append([int(arg.split(":")[0], 16), int(arg.split(":")[1], 16)])
            elif "0x" in arg:
                if read:
                    lcfg_ops.append(int(arg, 16))
                else:
                    lcfg_ops.append((int(arg, 16), 0))
        if read:
            ret = lcfg_op_app.lcfg_read(lcfg_ops)
        else:
            ret = lcfg_op_app.lcfg_write(lcfg_ops)
        print(ret)
        for i in ret:
            print("address: {} value: {}".format(hex(i[0]), hex(i[1])))

    def helpppg_lcfg_read_or_write(self):
        print("Read or write an application's LCFG")
        print("  Sample usage:")

        if device_id <= SENSORBOARD3:
            print("  Available for ECG and PPG")
            print("  #>lcfg_read_or_write [device] [(r)ead] [address in hex] ...")
            print("  #>lcfg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
            print("  #>lcfg_read_or_write ppg w 0x00:0xAB 0x01:0xAB")
            print("  #>lcfg_read_or_write ppg r 0x00 0x01 0x02")
        elif device_id == OPTICAL_BOARD_4100:
            print("  Available for PPG")
            print("  #>lcfg_read_or_write [device] [(r)ead] [address in hex] ...")
            print("  #>lcfg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
            print("  #>lcfg_read_or_write ppg w 0x00:0xAB 0x01:0xAB")
            print("  #>lcfg_read_or_write ppg r 0x00 0x01 0x02")
        elif IsPPGSupported:
            print("  Available for PPG")
            print("  #>lcfg_read_or_write [device] [(r)ead] [address in hex] ...")
            print("  #>lcfg_read_or_write [device] [(w)rite] [address in hex]:[value in hex] ...")
            print("  #>lcfg_read_or_write ppg w 0x00:0xAB 0x01:0xAB")
            print("  #>lcfg_read_or_write ppg r 0x00 0x01 0x02")
        else:
            print(" Command not supported for this device")

##################################################################################################




    def do_getMcuVersion(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 0:
            print("Invalid arguments! please type help <command>(help getMcuVersion) to know the usage.")
            return

        mcu_type = self.active_watch.pm_app.get_mcu_version()

        if mcu_type == self.active_watch.pm_app.PM_SYS_MCU_M3:
            status = "PM_SYS_MCU_M3"
        elif mcu_type == self.active_watch.pm_app.PM_SYS_MCU_M4:
            status = "PM_SYS_MCU_M4"
        else:
            status = "PM_SYS_MCU_INVALID"

        print("status : {}".format(status))

    def help_getMcuVersion(self):
        print("get MCU type information")
        print("  Example usage:")
        print("  #> getMcuVersion")


    def do_subscribe(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help subscribe) to know the usage.")
            return
        now = datetime.datetime.now()
        # print(now)

        if "adpd1" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream1.subscribe(adpd4000_cb().__disown__())
        elif "adpd2" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream2.subscribe(adpd4000_cb().__disown__())
        elif "adpd3" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream3.subscribe(adpd4000_cb().__disown__())
        elif "adpd4" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream4.subscribe(adpd4000_cb().__disown__())
        elif "adpd5" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream5.subscribe(adpd4000_cb().__disown__())
        elif "adpd6" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream6.subscribe(adpd4000_cb().__disown__())
        elif "adpd7" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream7.subscribe(adpd4000_cb().__disown__())
        elif "adpd8" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream8.subscribe(adpd4000_cb().__disown__())
        elif "adpd9" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream9.subscribe(adpd4000_cb().__disown__())
        elif "adpd10" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream10.subscribe(adpd4000_cb().__disown__())
        elif "adpd11" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream11.subscribe(adpd4000_cb().__disown__())
        elif "adpd12" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_stream12.subscribe(adpd4000_cb().__disown__())
        elif "adpdInterrupt" in args:
            sub_status = self.active_watch.adpd4000_app.adpd4000_optional_stream.subscribe(adpd4000_cb().__disown__())
        elif "adpd" in args:
            sub_status = self.active_watch.adpd_app.adpd_stream.subscribe(adpd_cb().__disown__())
            now = datetime.datetime.now()
            # print(now)
        elif "adxl" in args and m4_Platform == True:
            sub_status = self.active_watch.adxl_app.adxl_stream.subscribe(adxl_cb().__disown__())
        elif "ppg" in args:
            sub_status = self.active_watch.ppg_app.ppg_stream.subscribe(ppg_cb().__disown__())
        elif "syncppg" in args:
            sub_status = self.active_watch.syncppg_app.syncppg_stream.subscribe(syncppg_cb().__disown__())
        elif "ecg" in args:
            sub_status = self.active_watch.ecg_app.ecg_stream.subscribe(ecg_cb().__disown__())
        elif "eda" in args:
            sub_status = self.active_watch.eda_app.eda_stream.subscribe(eda_cb().__disown__())
        elif "temperature" in args:
            sub_status = self.active_watch.temperature_app.temperature_stream.subscribe(temperature_cb().__disown__())
        elif "bcm" in args:
            sub_status = self.active_watch.bcm_app.bcm_stream.subscribe(bcm_cb().__disown__())
        elif "pedometer" in args:
            sub_status = self.active_watch.ped_app.pedometer_stream.subscribe(pedometer_cb().__disown__())
        elif "hrv" in args:
            sub_status = self.active_watch.ppg_app.hrv_stream.subscribe(hrv_cb().__disown__())
        else:
            print("Invalid arguments! please type help <command>(help subscribe) to know the usage.")
            return
        print("status : {}".format(dict[sub_status]))


    def help_subscribe(self):
        print("subscribe the stream")
        print("   Example usage:")
        if device_id == OPTICAL_BOARD_4100:
            print(
                "   #>subscribe adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/adpdInterrupt/syncppg/ppg/adxl")
        elif IsADPD4000Supported and IsPPGSupported:
            print(
                "   #>subscribe adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/adpdInterrupt/syncppg/ppg/adxl")
        elif IsADPD4000Supported:
            print(
                "   #>subscribe adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/adpdInterrupt")
        elif device_id == SENSORBOARD3:
            print("  subscribe  syncppg,ppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv,bcm")
        elif device_id < SENSORBOARD3:
            print(" subscribe syncppg,ppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv")
        elif IsPPGSupported:
            print("   #>subscribe adpd/adxl/ppg/syncppg")
        elif m4_Platform:
            print("   #>subscribe adpd/adxl")
        else:
            print("   #>subscribe adpd")


    def do_unsubscribe(self, arg):
        args = self._parse_args(arg, 1)
        if args == None:
            print("Invalid arguments! please type help <command>(help unsubscribe) to know the usage.")
            return
        now = datetime.datetime.now()
        # print(now)
        # args = self._parse_args(arg, None)

        if "adpd1" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream1.unsubscribe()
        elif "adpd2" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream2.unsubscribe()
        elif "adpd3" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream3.unsubscribe()
        elif "adpd4" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream4.unsubscribe()
        elif "adpd5" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream5.unsubscribe()
        elif "adpd6" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream6.unsubscribe()
        elif "adpd7" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream7.unsubscribe()
        elif "adpd8" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream8.unsubscribe()
        elif "adpd9" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream9.unsubscribe()
        elif "adpd10" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream10.unsubscribe()
        elif "adpd11" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream11.unsubscribe()
        elif "adpd12" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_stream12.unsubscribe()
        elif "adpdInterrupt" in args:
            unsub_status = self.active_watch.adpd4000_app.adpd4000_optional_stream.unsubscribe()
        elif "adpd" in args:
            unsub_status = self.active_watch.adpd_app.adpd_stream.unsubscribe()
            now = datetime.datetime.now()
            # print(now)
        elif "adxl" in args:
            unsub_status = self.active_watch.adxl_app.adxl_stream.unsubscribe()
        elif "ppg" in args:
            unsub_status = self.active_watch.ppg_app.ppg_stream.unsubscribe()
        elif "syncppg" in args:
            unsub_status = self.active_watch.syncppg_app.syncppg_stream.unsubscribe()
        elif "ecg" in args:
            unsub_status = self.active_watch.ecg_app.ecg_stream.unsubscribe()
        elif "eda" in args:
            unsub_status = self.active_watch.eda_app.eda_stream.unsubscribe()
        elif "temperature" in args:
            unsub_status = self.active_watch.temperature_app.temperature_stream.unsubscribe()
        elif "bcm" in args:
            unsub_status = self.active_watch.bcm_app.bcm_stream.unsubscribe()
        elif "pedometer" in args:
            unsub_status = self.active_watch.ped_app.pedometer_stream.unsubscribe()
        elif "hrv" in args:
            unsub_status = self.active_watch.ppg_app.hrv_stream.unsubscribe()
        else:
            print("Invalid arguments! please type help <command>(help unsubscribe) to know the usage.")
            return
        print("status : {}".format(dict[unsub_status]))

        now = datetime.datetime.now()
        # print(now)


    def help_unsubscribe(self):
        print("unsubscribe the stream")
        print("   Example usage:")
        if device_id == OPTICAL_BOARD_4100:
            print(
                "   #>unsubscribe adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/adpdInterrupt/syncppg/ppg/adxl")
        elif IsADPD4000Supported and IsPPGSupported:
            print(
                "   #>unsubscribe adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/adpdInterrupt/syncppg/ppg/adxl")
        elif IsADPD4000Supported:
            print("   #>unsubscribe adpd1/adpd2/adpd3/adpd4/adpd5/adpd6/adpd7/adpd8/adpd9/adpd10/adpd11/adpd12/adpdInterrupt")
        elif device_id == SENSORBOARD3:
            print("  unsubscribe  syncppg,ppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv,bcm")
        elif device_id < SENSORBOARD3:
            print(" unsubscribe syncppg,ppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv")
        elif IsPPGSupported:
            print("   #>unsubscribe adpd/adxl/ppg/syncppg")
        elif m4_Platform:
            print("   #>unsubscribe adpd/adxl")
        else:
            print("   #>unsubscribe adpd")


    def do_sensor(self, arg):
        args = self._parse_args(arg, None)
        if len(args) != 2:
            print("Invalid arguments! please type help <command>(help stream) to know the usage.")
            return
        now = datetime.datetime.now()
        global stream_start,stream_start_adpd4000,stream_start_adxl,stream_start_ppg, stream_start_ecg,stream_start_eda,stream_start_bcm,stream_start_ped,stream_start_hrv,stream_start_temperature
        #print(now)
        if "start" in args[0]:
            start_req = True
        elif "stop" in args[0]:
            start_req = False
        else:
            print("Invalid arguments! please type help <command>(help stream) to know the usage.")
            return
        if args[1] == "adpd4000" and IsADPD4000Supported == True:
            if start_req:
                if stream_start_adpd4000 == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.adpd4000_app.adpd4000_stream1.start()
                    # self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_adpd4000 = True
                    now = datetime.datetime.now()
                    # print(now)
                else:
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_adpd4000 == True:
                    print("stream stopped")
                    self.active_watch.adpd4000_app.adpd4000_stream1.stop()
                    stream_start_adpd4000 = False
                    now = datetime.datetime.now()
                    # print(now)
                else:
                    print("Stream already in stopped state")
                    return

        elif args[1] == "adpd":
            if start_req:
                if stream_start == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.adpd_app.adpd_stream.start()
                    stream_start = True
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start == True:
                    print("stream stopped")
                    self.active_watch.adpd_app.adpd_stream.stop()
                    stream_start = False
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in stopped state")
                    return

        elif args[1] == "adxl":
            if start_req:
                if stream_start_adxl == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.adxl_app.adxl_stream.start()
                    #self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_adxl = True
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_adxl == True:
                    print("stream stopped")
                    self.active_watch.adxl_app.adxl_stream.stop()
                    stream_start_adxl = False
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in stopped state")
                    return
        elif args[1] == "ppg":
            if start_req:
                if stream_start_ppg == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.ppg_app.ppg_stream.start()
                    #self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_ppg = True
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_ppg == True:
                    print("stream stopped")
                    self.active_watch.ppg_app.ppg_stream.stop()
                    stream_start_ppg = False
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in stopped state")
                    return
        elif args[1] == "bcm":
            if start_req:
                if stream_start_bcm == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.bcm_app.bcm_stream.start()
                    #self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_bcm = True
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_bcm == True:
                    print("stream stopped")
                    self.active_watch.bcm_app.bcm_stream.stop()
                    stream_start_bcm = False
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in stopped state")
                    return
        elif args[1] == "pedometer":
            if start_req:
                if stream_start_ped == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.ped_app.pedometer_stream.start()
                    # self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_ped = True
                    now = datetime.datetime.now()
                    # print(now)
                else:
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_ped == True:
                    print("stream stopped")
                    self.active_watch.ped_app.pedometer_stream.stop()
                    stream_start_ped = False
                    now = datetime.datetime.now()
                    # print(now)
                else:
                    print("Stream already in stopped state")
                    return
        elif args[1] == "hrv":
            if start_req:
                if stream_start_hrv == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.ppg_app.hrv_stream.start()
                    # self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_hrv = True
                    now = datetime.datetime.now()
                    # print(now)
                else:
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_hrv == True:
                    print("stream stopped")
                    self.active_watch.ppg_app.hrv_stream.stop()
                    stream_start_hrv = False
                    now = datetime.datetime.now()
                    # print(now)
                else:
                    print("Stream already in stopped state")
                    return
        elif args[1] == "ecg":
            if start_req:
                if stream_start_ecg == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.ecg_app.ecg_stream.start()
                    #self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_ecg = True
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_ecg == True:
                    print("stream stopped")
                    self.active_watch.ecg_app.ecg_stream.stop()
                    stream_start_ecg = False
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in stopped state")
                    return
        elif args[1] == "eda":
            if start_req:
                if stream_start_eda == False:
                    # L1
                    print("Stream starts")
                    print("stream can check by giving msg_verbose command")
                    self.active_watch.eda_app.eda_stream.start()
                    #self.active_watch.adxl_app.adxl_stream.start()
                    stream_start_eda = True
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in playing state")
                    return
            else:
                # L2
                if stream_start_ecg == True:
                    print("stream stopped")
                    self.active_watch.eda_app.eda_stream.stop()
                    stream_start_eda = False
                    now = datetime.datetime.now()
                    #print(now)
                else :
                    print("Stream already in stopped state")
                    return
        else:
            print("Error in parsing arguments!please type help <command>(help stream) to know the usage.")


    def help_sensor(self):
        print("Start or stop a data stream.")
        print("  Example usage:")
        print("  #>sensor [start|stop] [stream]")
        if device_id == OPTICAL_BOARD_4100:
            print("  #>Available data stream adpd4000/ppg/adxl")
            print("  #>sensor start adpd4000")
            print("  #>sensor stop adpd4000")
        elif IsADPD4000Supported and IsPPGSupported:
            print("  #>Available data stream adpd4000/ppg/adxl")
            print("  #>sensor start adpd4000")
            print("  #>sensor stop adpd4000")
        elif IsADPD4000Supported:
            print("  #>Available data stream adpd4000")
            print("  #>sensor start adpd4000")
            print("  #>sensor stop adpd4000")
        elif device_id == SENSORBOARD3:
            print("  Available data stream  ppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv,bcm")
            print("  #>sensor start adpd")
            print("  #>sensor stop adpd")
        elif device_id < SENSORBOARD3:
            print("  Available application ppg,ecg,eda,temperature,pedometer,adpd,adxl,hrv")
            print("  #>sensor start adpd")
            print("  #>sensor stop adpd")
        elif IsPPGSupported:
            print("  #>Available data stream adpd/adxl/ppg")
            print("  #>sensor start adpd")
            print("  #>sensor stop adpd")
        elif m4_Platform:
            print("  #>Available data stream adpd/adxl")
            print("  #>sensor start adpd")
            print("  #>sensor stop adpd")
        else:
            print("  #>Available data stream adpd")
            print("  #>sensor start adpd")
            print("  #>sensor stop adpd")


    def get_names(self):
        # This method used to pull in base class attributes
        # at a time dir() didn't do it yet.
        return dir(self.__class__)

    def onecmd(self, line):
        """Interpret the argument as though it had been typed in response
        to the prompt.

        This may be overridden, but should not normally need to be;
        see the precmd() and postcmd() methods for useful execution hooks.
        The return value is a flag indicating whether interpretation of
        commands by the interpreter should stop.

        """
        cmd, arg, line = self.parseline(line)
        arr = ['darkoffset', 'floatconfig','get_ctrvalue','adpd_calibrate_clock','set_slot_adpd','get_testcommandA', 'get_testcommsandB', 'get_testcommandC', 'powerboostm4','boostvoltagem4']
        cl_arr = ['load_cfg_adpd4000', 'adpd4000_calibrate_clock', 'get_slot_active', 'get_slot_adpd4000', 'load_cfg_adpd4000',
                  'set_slot_active', 'set_slot_adpd4000']
        ppg_arr = ['ppg_load_lcfg','get_algo_vendor_version','lcfg_read_or_write']
        watch_arr = ['lcfg_read_or_write','powerboostm3','set_led', 'get_led', 'set_decimation', 'get_decimation',
                     'get_algo_vendor_version', 'get_lcfg',  'get_battery_info',
                     'set_battery_threshold', 'set_power_state',  'set_battery_charging',
                     'set_usb_power', 'thermistor_state_change',  'fs_format', 'fs_keyvaluepair',
                     'fs_refhr', 'fs_mount', 'fs_stream_start', 'fs_stream_stop', 'fs_status', 'fs_volume_info',
                     'fs_get_log_list', 'fs_get_stream_chunk', 'fs_get_file_count',
                     'fs_find_config_file', 'delete_config_file', 'ps_activate_touch_sensor',
                     'ps_deactivate_touch_sensor', 'get_low_touch_status', 'enable_config_log', 'fs_config_log',
                     'fs_push_config_file',
                     'logging', 'fs_sub_status', 'fs_debug_info',
                      'ppg_load_lcfg', 'set_slot_adpd', 'adpd_calibrate_clock', 'eepromRead']
        opl_arr = ['connect_bt', 'disconnect_bt', 'boostvoltagem4','powerboostm4']

        if device_id == SENSORBOARD3:
            SB3_array = ['bcm_freq_sweep', 'bcm_set_dft_num', 'set_ad5940_datarate', 'set_ad5940_wg_frequency']
            watch_arr.extend(SB3_array)
        elif device_id == OPTICAL_BOARD_4000 or OPTICAL_BOARD_4100:
            arr.remove('darkoffset')
            arr.remove('floatconfig')



        if device_id <= 4 and cmd in watch_arr :
            def_string = 'watch_'
        elif cmd in arr and IsADPDSupported:
            def_string = 'adpd_'
        elif cmd in ppg_arr and IsPPGSupported:
            def_string = 'ppg_'
        elif cmd in cl_arr and  IsADPD4000Supported == True:
            def_string = 'adpd4000_'
        elif cmd in opl_arr and m4_Platform == True:
            def_string = 'opl_'
        else:
            def_string = 'do_'

        if not line:
            return self.emptyline()
        if cmd is None:
            return self.default(line)
        self.lastcmd = line
        if line == 'EOF':
            self.lastcmd = ''
        if cmd == '':
            return self.default(line)
        else:
            try:
                func = getattr(self, def_string + cmd)
            except AttributeError:
                return self.default(line)
            return func(arg)

    def do_help(self, arg):
        'List available commands with "help" or detailed help with "help cmd".'
        watch_arr = []
        arr = []
        if arg:
            # XXX check arg syntax
            arr = ['darkoffset','floatconfig','get_ctrvalue','adpd_calibrate_clock','set_slot_adpd','get_testcommandA', 'get_testcommandB', 'get_testcommandC', 'powerboostm4','boostvoltagem4']
            cl_arr = ['load_cfg_adpd4000','adpd4000_calibrate_clock','get_slot_active','get_slot_adpd4000','load_cfg_adpd4000','set_slot_active','set_slot_adpd4000']
            ppg_arr = ['ppg_load_lcfg',  'get_algo_vendor_version','lcfg_read_or_write']
            watch_arr = ['lcfg_read_or_write','powerboostm3','set_led', 'get_led', 'set_decimation', 'get_decimation',
                         'get_algo_vendor_version', 'get_lcfg', 'get_battery_info',
                         'set_battery_threshold', 'set_power_state',  'set_battery_charging',
                         'set_usb_power', 'thermistor_state_change', 'fs_format', 'fs_keyvaluepair',
                         'fs_refhr', 'fs_mount', 'fs_stream_start', 'fs_stream_stop', 'fs_status', 'fs_volume_info',
                         'fs_get_log_list', 'fs_get_stream_chunk', 'fs_get_file_count',
                         'fs_find_config_file', 'delete_config_file', 'ps_activate_touch_sensor',
                         'ps_deactivate_touch_sensor', 'get_low_touch_status', 'enable_config_log', 'fs_config_log',
                         'fs_push_config_file',
                         'logging', 'fs_sub_status', 'fs_debug_info',
                          'ppg_load_lcfg', 'set_slot_adpd', 'adpd_calibrate_clock', 'eepromRead']

            opl_arr = ['connect_bt', 'disconnect_bt', 'boostvoltagem4', 'powerboostm4']

            if device_id == SENSORBOARD3:
                SB3_array = ['bcm_freq_sweep',
                         'bcm_set_dft_num', 'set_ad5940_datarate', 'set_ad5940_wg_frequency']
                watch_arr.extend(SB3_array)
            elif device_id == OPTICAL_BOARD_4000 or OPTICAL_BOARD_4100:
                arr.remove('darkoffset')
                arr.remove('floatconfig')

            if device_id <= 4 and arg in watch_arr:
                help_string = 'helpWatch_'
                def_string = 'watch_'
            elif arg in arr and IsADPDSupported:
                help_string = 'helpAdpd_'
                def_string = 'adpd_'
            elif arg in ppg_arr and IsPPGSupported:
                help_string = 'helpppg_'
                def_string = 'ppg_'
            elif arg in cl_arr and  IsADPD4000Supported == True:
                help_string = 'helpAdpd4000_'
                def_string = 'adpd4000_'
            elif arg in opl_arr and m4_Platform == True:
                help_string = 'helpOpl_'
                def_string = 'opl_'
            else:
                help_string = 'help_'
                def_string = 'do_'
            try:
                func = getattr(self, help_string + arg)
            except AttributeError:
                try:
                    doc = getattr(self, def_string + arg).__doc__
                    if doc:
                        self.stdout.write("%s\n" % str(doc))
                        return
                except AttributeError:
                    pass
                self.stdout.write("%s\n" % str(self.nohelp % (arg,)))
                return
            func()
        else:
            names = self.get_names()
            cmds_doc = []
            cmds_undoc = []
            help = {}
            watch_arr = ['helpWatch_lcfg_read_or_write','helpWatch_powerboostm3','helpWatch_set_led', 'helpWatch_get_led', 'helpWatch_set_decimation',
                             'helpWatch_get_decimation',
                              'helpWatch_get_algo_vendor_version', 'helpWatch_get_lcfg',
                              'helpWatch_get_battery_info', 'helpWatch_set_battery_threshold',
                             'helpWatch_set_power_state',
                             'helpWatch_set_battery_charging', 'helpWatch_set_usb_power',
                             'helpWatch_thermistor_state_change',
                              'helpWatch_fs_format', 'helpWatch_fs_keyvaluepair',
                             'helpWatch_fs_refhr',
                             'helpWatch_fs_mount', 'helpWatch_fs_stream_start', 'helpWatch_fs_stream_stop',
                             'helpWatch_fs_status',
                             'helpWatch_fs_volume_info', 'helpWatch_fs_get_log_list',
                             'helpWatch_fs_get_stream_chunk', 'helpWatch_fs_get_file_count',
                             'helpWatch_fs_find_config_file',
                             'helpWatch_delete_config_file', 'helpWatch_ps_activate_touch_sensor',
                             'helpWatch_ps_deactivate_touch_sensor', 'helpWatch_get_low_touch_status',
                             'helpWatch_enable_config_log',
                             'helpWatch_fs_config_log', 'helpWatch_fs_push_config_file',
                             'helpWatch_logging', 'helpWatch_fs_sub_status',
                             'helpWatch_fs_debug_info',
                              'helpWatch_ppg_load_lcfg',
                             'helpWatch_set_slot_adpd', 'helpWatch_adpd_calibrate_clock', 'helpWatch_eepromRead']

            cl_arr = ['helpAdpd4000_load_cfg_adpd4000', 'helpAdpd4000_adpd4000_calibrate_clock',
                      'helpAdpd4000_get_slot_active', 'helpAdpd4000_get_slot_adpd4000',
                      'helpAdpd4000_load_cfg_adpd4000', 'helpAdpd4000_set_slot_active',
                      'helpAdpd4000_set_slot_adpd4000']
            ppg_arr = ['helpppg_ppg_load_lcfg', 'helpppg_get_algo_vendor_version','helpppg_lcfg_read_or_write']
            arr = ['helpAdpd_darkoffset', 'helpAdpd_floatconfig', 'helpAdpd_get_ctrvalue', 'helpAdpd_adpd_calibrate_clock', 'helpAdpd_set_slot_adpd','helpAdpd_get_testcommandA', 'helpAdpd_get_testcommandB', 'helpAdpd_get_testcommandC', 'helpAdpd_powerboostm4','helpAdpd_boostvoltagem4']
            opl_arr = ['helpOpl_connect_bt', 'helpOpl_disconnect_bt', 'helpOpl_boostvoltagem4', 'helpOpl_powerboostm4']



            if device_id == SENSORBOARD3:
                SB3_array = ['helpWatch_bcm_freq_sweep',
                             'helpWatch_bcm_set_dft_num', 'helpWatch_set_ad5940_datarate',
                             'helpWatch_set_ad5940_wg_frequency']
                watch_arr.extend(SB3_array)
            elif device_id == OPTICAL_BOARD_4000 or OPTICAL_BOARD_4100:
                arr.remove('helpAdpd_darkoffset')
                arr.remove('helpAdpd_floatconfig')

            for name in names:
                if device_id <= 4 and name in watch_arr:
                    def_string = 'helpWatch_'
                    k = 10
                elif name in ppg_arr and IsPPGSupported:
                    def_string = 'helpppg_'
                    k = 8
                elif IsADPDSupported and name in arr:
                    def_string = 'helpAdpd_'
                    k = 9
                elif  IsADPD4000Supported == True and name in cl_arr:
                    def_string = 'helpAdpd4000_'
                    k = 13
                elif name in opl_arr and m4_Platform == True:
                    def_string = 'helpOpl_'
                    k = 8
                else:
                    def_string = 'help_'
                    k = 5
                if name[:5] == 'help_':
                    help[name[5:]] = 1
                elif name[:k] == def_string:
                    help[name[k:]] = 1

            names.sort()
            # There can be duplicates if routines overridden
            prevname = ''
            watch_arr = ['watch_lcfg_read_or_write','watch_powerboostm3','watch_set_led', 'watch_get_led', 'watch_set_decimation', 'watch_get_decimation',
                         'watch_get_algo_vendor_version', 'watch_get_lcfg', 'watch_get_battery_info', 'watch_set_battery_threshold',
                         'watch_set_power_state',
                         'watch_set_battery_charging', 'watch_set_usb_power', 'watch_thermistor_state_change',
                          'watch_fs_format', 'watch_fs_keyvaluepair', 'watch_fs_refhr',
                         'watch_fs_mount', 'watch_fs_stream_start', 'watch_fs_stream_stop', 'watch_fs_status',
                         'watch_fs_volume_info', 'watch_fs_get_log_list',
                         'watch_fs_get_stream_chunk', 'watch_fs_get_file_count', 'watch_fs_find_config_file',
                         'watch_delete_config_file', 'watch_ps_activate_touch_sensor',
                         'watch_ps_deactivate_touch_sensor', 'watch_get_low_touch_status', 'watch_enable_config_log',
                         'watch_fs_config_log', 'watch_fs_push_config_file', 'watch_logging', 'watch_fs_sub_status', 'watch_fs_debug_info',
                           'watch_ppg_load_lcfg',
                         'watch_set_slot_adpd', 'watch_adpd_calibrate_clock', 'watch_eepromRead']
            arr = ['adpd_darkoffset', 'adpd_floatconfig', 'adpd_get_ctrvalue', 'adpd_adpd_calibrate_clock', 'adpd_set_slot_adpd','adpd_get_testcommandA', 'adpd_get_testcommandB', 'adpd_get_testcommandC', 'adpd_powerboostm4','adpd_boostvoltagem4']
            cl_arr = ['adpd4000_load_cfg_adpd4000', 'adpd4000_adpd4000_calibrate_clock', 'adpd4000_get_slot_active',
                      'adpd4000_get_slot_adpd4000',
                      'adpd4000_load_cfg_adpd4000', 'adpd4000_set_slot_active', 'adpd4000_set_slot_adpd4000']
            ppg_arr = ['ppg_ppg_load_lcfg', 'ppg_get_algo_vendor_version', 'ppg_lcfg_read_or_write']
            opl_arr = ['opl_connect_bt', 'opl_disconnect_bt', 'opl_boostvoltagem4', 'opl_powerboostm4']


            if device_id == SENSORBOARD3:
                SB3_array = [ 'watch_bcm_freq_sweep',
                         'watch_bcm_set_dft_num', 'watch_set_ad5940_datarate', 'watch_set_ad5940_wg_frequency']
                watch_arr.extend(SB3_array)
            elif device_id == OPTICAL_BOARD_4000 or OPTICAL_BOARD_4100:
                arr.remove('adpd_darkoffset')
                arr.remove('adpd_floatconfig')

            for name in names:
                if  device_id <= 4 and name in watch_arr:
                    help_str = 'watch_'
                    k = 6
                elif name in ppg_arr and IsPPGSupported:
                    help_str = 'ppg_'
                    k = 4
                elif IsADPDSupported and name in arr:
                    help_str = 'adpd_'
                    k = 5
                elif IsADPD4000Supported == True and name in cl_arr:
                    help_str = 'adpd4000_'
                    k = 9
                elif name in opl_arr and m4_Platform == True:
                    help_str = 'opl_'
                    k = 4
                else:
                    help_str = 'do_'
                    k = 3
                if name[:k] == help_str or name[:3] == 'do_':
                    if name == prevname:
                        continue
                    prevname = name
                    if name[:3] == 'do_':
                        cmd = name[3:]
                    elif name[:k] == help_str:
                        cmd = name[k:]
                    if cmd in help:
                        cmds_doc.append(cmd)
                        del help[cmd]
                    elif getattr(self, name).__doc__:
                        cmds_doc.append(cmd)
                    else:
                        cmds_undoc.append(cmd)
            cmds_doc.sort()
            self.stdout.write("%s\n" % str(self.doc_leader))
            self.print_topics(self.doc_header, cmds_doc, 15, 80)
            self.print_topics(self.misc_header, list(help.keys()), 15, 80)
            self.print_topics(self.undoc_header, cmds_undoc, 15, 80)


if __name__ == '__main__':
    sdk_shell().cmdloop()
