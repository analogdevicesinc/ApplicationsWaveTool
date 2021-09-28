/*!
*  \file CPPSample.cpp
*
* SDK C++ Sample
*
*/
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



//#include "stdafx.h"
#include "include/serial/serial.h"
#include <iostream>
#include <cstdio>
#include <thread>
#include <bitset>
#include <string>
#include <iostream>
#include <fstream>
#include "watch.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#endif
#include <stdio.h>
#include <tchar.h>
//#define TXRAWPACKET
//#define RXRAWPACKET

#define chipId_RegAddress 0x0008
#define adpd_Address 0x0016
#define adpd4000_Address 0x00c0
#define adpd4100_Address 0x00c2
#define HW_ID_GLUE 0x8001
#define HW_ID_MUSKA 0x8003
#define ppg_hw_id_9031 0x9031 
#define ppg_hw_id_9030 0x9030
using namespace serial;
class CallbackHelper {
private:
	std::ofstream fcsv;
	std::ofstream f1csv;
	std::string fname;
	uint32_t cnt;
	uint16_t seqno;
	bool first_seqno = false;
	const char *output_directory = "../OutputLogs/";
	
public:
	bool isHeaderWritten = false;
	bool isSLOTAHeaderWritten = false;
	bool isSLOTBHeaderWritten = false;

	void setname(std::string name) { fname = output_directory + name; }
	void zerocnt() { cnt = 0; }
	uint32_t inccnt(int pp) { cnt += pp; return cnt; }
	void displaycnt(uint32_t streamtime) { 

		std::cout << "Output file was written into a path " << fname 
			<< " " << cnt 
			<< "Pkts"
			<< " " << ((float)cnt / (float)streamtime) 
			<< "Hz"
			<< std::endl; 
	}
	int32_t validate_seqno(int32_t sequence_num) {
		if (first_seqno == false) {
			first_seqno = true;
			seqno = sequence_num;
			return 1;
		}
		else if (++seqno != sequence_num) {
			std::cout << fname << " Mismatch seqno " << seqno << " != " << sequence_num << std::endl;
			return -1;
		}
		return 0;
	}
	int32_t opencsv() {
		if (fname == "")
			return -1;

		if (fcsv.is_open())
			fcsv.close();

		SYSTEMTIME st;

		GetSystemTime(&st);
		fname += std::to_string(st.wYear) +
			std::to_string(st.wMonth) +
			std::to_string(st.wDay) +
			std::to_string(st.wHour) +
			std::to_string(st.wMinute) +
			std::to_string(st.wSecond) +
			std::to_string(st.wMilliseconds) + 
			".csv";

		if (CreateDirectoryW( _T("../OutputLogs"), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{
			fcsv.open(fname);
		}
		else
		{
			std::cout << "Failed to create output directory";
		}
		return 0;
	}
	int32_t closecsv() {
		if (fcsv.is_open()) {
			fcsv.close();
			return 0;
		}
		return -1;
	}
	int32_t addcsv(std::string value) {
		if (fcsv.is_open()) {
			fcsv << value;
			return 0;
		}
		return -1;
	}

};

class AdxlStreamCallback : public adxl_stream_callback, public CallbackHelper {
public:
	AdxlStreamCallback() {
		setname("Adxl");
	}

	void call(std::vector<adxl_stream_cb_data_t> pkt, uint16_t sequence_num) {

		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			std::ostringstream oss;
			isHeaderWritten = true;
			// ADXL Header
			oss << "Timestamp" <<  ", "
				<< "X data" << ", "
				<< "Y data" << ", "
				<< "Z data"
				<< std::endl;
			addcsv(oss.str());

		}

		for (uint32_t i = 0; i < pkt.size(); i++) {
			adxl_stream_cb_data_t *pAdxlItem = &pkt[i];
			std::ostringstream oss;
			oss << pAdxlItem->timestamp << ", "
				<< pAdxlItem->x << ", "
				<< pAdxlItem->y << ", "
				<< pAdxlItem->z
				<< std::endl;
			addcsv(oss.str());
		}
	};
};



class PpgStreamCallback : public ppg_stream_callback, public CallbackHelper {
public:
	PpgStreamCallback() {
		setname("PPG");
	}

	void call(std::vector<ppg_stream_cb_data_t> pkt, uint16_t sequence_num) {
		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			std::ostringstream oss;
			isHeaderWritten = true;
			// PPG Header
			oss << "Timestamp" << ", "
				<< "AdpdLibState" << ", "
				<< "Hr" << ", "
				<< "Hr Type" << ", "
				<< "Confidence" << ","
				<< "RR Interval"
				<< std::endl;
			addcsv(oss.str());

		}

		for (uint32_t i = 0; i < pkt.size(); i++) {
			ppg_stream_cb_data_t *pPpgItem = &pkt[i];
			std::ostringstream oss;
			oss << pPpgItem->timestamp << ", " << pPpgItem->adpdlibstate << ", " << pPpgItem->hr << ", "
				<< pPpgItem->hr_type << ", " << pPpgItem->confidence << ", "
				<< pPpgItem->rr_interval << ", "
				<< std::endl;
			addcsv(oss.str());
		}
	};
};

class SyncPPGStreamCallback : public syncppg_stream_callback, public CallbackHelper {
public:
	SyncPPGStreamCallback() {
		setname("SyncPPG");
	}
	void call(std::vector<syncppg_stream_cb_data_t> pkt, uint16_t sequence_num) {
		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			std::ostringstream oss;
			isHeaderWritten = true;
			// Syncppg Header
			oss << "Timestamp" << ", "
				<< "PPG" << ", "
				<< "X" << ", "
				<< "Y" << ","
				<< "Z"
				<< std::endl;
			addcsv(oss.str());

		}

		for (int i = 0; i < pkt.size(); i++) {
			syncppg_stream_cb_data_t *syncppgStreamCbData = &pkt[i];
			std::ostringstream oss;
			oss << syncppgStreamCbData->timestamp << ", "
				<< syncppgStreamCbData->ppg << ", "
				<< syncppgStreamCbData->x << ", "
				<< syncppgStreamCbData->y << ", "
				<< syncppgStreamCbData->z
				<< std::endl;
			addcsv(oss.str());
		}
	};
};

