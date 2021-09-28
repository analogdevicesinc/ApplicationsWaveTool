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

import sys, threading, serial, time, struct
import colorama as cr

if sys.platform == "linux" or sys.platform == "linux2":
    sys.path.append('./../../bin/linux/python/')
elif sys.platform == "darwin":
    sys.path.append('./../../bin/macOS/python/')
elif sys.platform == "win32":
    if ((struct.calcsize("P") * 8) == 64):
        sys.path.append('./../../bin/windows/x64/python/')  # 32 bit python
    else:
        sys.path.append('./../../bin/windows/Win32/python/')  # 64 bit python
elif sys.platform == "win64":
    sys.path.append('./../../bin/windows/x64/python/')

import gen3_sdk

'''
This is a minimal working example of a Python application that uses the SDK.
This example uses two threads; one to handle asynchronous data coming over a
serial port from the device ("serial_rx_thread"), and a "main" thread that calls SDK functions
to interact with the device.
'''

SERIAL_PORT_ADDR = "COM6"

class serial_tx_cb(gen3_sdk.watch_phy_callback):
    # Callback used by the SDK to send data over a serial port.
  def __init__(self, serial_object):
      gen3_sdk.watch_phy_callback.__init__(self)
      self.serial = serial_object

  def call(self, data):
      # This function is called by the SDK whenever it wants to send data. The
      # data is in the form of a tuple of bytes.
      # In this example, this function runs in the context of the "main" thread.
      self.serial.write(data)

class adxl_stream_cb(gen3_sdk.adxl_stream_callback):
    # Callback that is called by the SDK whenever data comes over the ADXL stream.
    def __init__(self):
        gen3_sdk.adxl_stream_callback.__init__(self)
        self.seq_num = 0

    def call(self, data, sequence_num):
        # This function is called by the SDK whenever data comes over the ADXL stream.
        # In this example, this function runs in the context of the `serial_rx_thread` thread.
        # print("ADXL STREAM CALLBACK::")
        if self.seq_num + 1 != sequence_num:
            print("ADXL STREAM SEQUENCE NUM MISMATCH! EXPECTED {}, GOT {}".format(self.seq_num + 1, sequence_num))
        self.seq_num = sequence_num
        for d in data:
            print("t:{} x:{} y:{} z:{}".format(d.timestamp, d.x, d.y, d.z))

class serial_rx_cls():
    def __init__(self, serial_obj, watch_obj):
        self.run = True
        self.s = serial_obj
        self.rx_thread = threading.Thread(target=self.serial_rx_function,
                                            args=(s, watch_obj))
        # Run as a daemon so that the thread will be killed if the parent process is killed
        self.rx_thread.setDaemon(True)
        self.rx_thread.start()

    def serial_rx_function(self, serial_obj, watch_obj):
        # This function runs in a second thread, and watches the serial port for data
        # from the device. When data comes in, this thread passes it into the SDK.
        while(self.run):
            # Read the packet header
            pkt = self.s.read(8)
            if len(pkt) != 0:
                # Extract the length from the header
                length = (pkt[4] << 8) + (pkt[5])
                # Read the rest of the packet
                pkt += self.s.read(length - 8)
                # Process the packet
                watch_obj.dispatch(pkt)

    def quit(self):
        self.run = False
        self.rx_thread.join()

if __name__ == "__main__":
    # Init console colours
    cr.init(autoreset=True)
    print(cr.Fore.CYAN + "Opening serial port...")
    with serial.Serial(SERIAL_PORT_ADDR, 921600, timeout=1) as s:
        # Instantiate an SDK instance, making sure to disown the callback so that
        # it doesn't get garbage collected by the Python runtime
        print(cr.Fore.CYAN + "Instantiating watch class...")
        w = gen3_sdk.watch(serial_tx_cb(s).__disown__())
        print(cr.Fore.CYAN + "Starting serial RX thread...")
        serial_rx_obj = serial_rx_cls(s, w)
        w.initialize_sdk(gen3_sdk.watch.python)
        print(cr.Fore.CYAN + "Serial RX thread up!")

        print(cr.Fore.CYAN + "Starting the ADXL and running for 10s...")

        # "level 2" command to start the ADXL using sane default
        w.start_adxl(adxl_stream_cb().__disown__())

        # Start the ADXL directly using "level 1" commands
        # w.adxl_app.adxl_stream.start()
        # w.adxl_app.adxl_stream.subscribe(adxl_stream_cb().__disown__())
        # w.adxl_app.adxl_stream.start()

        # Stream for 10s
        time.sleep(10)

        # "level 2" command to stop the ADXL
        w.stop_adxl()

        # Stop the ADXL directly using "level 1" commands
        # w.adxl_app.adxl_stream.stop()
        # w.adxl_app.adxl_stream.unsubscribe()
        print(cr.Fore.CYAN + "ADXL has been stopped!")
        serial_rx_obj.quit()
