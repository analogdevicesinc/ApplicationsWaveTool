# Application WaveTool Data Analysis Path
This contains the extension modules to analyze data out of Wavetool Evaluation Software
1.	UDP Modules to transfer data from Wavetool to other applications
2.	SDK to interface with the M4 Evaluation Platform firmware 
3.	ADI Study Watch SDK Source and iOS binaries
4.	Reference Flutter App Source code

The license for this is in License file and the third party licenses are added in NOTICE file.

## Wavetool_UDP

### Python Modules

Requirement: Applications WaveTool 3.5.10.
Python: 3.7.1+

#### Running MATLAB Example. 
1.	Open Applications Wavetool
2.	Connect to COM port
3.	Load a specific DCFG for ADPD4xxx
4.	Play the data in Applications Wavetool (ADPD view)
5.	Enable UDP
6.	Run the Matlab awt_udp_parser.m file

##### Running Labview Example
1.	Open Applications Wavetool
2.	Connect to COM port
3.	Load a specific DCFG for ADPD4xxx
4.	Play the data in Applications Wavetool (ADPD view)
5.	Enable UDP
6.  Run the AwtTransferModule.vi
7.  Stop UDP to stop the data transfer and before closing the VI

##### Running Python example:
1.	Open Applications Wavetool
2.	Mention the COM port in the python example
3.	Mention specific DCFG for ADPD4xxx in the example

Open *example.py*

```
CONTROL_UDP_RX_PORT = 50002
CONTROL_UDP_TX_PORT = 50001
DATA_TXPORT = 50007

COM_PORT = 3   #Replace with the COM port you are using
DCFG_FILE = 'ADPD4100_PPG_Default.dcfg' #Replace with the dcfg of your choice 
                                        #but the dcfg should be in the Wavetool cfg folder
Host_IP = "localhost"
```
**COM_PORT**: *COM Port* the evaluation board is connected. This allows the python script to connect to device, as would using the *Applications WaveTool **Connect*** button.

**DCFG_FILE**: The DCFG file to load from the local folder. See you WaveTool installation *cfg* folder.

**CONTROL_UDP_RX_PORT**, **CONTROL_UDP_TX_PORT**, **DATA_TXPORT**: See *Applications WaveTool Settings->UDP* for the ports being used. The selection above is the default selection.

##### Sample output

---
\
**(base) > python3 example.py**\
\
Response Data:  b'\x99\x00\x00'
Response Status: Success
Device Connected successfully
Opening ADPD View
Enter OpenView command with any of above choice as shown below..

01 )ADPDCL Device \
02 )ADXL Device \
03 )ECG App \
04 )PPG View 

Response Status: Success
Response Data:  b'\x9e\x00\x00'
Response Status: Success
Modifying OPMODE Register
Response Data:  b'\x81\x00\x00'
Response Status: Success
Reading OPMODE Register
Response Data:  b'\x80\x00\x02' 0
Register Value is  0
Response Status: Success
Reading Chip Register
Response Data:  b'\x80\x00\x02' 450
Register Value is  450
Response Status: Success

Available DCFG File

01 )AD5940.dcfg \
02 )ADPD.dcfg \
03 )ADPD108.dcfg \
04 )ADPD1080Z-GST_EVAL_ANGLE_Nominal_01.dcfg \
05 )ADPD1080Z-GST_EVAL_GEST_Nominal_01.dcfg \
06 )ADPD1080Z-PRX_EVAL_PROX_LEDwaNominal_01.dcfg\
07 )ADPD1081Z-PPG_EVAL_Float_01.dcfg \
08 )ADPD1081Z-PPG_EVAL_Normal_01.dcfg \
09 )ADPD144RIZ-SF_EVAL_PPG_EarOrig_01.dcfg \
0A )ADPD188BIZ_EVAL_default_r120519.dcfg \
0B )ADPD188GGZ_EVAL_PPG_FloatMode.dcfg \
0C )ADPD188GGZ_TEST_PPGECG_AD8233_01.dcfg \
0D )ADPD2140Z_EVAL_ANGLE_02.dcfg \
0E )ADPD4000Z_EVAL_ECG_sAIN34_01.dcfg \
0F )ADPD4000_digital_int_IW.dcfg \
10 )ADPD4000_ECG_PPG.DCFG \
11 )ADPD4100_ECG_PPG_RevF.dcfg \
12 )ADPD4100_ECG_RevF.dcfg \
13 )ADPD4100_PPG_Default.dcfg \
14 )ADXL.dcfg

Enter load config command with any of above choice as shown below..

        Eg.,0D 01 <01/02/03...>choice

Response Data:  b'\x95\xd0t'

Loading the DCFG File ADPD4100_PPG_Default.dcfg 

Response Data:  b'\x97\x00\x00'
Response Status: Success
Selecting Plot 1 with Slot A
Response Data:  b'\x88\x00\x00'
Response Status: Success
Selecting Plot 2 with Slot B
Response Data:  b'\x88-\x00'

Playing the data from the sensor for 20 seconds 

Response Data:  b'\x82\x00\x00'
Response Status: Success

Enabling the Log and UDP Data transfer for 20 seconds 

Response Data:  b'\x85\x00\x00'
Response Status: Success
Response Data:  b'\x86\x00\x00'
Response Status: Success
UDP connection status :0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        UDP TRANSFER TOOL

UDP Port Number: 50007
Version        : 1.0

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ \

[210.0, 214.0, 207.0, 210.0, 209.0, 210.0, 214.0, 216.0, 205.0, 208.0, 212.0, ...] 
... 

Disabling the Log 

Response Data:  b'\x85\x00\x00'
Response Status: Success

Disabling the UDP Data transfer

Response Data:  b'\x86\x00\x00'
Response Status: Success

Stopping the Playback of the ADPD sensor data 

Response Data:  b'\x83\x00\x00'
Response Status: Success

Disconnecting the device 

Response Data:  b'\x91\x00\x00'
Response Status: Success

---

## M4_Eval_SDK
Software Development Kit that contains APIs interfacing with the firmware and can be used to build applications on other platforms.

### SDK - Windows
This contains the C++ and Python libraries and samples for Windows platform
Hardware - EVAL-ADPDUCZ connected to ADPD sensor board

### SDK - Linux
This contains the Python libraries and samples for Linux platform
Hardware - EVAL-ADPDUCZ connected to ADPD sensor board

## iOS_SDK
Software Development Kit (binaries) for interfacing with the Study Watch firmware and can be used for building iOS applications

## StudyWatch_SDK_App
Software DevelopmentKit C++ source code and a reference Flutter App that interfaces with the C++ SDK