class HRVStreamCallback : public hrv_stream_callback, public CallbackHelper
{
public:
	HRVStreamCallback()
	{
		setname("HRV_");
	}

	void call(std::vector<hrv_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			std::ostringstream oss;
			isHeaderWritten = true;
			// Syncppg Header
			oss << "Timestamp" << ", "
				<< "RR Interval" << ", "
				<< "Is GAP" 
				<< std::endl;
			addcsv(oss.str());

		}

		std::cout << std::dec;
		for (uint32_t i = 0; i < pkt.size(); i++) {
			hrv_stream_cb_data_t *pHRVItem = &pkt[i];
			std::ostringstream oss;
			oss << pHRVItem->timestamp << ","
				<< pHRVItem->rr_interval << ","
				<< pHRVItem->is_gap << std::endl;
			addcsv(oss.str());
		}


	}


};


class PedometerStreamCallback : public pedometer_stream_callback, public CallbackHelper
{
public:
	PedometerStreamCallback()
	{
		setname("PEDOMETER");
	
	}

	void call(std::vector<pedometer_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			std::ostringstream oss;
			isHeaderWritten = true;
			// Syncppg Header
			oss << "Timestamp" << ", "
				<< "No of steps" 
				<< std::endl;
			addcsv(oss.str());

		}

		std::cout << std::dec;
		for (uint32_t i = 0; i < pkt.size(); i++) {
			pedometer_stream_cb_data_t *pPedItem = &pkt[i];
			std::ostringstream oss;
			oss << pPedItem->TimeStamp << ","
				<< pPedItem->NumSteps << std::endl;
			addcsv(oss.str());
		}
	};
};

class AGCStreamCallback : public agc_stream_callback, public CallbackHelper {
public:

	AGCStreamCallback()
	{
		setname("AGC");

	}

	void call(std::vector<agc_stream_cb_data_t> pkt, uint16_t sequence_num) {

		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			std::ostringstream oss;
			isHeaderWritten = true;
			// Syncppg Header
			oss << "Timestamp" << ", "
				<< "mts0" << ", "
				<< "mts1" << ", "
				<< "mts2" << ", "
				<< "mts3" << ", "
				<< "mts4" << ", "
				<< "mts5" << ", "
				<< "setting0" << ", "
				<< "setting1" << ", "
				<< "setting2" << ", "
				<< "setting3" << ", "
				<< "setting4" << ", "
				<< "setting5" << ", "
				<< "setting6" << ", "
				<< "setting7" << ", "
				<< "setting8" << ", "
				<< "setting9"
				<< std::endl;
			addcsv(oss.str());

		}

		std::cout << std::dec;

	
		for (int i = 0; i < pkt.size(); i++) {
			agc_stream_cb_data_t *agcStreamCbData = &pkt[i];
			std::ostringstream oss;
			oss << "TS, mts0, mts1, mts2, mts3, mts4, mts5, setting0, setting1, setting2, setting3, setting4, setting5, setting6, setting7, setting8, setting9 = "
				<< agcStreamCbData->timestamp << ", "
				<< agcStreamCbData->mts0 << ", "
				<< agcStreamCbData->mts1 << ", "
				<< agcStreamCbData->mts2 << ", "
				<< agcStreamCbData->mts3 << ", "
				<< agcStreamCbData->mts4 << ", "
				<< agcStreamCbData->mts5 << ", "
				<< agcStreamCbData->setting0 << ", "
				<< agcStreamCbData->setting1 << ", "
				<< agcStreamCbData->setting2 << ", "
				<< agcStreamCbData->setting3 << ", "
				<< agcStreamCbData->setting4 << ", "
				<< agcStreamCbData->setting5 << ", "
				<< agcStreamCbData->setting6 << ", "
				<< agcStreamCbData->setting7 << ", "
				<< agcStreamCbData->setting8 << ", "
				<< agcStreamCbData->setting9
				<< std::endl;

			addcsv(oss.str());
		}
	}
};



class AdpdStreamCallback : public adpd_stream_callback, public CallbackHelper {
public:
	CallbackHelper slotA_cb;
	CallbackHelper slotB_cb;
	AdpdStreamCallback() {
		slotA_cb = CallbackHelper();
		slotB_cb = CallbackHelper();
		slotA_cb.setname("Adpd_SLOTA_");
		slotB_cb.setname("Adpd_SLOTB_");

	}

	void call(std::vector<adpd_stream_cb_data_t_sum32> pkt, uint16_t sequence_num) {

		if (pkt[0].datatype == RAW_DATA_ADPD_A_SUM_32b)
		{
			slotA_cb.validate_seqno(sequence_num);
			slotA_cb.inccnt(pkt.size());
		}
		else
		{
			slotB_cb.validate_seqno(sequence_num);
			slotB_cb.inccnt(pkt.size());
		}


		if (!isSLOTAHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_A_SUM_32b)
		{
			std::ostringstream oss;
			isSLOTAHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data"
				<< std::endl;
			
			slotA_cb.addcsv(oss.str());
		}
		else if (!isSLOTBHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_B_SUM_32b)
		{
			std::ostringstream oss;
			isSLOTBHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data"
				<< std::endl;
			slotB_cb.addcsv(oss.str());
		}

		

		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd_stream_cb_data_t_sum32 *pAdpdItem = &pkt[i];
			std::ostringstream oss;
			oss << pAdpdItem->datatype << ", "
				<< pAdpdItem->sum_timestamp <<", "
				<< (uint32_t)pAdpdItem->sum32_adpd_data <<", "
				<< std::endl;
			if (pAdpdItem->datatype == RAW_DATA_ADPD_A_SUM_32b)
			{
				slotA_cb.addcsv(oss.str());
			}
			else
			{
				slotB_cb.addcsv(oss.str());
			}
		}
	};
	void call(std::vector<adpd_stream_cb_data_t_sum16> pkt, uint16_t sequence_num) {

		if (pkt[0].datatype == RAW_DATA_ADPD_A_SUM_16b)
		{
			slotA_cb.validate_seqno(sequence_num);
			slotA_cb.inccnt(pkt.size());
		}
		else
		{
			slotB_cb.validate_seqno(sequence_num);
			slotB_cb.inccnt(pkt.size());
		}


		if (!isSLOTAHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_A_SUM_16b)
		{

			std::ostringstream oss;
			isSLOTAHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data"
				<< std::endl;
			slotA_cb.addcsv(oss.str());
		}
		else if (!isSLOTBHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_B_SUM_16b)
		{
			std::ostringstream oss;
			isSLOTBHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data"
				<< std::endl;
			slotB_cb.addcsv(oss.str());
		}


		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd_stream_cb_data_t_sum16 *pAdpdItem = &pkt[i];
			std::ostringstream oss;

			oss << pAdpdItem->datatype << ", "
				<< pAdpdItem->sum_timestamp << ", "
				<< (uint16_t)pAdpdItem->sum16_adpd_data << ", "
				<< std::endl;
			if (pAdpdItem->datatype == RAW_DATA_ADPD_A_SUM_16b)
			{
				slotA_cb.addcsv(oss.str());
			}
			else
			{
				slotB_cb.addcsv(oss.str());
			}
		}
	};
	void call(std::vector<adpd_stream_cb_data_t_4CH32> pkt, uint16_t sequence_num) {

		if (pkt[0].datatype == RAW_DATA_ADPD_A_4CH_32b)
		{
			slotA_cb.validate_seqno(sequence_num);
			slotA_cb.inccnt(pkt.size());
		}
		else
		{
			slotB_cb.validate_seqno(sequence_num);
			slotB_cb.inccnt(pkt.size());
		}

		if (!isSLOTAHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_A_4CH_32b)
		{
			std::ostringstream oss;
			isSLOTAHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data_CH1" << ","
				<< "ADPD_Data_CH2" << ","
				<< "ADPD_Data_CH3" << ","
				<< "ADPD_Data_CH4" << ","
				<< std::endl;
			slotA_cb.addcsv(oss.str());
		}
		else if (!isSLOTBHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_B_4CH_32b)
		{
			std::ostringstream oss;
			isSLOTBHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data_CH1" << ","
				<< "ADPD_Data_CH2" << ","
				<< "ADPD_Data_CH3" << ","
				<< "ADPD_Data_CH4" << ","
				<< std::endl;
			slotB_cb.addcsv(oss.str());
		}

		

		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd_stream_cb_data_t_4CH32 *pAdpdItem = &pkt[i];

				std::cout << "Channel Size - " << pAdpdItem->ch_data.size() << std::endl;
				std::ostringstream oss;
				oss << pAdpdItem->datatype << ", "
					<< pAdpdItem->ch_timestamp << ", ";
				for (int k = 0; k < pAdpdItem->ch_data.size(); k++)
				{
					oss << pAdpdItem->ch_data[k] << ",";
				}
				oss << std::endl;
				if (pAdpdItem->datatype == RAW_DATA_ADPD_A_4CH_32b)
				{
					slotA_cb.addcsv(oss.str());
				}
				else
				{
					slotB_cb.addcsv(oss.str());
				}
		}
	};
	void call(std::vector<adpd_stream_cb_data_t_4CH16> pkt, uint16_t sequence_num) {

		if (pkt[0].datatype == RAW_DATA_ADPD_A_4CH_16b)
		{
			slotA_cb.validate_seqno(sequence_num);
			slotA_cb.inccnt(pkt.size());
		}
		else
		{
			slotB_cb.validate_seqno(sequence_num);
			slotB_cb.inccnt(pkt.size());
		}

		if (!isSLOTAHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_A_4CH_16b)
		{
			std::ostringstream oss;
			isSLOTAHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data_CH1" << ","
				<< "ADPD_Data_CH2" << ","
				<< "ADPD_Data_CH3" << ","
				<< "ADPD_Data_CH4" << ","
				<< std::endl;
			slotA_cb.addcsv(oss.str());
		}
		else if(!isSLOTBHeaderWritten && pkt[0].datatype == RAW_DATA_ADPD_B_4CH_16b)
		{
			std::ostringstream oss;
			isSLOTBHeaderWritten = true;
			// ADPD header
			oss << "Datatype" << ", "
				<< "Timestamp" << ", "
				<< "ADPD_Data_CH1" << ","
				<< "ADPD_Data_CH2" << ","
				<< "ADPD_Data_CH3" << ","
				<< "ADPD_Data_CH4" << ","
				<< std::endl;
			slotB_cb.addcsv(oss.str());
		}

		

		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd_stream_cb_data_t_4CH16 *pAdpdItem = &pkt[i];

				std::ostringstream oss;

				oss << pAdpdItem->datatype << ", "
					<< pAdpdItem->ch_timestamp << ", ";
				for (int k = 0; k < pAdpdItem->ch_data.size(); k++)
				{
					oss << pAdpdItem->ch_data[k] << ",";
				}
				oss << std::endl;
				if (pAdpdItem->datatype == RAW_DATA_ADPD_A_4CH_16b)
				{
					slotA_cb.addcsv(oss.str());
				}
				else
				{
					slotB_cb.addcsv(oss.str());
				}
		}
	};
};

class Adpd4000StreamCallback : public adpd4000_stream_callback, public CallbackHelper {
public:
	CallbackHelper adpd4xxx_cb;
	CallbackHelper adpd4xxxInterrupt_cb;
	Adpd4000StreamCallback() {
		adpd4xxx_cb = CallbackHelper();
		adpd4xxxInterrupt_cb = CallbackHelper();
		adpd4xxx_cb.setname("Adpd4xxx_");
		adpd4xxxInterrupt_cb.setname("Adpd4xxxInterrupt_");
	}

	void call(std::vector<adpd4000_stream_cb_data_t> pkt, uint16_t sequence_num) {

		adpd4xxx_cb.inccnt(pkt.size());

		if (!adpd4xxx_cb.isHeaderWritten)
		{
			adpd4xxx_cb.isHeaderWritten = true;
			std::ostringstream oss;
			// ADPD4000 header
			oss << "Source" << ", "
				<< "Dark" << ", "
				<< "Signal" << ", "
				<< "Impulse" << ", "
				<< "Lit" << ", "
				<< "Channel Number" << ", "
				<< "TimeStamp" << ", ";
			for (uint32_t i = 0; i < pkt.size(); i++) {
				adpd4000_stream_cb_data_t *pAdpdItem = &pkt[i];
				for (int j = 0; j < pAdpdItem->adpddata_d.size(); j++)
				{
					oss << "adpdData_D[" << j << "]" << ",";
				}
				for (int k = 0; k < pAdpdItem->adpddata_s.size(); k++)
				{
					oss << "adpdData_S[" << k << "]" << ",";
				}
				for (int i = 0; i < pAdpdItem->adpddata_l.size();i++)
				{
					oss << "adpdData_L[" << i << "]" << ",";
				}
			}
			oss << std::endl;
			adpd4xxx_cb.addcsv(oss.str());

		}

		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd4000_stream_cb_data_t *pAdpdItem = &pkt[i];
			std::ostringstream oss;
			if (pAdpdItem->adpd_stream == 0xC211) // Check Sequence number for ADPD4000 Stream 1
			{
				adpd4xxx_cb.validate_seqno(sequence_num);
			}
			oss << pAdpdItem->adpd_stream << ", "
				<< (int)pAdpdItem->dark << ", "
				<< (int)pAdpdItem->signal << ", "
				<< (int)pAdpdItem->impulse << ", "
				<< (int)pAdpdItem->lit << ", "
				<< (int)pAdpdItem->channel_num << ", "
				<< pAdpdItem->timestamp << ", ";
			for (int j = 0; j < pAdpdItem->adpddata_d.size(); j++)
			{
				oss << pAdpdItem->adpddata_d[j] << ",";
			}
			for (int k = 0; k < pAdpdItem->adpddata_s.size(); k++)
			{
				oss << pAdpdItem->adpddata_s[k] << ",";
			}
			for (int l = 0; l < pAdpdItem->adpddata_l.size(); l++)
			{
				oss << pAdpdItem->adpddata_l[l] << ",";
			}
			oss << std::endl;

			adpd4xxx_cb.addcsv(oss.str());
		}
	};

	void call(std::vector<adpd4000_optional_stream_cb_data_t> pkt, uint16_t sequence_num) {
		if (!adpd4xxxInterrupt_cb.isHeaderWritten)
		{
			adpd4xxxInterrupt_cb.isHeaderWritten = true;
			std::ostringstream oss;
			// ADPD4000 Interrupt header
			oss << "Timestamp" << ", "
				<< "Data Int" << ", "
				<< "Level0 Int" << ", "
				<< "Level1 Int" << ", "
				<< "TIA CH1" << ", "
				<< "TIA CH2";
			oss << std::endl;
			adpd4xxxInterrupt_cb.addcsv(oss.str());

		}
		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd4000_optional_stream_cb_data_t *pAdpdItem = &pkt[i];
			std::ostringstream oss;

			oss << sequence_num << ","
				<< pAdpdItem->data_int << ","
				<< pAdpdItem->level0_int << ","
				<< pAdpdItem->level1_int << ","
				<< pAdpdItem->tia_ch1_int << ","
				<< pAdpdItem->tia_ch2_int << ","
				<< std::endl;
			adpd4xxxInterrupt_cb.addcsv(oss.str());
		}
	};
};


class BCMStreamCallback : public bcm_stream_callback, public CallbackHelper
{
public:
	BCMStreamCallback()
	{
		setname("BCM_");
	}
	void call(std::vector<bcm_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		validate_seqno(sequence_num);
		inccnt(pkt.size());


		if (!isHeaderWritten)
		{
			isHeaderWritten = true;
			std::ostringstream oss;
			oss << "Timestamp" << ", "
				<< "Admittance Magnitudd" << ", "
				<< "Admittance Phase" << ", "
				<< "Impedance Magnitude" << ", "
				<< "Impedance Phase" << ", "
				<< std::endl;
			addcsv(oss.str());

		}


		std::cout << std::dec;
		for (uint32_t i = 0; i < pkt.size(); i++) {
			bcm_stream_cb_data_t *pbcmItem = &pkt[i];
			std::ostringstream oss;
			oss << pbcmItem->timestamp << ", " << pbcmItem->admittance_magnitude << ", "
				<< pbcmItem->admittance_phase << ", "
				<< pbcmItem->impedance_magnitude << ", "
				<< pbcmItem->impedance_phase << std::endl;
			addcsv(oss.str());
		}
	 
	}
};

class EcgStreamCallback: public ecg_stream_callback, public CallbackHelper {
public:
	EcgStreamCallback() {
		setname("ECG_");
	}

	void call(std::vector<ecg_stream_cb_data_t> pkt, uint16_t sequence_num) {
		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			isHeaderWritten = true;
			std::ostringstream oss;
			oss << "Timestamp" << ", "
				<< "LeadsOff" << ", "
				<< "HR" << ", "
				<< "ECG Data" << ", " 
				<< std::endl;
			addcsv(oss.str());

		}

		std::cout << std::dec;
		for (uint32_t i = 0; i < pkt.size(); i++) {
			ecg_stream_cb_data_t *pEcgItem = &pkt[i];
			std::ostringstream oss;
			oss << pEcgItem->timestamp << ", "
					<< ((pEcgItem->leadsoff) ? "Off" : "On") << ", "
					<< (uint16_t) pEcgItem->HR << ", " << pEcgItem->ecg_data
					<< std::endl;
			addcsv(oss.str());
		}
	};
};

class TemperatureStreamCallback : public temperature_stream_callback, public CallbackHelper {
public:
	TemperatureStreamCallback() {
		setname("TEMPARATURE_");

	}

	void call(std::vector<temperature_stream_cb_data_t> pkt,
		uint16_t sequence_num) {
		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			isHeaderWritten = true;
			std::ostringstream oss;
			oss << "Timestamp" << ", "
				<< "TEMP SKIN" << ", "
				<< "TEMP AMBIENT" << ", "
				<< std::endl;
			addcsv(oss.str());

		}

		for (uint32_t i = 0; i < pkt.size(); i++) {
			temperature_stream_cb_data_t *pEcgItem = &pkt[i];
			std::ostringstream oss;
			oss << pEcgItem->timestamp << ", " << pEcgItem->temp_skin << ", "
				<< pEcgItem->temp_ambient << std::endl;
			addcsv(oss.str());
		}
	};
};

class EdaStreamCallback: public eda_stream_callback, public CallbackHelper {
public:
	EdaStreamCallback() {
		setname("EDA_");
	}
	

	void call(std::vector<eda_stream_cb_data_t> pkt, uint16_t sequence_num) {
		validate_seqno(sequence_num);
		inccnt(pkt.size());

		if (!isHeaderWritten)
		{
			isHeaderWritten = true;
			std::ostringstream oss;
			oss << "Timestamp" << ", "
				<< "Admittance Magnitudd" << ", "
				<< "Admittance Phase" << ", "
				<< "Impedance Magnitude" << ", "
				<< "Impedance Phase" << ", "
				<< std::endl;
			addcsv(oss.str());

		}

		std::cout << std::dec;
		for (uint32_t i = 0; i < pkt.size(); i++) {
			eda_stream_cb_data_t *pEdaItem = &pkt[i];
			std::ostringstream oss;
			oss << pEdaItem->timestamp << ", " << pEdaItem->admittance_magnitude << ", "
					<< pEdaItem->admittance_phase << ", "
				<< pEdaItem->impedance_magnitude << ", "
				<< pEdaItem->impedance_phase << std::endl;
			addcsv(oss.str());
		}
	};
};
class TXCallback : public watch_phy_callback {
private:
	OVERLAPPED overlapped;
	Serial *port;
public:
	TXCallback(Serial *_port, OVERLAPPED &_overlapped) {
		port = _port;
		overlapped = _overlapped;
	}
	void call(std::vector<uint8_t> pkt) {
#ifdef TXRAWPACKET
		std::cout << "Tx:" << std::endl;
		for (uint32_t i = 0; i < pkt.size(); i++) {
			std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << (int)pkt[i];
		}
		std::cout << std::endl;
#endif
		try
		{
			if (port->isOpen())
			{
				uint8_t *pkt_out = (uint8_t *)&pkt[0];
				port->write(pkt_out,
					(unsigned long)(pkt.size() * sizeof(uint8_t)));
			}
		}
		catch (std::exception & e)
		{
		/*	if (e.m_dwError == ERROR_IO_PENDING)
			{
				DWORD dwBytesTransferred = 0;
				port.GetOverlappedResult(overlapped, dwBytesTransferred, TRUE);
			}
			else
			{
				CSerialPort::ThrowSerialException(e.m_dwError);
			}*/
		}
	};

	void sys_alert_call(uint8_t alert_message)
	{
		if (alert_message == ADI_SDK_PACKET_TIMED_OUT)
			std::cout << "Error in receiving packet" << std::endl;
	};
};


DWORD WINAPI RxThread(LPVOID lpParam)
{
#if _WIN64
	uint64_t *params = (uint64_t *)lpParam;
	uint64_t *terminate = (uint64_t *)params[0];
#else
	uint32_t *params = (uint32_t *)lpParam;
	uint32_t *terminate = (uint32_t *)params[0];
#endif
	
	watch *mw = (watch *)params[1];
	Serial *my_serial = (Serial *)params[2];
	while (*terminate == 0) {
		if (my_serial->isOpen() && (my_serial->available() >= 8)) {
			uint8_t pkt[8 + 2048];
			uint32_t br;
			memset(pkt, 0, 8 + 2048);
			br = my_serial->read((uint8_t *)pkt, 8);
#ifdef RXRAWPACKET
			std::cout << "Rx:" << std::endl;
			for (uint32_t i = 0; i < br; i++) {
				std::cout << " " << std::hex << std::setfill('0')
					<< std::setw(2) << (int)pkt[i];
			}
			std::cout << std::endl;
#endif
			uint32_t len = ((pkt[4] << 8) + pkt[5]);
			uint32_t payload_len = len - 8;
			while (my_serial->available() < payload_len);
			br = my_serial->read((uint8_t *)&pkt[8], payload_len);
#ifdef RXRAWPACKET
			for (uint32_t i = 0; i < br; i++) {
				std::cout << " " << std::hex << std::setfill('0')
					<< std::setw(2) << (int)pkt[8 + i];
			}
			std::cout << std::endl;
#endif
			std::vector<uint8_t> vec_pkt(std::begin(pkt), &pkt[len]);

			mw->dispatch(vec_pkt);
			
			}
			}
	//std::cout << "RxThread Terminate" << std::endl;
	return 0;
}
int num = 1;
int32_t L1StreamTest(watch *pMw,
	uint32_t streamtime, 
	bool adpd, /*ADPD*/
	bool adpd4000, /*ADPD4000*/
	bool adxl, /*ADXL*/ 
	bool ppg, /*PPG*/
	bool ecg, /*ECG*/
	bool eda, /*EDA*/
	bool temparature /*TEMP*/

	){ 
	 
	AdpdStreamCallback adpd_stream_callback = AdpdStreamCallback();
	AdxlStreamCallback adxl_stream_callback = AdxlStreamCallback();
	Adpd4000StreamCallback adpd4000_stream_callback = Adpd4000StreamCallback();
	PpgStreamCallback ppg_stream_callback = PpgStreamCallback();
	SyncPPGStreamCallback syncppg_stream_callback = SyncPPGStreamCallback();
	EcgStreamCallback ecg_stream_callback = EcgStreamCallback();
	EdaStreamCallback eda_stream_callback = EdaStreamCallback();
	TemperatureStreamCallback temp_stream_callback = TemperatureStreamCallback();

	pm_sys_info_t sys_info;


	pMw->pm_app.get_system_info(&sys_info);
	if (ppg)
	{
		std::cout << "Started Writing L1 PPG stream" << std::endl;
		ppg_stream_callback.zerocnt();
		ppg_stream_callback.opencsv();
		syncppg_stream_callback.zerocnt();
		syncppg_stream_callback.opencsv();
		pMw->adpd_app.load_cfg(pMw->adpd_app.ADPD_DEVICE_108);
		pMw->adpd_app.calibrate_clock();
		pMw->ppg_app.load_lcfg(pMw->ppg_app.PPG_LCFG_ID_ADPD108);
		pMw->syncppg_app.syncppg_stream.subscribe(syncppg_stream_callback);
		pMw->ppg_app.ppg_stream.subscribe(ppg_stream_callback);
		pMw->ppg_app.ppg_stream.start();
		Sleep(streamtime * 1000); //10 seconds
		std::cout << "Finished Writing L1 PPG stream" << std::endl;
		pMw->syncppg_app.syncppg_stream.unsubscribe();
		pMw->ppg_app.ppg_stream.unsubscribe();
		pMw->ppg_app.ppg_stream.stop();
		ppg_stream_callback.closecsv();
		ppg_stream_callback.displaycnt(streamtime);
		syncppg_stream_callback.closecsv();
		syncppg_stream_callback.displaycnt(streamtime);
	}


	if (adpd4000)
	{
		std::cout << "Started Writing L1 ADPD4000 stream" << std::endl;
		adpd4000_stream_callback.adpd4xxx_cb.zerocnt();
		adpd4000_stream_callback.adpd4xxx_cb.opencsv();
		adpd4000_stream_callback.adpd4xxxInterrupt_cb.zerocnt();
		adpd4000_stream_callback.adpd4xxxInterrupt_cb.opencsv();
		//pMw->adpd4000_app.set_slot(1, 1, 0x3243, 1);
		pMw->adpd4000_app.adpd4000_optional_stream.subscribe(adpd4000_stream_callback);
		pMw->adpd4000_app.adpd4000_stream1.subscribe(adpd4000_stream_callback);
		pMw->adpd4000_app.adpd4000_stream2.subscribe(adpd4000_stream_callback);
		pMw->adpd4000_app.adpd4000_stream1.start();
		Sleep(streamtime * 1000); //10 seconds
		std::cout << "Finished Writing L1 ADPD4000 stream" << std::endl;
		pMw->adpd4000_app.adpd4000_stream1.unsubscribe();
		pMw->adpd4000_app.adpd4000_stream2.unsubscribe();
		pMw->adpd4000_app.adpd4000_optional_stream.unsubscribe();
		pMw->adpd4000_app.adpd4000_stream1.stop();
		adpd4000_stream_callback.adpd4xxx_cb.closecsv();
		adpd4000_stream_callback.adpd4xxx_cb.displaycnt(streamtime);
		adpd4000_stream_callback.adpd4xxxInterrupt_cb.closecsv();
		adpd4000_stream_callback.adpd4xxxInterrupt_cb.displaycnt(streamtime);

	}

	if (adpd) {
		std::cout << "Started Writing L1 ADPD stream" << std::endl;
		adpd_stream_callback.slotA_cb.zerocnt();
		adpd_stream_callback.slotA_cb.opencsv();
		adpd_stream_callback.slotB_cb.zerocnt();
		adpd_stream_callback.slotB_cb.opencsv();
		pMw->adpd_app.calibrate_clock();
		pMw->adpd_app.set_slot(pMw->adpd_app.SLOTMODE_4CH_16b,
			pMw->adpd_app.SLOTMODE_DISABLED);
		pMw->adpd_app.adpd_stream.subscribe(adpd_stream_callback);
		pMw->adpd_app.adpd_stream.start();
		Sleep(streamtime *1000); // 10 seconds
		std::cout << "Finished Writing L1 ADPD stream" << std::endl;
		pMw->adpd_app.adpd_stream.stop();
		pMw->adpd_app.adpd_stream.unsubscribe();
		adpd_stream_callback.slotA_cb.closecsv();
		adpd_stream_callback.slotB_cb.closecsv();
		adpd_stream_callback.slotA_cb.displaycnt(streamtime);
		adpd_stream_callback.slotB_cb.displaycnt(streamtime);

	}


	if (adxl) {
		std::cout << "Started Writing L1 ADXL stream" << std::endl;
		adxl_stream_callback.zerocnt();
		adxl_stream_callback.opencsv();
		ret::sdk_status status = pMw->adxl_app.adxl_stream.subscribe(adxl_stream_callback);
		std::cout << "SUB STATUS -  " << status << std::endl;

		m2m2_application::APP_COMMON_STATUS_ENUM_t start_status = pMw->adxl_app.adxl_stream.start();
		std::cout << "START STATUS -  " << start_status << std::endl;

		Sleep(streamtime * 1000); //10 seconds
		std::cout << "Finished Writing L1 ADXL stream" << std::endl;
		pMw->adxl_app.adxl_stream.stop();
		pMw->adxl_app.adxl_stream.unsubscribe();
		adxl_stream_callback.closecsv();
		adxl_stream_callback.displaycnt(streamtime);
	}

	if (ecg) {
		std::cout << "Started Writing L1 ECG stream" << std::endl;
		ecg_stream_callback.zerocnt();
		ecg_stream_callback.opencsv();
		if (sys_info.hw_id == HW_ID_MUSKA)
		{
			std::vector<std::pair<uint8_t, uint16_t>> addr_vals;
			std::vector<std::pair<uint8_t, uint16_t>> ret_vals;
			addr_vals.push_back(std::make_pair(static_cast<uint8_t>(0), static_cast<uint16_t>(100))); //ECG freq = 100
			addr_vals.push_back(std::make_pair(static_cast<uint8_t>(2), static_cast<uint16_t>(1))); // SPORT
			addr_vals.push_back(std::make_pair(static_cast<uint8_t>(5), static_cast<uint16_t>(0)));  // CASE
			ret_vals = pMw->ecg_app.lcfg_write(addr_vals);
			pMw->ad5940_app.start();
			pMw->ecg_app.ecg_stream.start();
		}
		else
		{
			pMw->ecg_app.ecg_stream.start();
			pMw->ad7689_app.start();
		}
		Sleep(streamtime * 1000); //10 seconds
		std::cout << "Finished Writing L1 ECG stream" << std::endl;
		if (sys_info.hw_id == HW_ID_MUSKA)
		{
			pMw->ad5940_app.stop();
		}
		else
		{
			pMw->ad7689_app.stop();
		}
		pMw->ecg_app.ecg_stream.stop();
		pMw->ecg_app.ecg_stream.unsubscribe();
		ecg_stream_callback.closecsv();
		ecg_stream_callback.displaycnt(streamtime);
	}

	if (temparature)
	{
		std::cout << "Started Writing L1 TEMPARATURE stream" << std::endl;
		temp_stream_callback.zerocnt();
		temp_stream_callback.opencsv();
		pMw->temperature_app.temperature_stream.subscribe(temp_stream_callback);
		if (sys_info.hw_id == HW_ID_MUSKA)
		{
			pMw->ad5940_app.start();
			pMw->temperature_app.temperature_stream.start();
		}
		else
		{
			pMw->temperature_app.temperature_stream.start();
			pMw->ad7689_app.start();
		}

		Sleep(streamtime * 1000);

		std::cout << "Finished Writing L1 TEMPARATURE stream" << std::endl;
		if (sys_info.hw_id == HW_ID_MUSKA)
		{
			pMw->ad5940_app.stop();
		}
		else
		{
			pMw->ad7689_app.stop();
		}
		pMw->temperature_app.temperature_stream.stop();
		pMw->temperature_app.temperature_stream.unsubscribe();
		temp_stream_callback.closecsv();
		temp_stream_callback.displaycnt(streamtime);
		
	}

	if (eda) {
		std::cout << "Started Writing L1 EDA stream" << std::endl;
		eda_stream_callback.zerocnt();
		eda_stream_callback.opencsv();
		pMw->eda_app.eda_stream.subscribe(eda_stream_callback);
		if (sys_info.hw_id == HW_ID_MUSKA)
		{
			pMw->ad5940_app.start();
			pMw->eda_app.eda_stream.start();

		}
		else
		{
			pMw->eda_app.eda_stream.start();
			pMw->ad7689_app.start();
		}

		Sleep(streamtime * 1000); //10 seconds
		std::cout << "Finished Writing L1 EDA stream" << std::endl;
		if (sys_info.hw_id == HW_ID_MUSKA)
		{
			pMw->ad5940_app.stop();
		}
		else
		{
			pMw->ad7689_app.stop();
		}
		pMw->eda_app.eda_stream.stop();
		pMw->eda_app.eda_stream.unsubscribe();
		eda_stream_callback.closecsv();
		eda_stream_callback.displaycnt(streamtime);
	}



	return 0;
}

int32_t L2StreamTest(watch *pMw,
	uint32_t streamtime,
	bool adpd,
	bool adpd4000,
	bool adxl,
	bool ppg,
	bool ecg,
	bool eda,
	bool temparature /*TEMP*/,
	bool bcm,
	bool hrv,
	bool pedometer) {

	AdxlStreamCallback adxl_stream_callback = AdxlStreamCallback();
	Adpd4000StreamCallback adpd4000_stream_callback = Adpd4000StreamCallback();
	AdpdStreamCallback adpd_stream_callback = AdpdStreamCallback();
	PpgStreamCallback ppg_stream_callback = PpgStreamCallback();
	SyncPPGStreamCallback syncppg_stream_callback = SyncPPGStreamCallback();
	EcgStreamCallback ecg_stream_callback = EcgStreamCallback();
	EdaStreamCallback eda_stream_callback = EdaStreamCallback();
	TemperatureStreamCallback temp_stream_callback = TemperatureStreamCallback();
	BCMStreamCallback bcm_stream_callback = BCMStreamCallback();
	AGCStreamCallback agc_stream_callback = AGCStreamCallback();
	HRVStreamCallback hrv_stream_callback = HRVStreamCallback();
	PedometerStreamCallback ped_streamcallback = PedometerStreamCallback();

	if (ppg)
	{
		std::cout << "Started Writing L2 PPG stream" << std::endl;
		//PPG File Write
		ppg_stream_callback.zerocnt();
		ppg_stream_callback.opencsv();
		//Syncppg File write
		syncppg_stream_callback.zerocnt();
		syncppg_stream_callback.opencsv();
		pMw->start_syncppg(ppg_stream_callback, syncppg_stream_callback);

	}


	if (adpd)
	{
		std::cout << "Started Writing L2 ADPD stream" << std::endl;
		//slot A file write
		adpd_stream_callback.slotA_cb.zerocnt();
		adpd_stream_callback.slotA_cb.opencsv();
		//slot B file write
		adpd_stream_callback.slotB_cb.zerocnt();
		adpd_stream_callback.slotB_cb.opencsv();
		pMw->start_adpd(adpd_stream_callback);
	}

	if (adpd4000)
	{
		std::cout << "Started Writing L2 ADPD4000 stream" << std::endl;
		adpd4000_stream_callback.adpd4xxx_cb.zerocnt();
		adpd4000_stream_callback.adpd4xxx_cb.opencsv();
		pMw->adpd4000_app.load_cfg();
		pMw->start_adpd4000(adpd4000_stream_callback);
	}

	if (adxl) {
		std::cout << "Started Writing L2 ADXL stream" << std::endl;
		adxl_stream_callback.zerocnt();
		adxl_stream_callback.opencsv();
		pMw->start_adxl(adxl_stream_callback);
	}

	if (ecg) {
		std::cout << "Started Writing L2 ECG stream" << std::endl;
		ecg_stream_callback.zerocnt();
		ecg_stream_callback.opencsv();
		pMw->start_ecg(ecg_stream_callback);
	}

	if (eda) {
		std::cout << "Started Writing L2 EDA stream" << std::endl;
		eda_stream_callback.zerocnt();
		eda_stream_callback.opencsv();
		pMw->start_eda(eda_stream_callback);
	}

	if (temparature)
	{
		std::cout << "Started Writing L2 TEMPARATURE stream" << std::endl;
		temp_stream_callback.zerocnt();
		temp_stream_callback.opencsv();
		pMw->start_temperature(temp_stream_callback);
	}

	if (bcm) {
		std::cout << "Started Writing L2 BCM stream" << std::endl;
		bcm_stream_callback.zerocnt();
		bcm_stream_callback.opencsv();
		pMw->start_bcm(bcm_stream_callback);
	}

	if (hrv) {
		std::cout << "Started Writing L2 HRV stream" << std::endl;
		hrv_stream_callback.zerocnt();
		hrv_stream_callback.opencsv();
		pMw->start_hrv(hrv_stream_callback);
	}

	if (pedometer) {
		std::cout << "Started Writing L2 PEDOMETER stream" << std::endl;
		ped_streamcallback.zerocnt();
		ped_streamcallback.opencsv();
		pMw->start_pedometer(ped_streamcallback);
	}

	Sleep(streamtime * 1000);

	if (ppg)
	{
		std::cout << "Finished Writing L2 PPG stream" << std::endl;
		pMw->stop_syncppg();
		ppg_stream_callback.closecsv();
		syncppg_stream_callback.closecsv();
		ppg_stream_callback.displaycnt(streamtime);
		syncppg_stream_callback.displaycnt(streamtime);
	}


	if (adpd)
	{
		std::cout << "Finished Writing L2 ADPD stream" << std::endl;
		pMw->stop_adpd();
		adpd_stream_callback.slotA_cb.closecsv();
		adpd_stream_callback.slotB_cb.closecsv();
		adpd_stream_callback.slotA_cb.displaycnt(streamtime);
		adpd_stream_callback.slotB_cb.displaycnt(streamtime);
	}

	if (adpd4000)
	{
		std::cout << "Finished Writing L2 ADPD4000 stream" << std::endl;
		pMw->stop_adpd4000();
		adpd4000_stream_callback.adpd4xxx_cb.closecsv();
		adpd4000_stream_callback.adpd4xxx_cb.displaycnt(streamtime);
	}

	if (adxl) {
		std::cout << "Finished Writing L2 ADXL stream" << std::endl;
		pMw->stop_adxl();
		adxl_stream_callback.closecsv();
		adxl_stream_callback.displaycnt(streamtime);
	}

	if (ecg) {
		std::cout << "Finished Writing L2 ECG stream" << std::endl;
		ecg_stream_callback.closecsv();
		ecg_stream_callback.displaycnt(streamtime);
	
		pMw->stop_ecg();
	}

	if (eda) {
		std::cout << "Finished Writing L2 EDA stream" << std::endl;
		eda_stream_callback.closecsv();
		eda_stream_callback.displaycnt(streamtime);
		
		pMw->stop_eda();
	}

	if (temparature)
	{
		std::cout << "Finished Writing L2 TEMPARATURE stream" << std::endl;
		temp_stream_callback.closecsv();
		temp_stream_callback.displaycnt(streamtime);

		pMw->stop_temperature();
	}

	if (bcm) {
		std::cout << "Started Writing L2 BCM stream" << std::endl;
		bcm_stream_callback.closecsv();
		bcm_stream_callback.displaycnt(streamtime);
		pMw->stop_bcm();
	}

	if (hrv) {
		std::cout << "Started Writing L2 HRV stream" << std::endl;
		hrv_stream_callback.closecsv();
		hrv_stream_callback.displaycnt(streamtime);
		pMw->stop_hrv();
	}

	if (pedometer) {
		std::cout << "Started Writing L2 PEDOMETER stream" << std::endl;
		ped_streamcallback.closecsv();
		ped_streamcallback.displaycnt(streamtime);
		pMw->stop_pedometer();
	}

	return 0;
}

void my_sleep(unsigned long milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds); // 100 ms
#else
	usleep(milliseconds * 1000); // 100 ms
#endif
}
                                                                                                                                                                                                                                                                                                                                                                                                           
int _tmain(int argc, _TCHAR* argv[]) {



	HANDLE hEvent = NULL;
	OVERLAPPED overlapped;
	DWORD   dwRxThread;
	HANDLE  hRxThread;

#if _WIN64
	uint64_t terminate;

#else
	uint32_t terminate;

#endif
	std::string port = "COM4";// enter the serial port number.ff

	Serial my_serial(port, 921600, serial::Timeout::simpleTimeout(1000));

	try {

		memset(&overlapped, 0, sizeof(overlapped));
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		overlapped.hEvent = hEvent;
	}
	catch (std::exception & e) {
		//CSerialPort::ThrowSerialException(e.m_dwError);
	}

	terminate = 0;

	TXCallback *g_phy_callback = new TXCallback(&my_serial, overlapped);
	watch mw = watch(*g_phy_callback);
	mw.self_test();
	


#if _WIN64
	uint64_t *params[] = { (uint64_t *)&terminate, (uint64_t *)&mw, (uint64_t *)&my_serial };

#else
	uint32_t *params[] = { (uint32_t *)&terminate, (uint32_t *)&mw, (uint32_t *)&my_serial };

#endif

	hRxThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		RxThread,				// thread function name
		params,				// argument to thread function 
		0,                      // use default creation flags 
		&dwRxThread);			// returns the thread identifier 

	my_serial.flush();
	mw.initialize_sdk(mw.windows);
	ret::device_identifiers device_id = mw.getDeviceID();


	if (hRxThread != NULL) {

		if (device_id <= ret::device_identifiers::SENSORBOARD3) // GLUE WATCH, SB3 
		{
			// Star L2 PPG stream
			L2StreamTest(&mw, 10,
				false, /* ADPD */
				false, /* ADPD4000 */
				false,/* ADXL */
				true, /* PPG */
				false,/* ECG */
				false,/* EDA */
				false, /* TEMP */
				false, /* BCM */
				false, /* HRV */
				false); /* PED */
		}
		else if (device_id == ret::device_identifiers::OPTICAL_BOARD_4000 || device_id == ret::device_identifiers::OPTICAL_BOARD_4100) // Check whether Chip id is ADPD
		{
			
				// Start L2 ADPD4000 stream
				L2StreamTest(&mw, 10,
					false, /* ADPD */
					true, /* ADPD4000 */
					false,/* ADXL */
					false, /* PPG */
					false,/* ECG */
					false,/* EDA */
					false, /* TEMP */
					false, /* BCM */
					false, /* HRV */
					false); /* PED */
	
		}
		else
		{
			pm_sys_info_t sys_info;
			mw.pm_app.get_system_info(&sys_info);
			if (sys_info.hw_id == ppg_hw_id_9031 || sys_info.hw_id == ppg_hw_id_9030)
			{
				// Star L2 ADPD stream
				L2StreamTest(&mw, 10,
					true, /* ADPD */
					false, /* ADPD4000 */
					false,/* ADXL */
					false, /* PPG */
					false,/* ECG */
					false,/* EDA */
					false, /* TEMP */
					false, /* BCM */
					false, /* HRV */
					false); /* PED */
			}
			else
			{
				// Star L2 ADXL stream
				L2StreamTest(&mw, 10,
					false, /* ADPD */
					false, /* ADPD4000 */
					true,/* ADXL */
					false, /* PPG */
					false,/* ECG */
					false,/* EDA */
					false, /* TEMP */
					false, /* BCM */
					false, /* HRV */
					false); /* PED */
			}
	
		}

		terminate = 1;
	}
	my_serial.close();
	for (int i = 0; i < 10; i++)
	{
		std::cout << "\rWindows will be automatically closed after " << 10 - i << " seconds ";
		Sleep(1000);
	}

	return 0;
}

