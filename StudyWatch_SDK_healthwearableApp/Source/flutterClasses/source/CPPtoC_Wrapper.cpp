#include "CPPtoC_Wrapper.h"
#include "watch.hpp"
#include <string>
#include "dart_api_dl.h"

static Tx_Callback TX_CB = NULL;
static adxl_stream_Cb ADXL_cb = NULL;
//static adpd_stream_sum ADPD_SUM_cb = NULL;
//static adpd_stream_4ch ADPD_4CH_cb = NULL;
static battery_stream_cb BATTERY_cb = NULL;
static adpd4xxx_stream_cb ADPD4XXX_cb = NULL;
static ppg_stream_cb PPG_cb = NULL;
static syncppg_stream_cb SYNCPPG_cb = NULL;
static ecg_stream_cb ECG_cb = NULL;
static eda_stream_cb EDA_cb = NULL;
static temperature_stream_cb TEMP_cb = NULL;
static adpd4xxx_interrupt_stream_cb ADPD4XXX_INTERRUPT_cb = NULL;
static pingStreamCB PING_cb = NULL;
static bcm_stream_cb BCM_cb = NULL;
static agc_stream_cb AGC_cb = NULL;
static pedometer_stream_cb PED_cb = NULL;
//static hrv_stream_cb HRV_cb = NULL;
static fs_ls_cb FS_LS_cb = NULL;
static fs_stream_cb FS_STREAM_cb = NULL;

static Dart_Port send_port;
static Dart_Port adpd_port;
static Dart_Port adxl_port;
static Dart_Port syncppg_port;
static Dart_Port ppg_port;
static Dart_Port ecg_port;
static Dart_Port eda_port;
static Dart_Port temp_port;


static sdkAlertCB SDKALERT_CB = NULL;
//#define TXRAWPACKET
//#define RXRAWPACKET


std::string hexStr(char *data, int len){
    std::stringstream ss;
    ss << std::hex;

     for( int i(0) ; i < len; ++i )
         ss << "0X" << std::setw(4) << std::setfill('0') << (int)data[i] << ":";

     return ss.str();
}

class TXCallback : public watch_phy_callback {
public:
	void call(std::vector<uint8_t> pkt) {
#ifdef TXRAWPACKET
		std::cout << "Tx:" << std::endl;
		for (uint32_t i = 0; i < pkt.size(); i++) {
			std::cout << " " << std::hex << std::setfill('0') << std::setw(2) << (int)pkt[i];
		}
		std::cout << std::endl;
#endif
		
		char *pTxByte = new char[pkt.size()];
 
		for (int i = 0; i < pkt.size(); i++)
		{
			pTxByte[i] = (char)pkt[i];
		}
		int64_t pktSz = static_cast<int64_t>(pkt.size());
		std::string hexString = hexStr(pTxByte, pkt.size());
		Dart_CObject obj = Dart_CObject();
		obj.type = Dart_CObject_kString;

		obj.value.as_string = new char[hexString.length() + 1];
		strcpy(obj.value.as_string, hexString.c_str());

        const bool result = Dart_PostCObject_DL(send_port, &obj);

        if (!result) {
            std::cout << "C   :  Posting message to port failed." << std::endl;
        }

		//	if (TX_CB != NULL)
		//		TX_CB(pTxByte, pkt.size());

		delete pTxByte;		
	};

	void sys_alert_call(uint8_t sys_alert) {
	    if (SDKALERT_CB != NULL)
	        SDKALERT_CB(sys_alert);
		//std::cout << "sys_alert --: \n" << (int)sys_alert << std::endl;
	};
};

class AdxlStreamCallback : public adxl_stream_callback {
public:

	void call(std::vector<adxl_stream_cb_data_t> pkt, uint16_t sequence_num) {
		for (uint32_t i = 0; i < pkt.size(); i++) {
			adxl_stream_cb_data_t *pAdxlItem = &pkt[i];
			adxl_stream_t adxl_data = { 0 };
			adxl_data.sequence_number = sequence_num;
			adxl_data.timestamp = pAdxlItem->timestamp;
			adxl_data.x = pAdxlItem->x;
			adxl_data.y = pAdxlItem->y;
			adxl_data.z = pAdxlItem->z;

			Dart_CObject obj = Dart_CObject();
            obj.type = Dart_CObject_kString;
            //char *temp_byte = new char[hexString.length() + 1];

            std::string x_str = std::to_string(pAdxlItem->x);
            obj.value.as_string = new char[x_str.length() + 1];

            strcpy(obj.value.as_string, x_str.c_str());
           // strcpy(obj.value.as_string, hexString.c_str());

        const bool result = Dart_PostCObject_DL(adxl_port, &obj);

        if (!result) {
            std::cout << "C   :  Posting message to port failed." << std::endl;
        }

			//if (ADXL_cb != NULL)
			//	ADXL_cb(&adxl_data);
			//else
			//	std::cout << "ADXL Callback Not Initialized" << std::endl;
			
		}
	};
};


/*class AdpdStreamCallback : public adpd_stream_callback {
public:
	
	void call(std::vector<adpd_stream_cb_data_t_sum32> pkt, uint16_t sequence_num) {

		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd_stream_cb_data_t_sum32 *pAdpdItem = &pkt[i];
			adpd_stream_cb_data_t_sum pData;
			pData.seqnum = sequence_num;
			pData.adpd_data = pAdpdItem->sum32_adpd_data;
			pData.timestamp = pAdpdItem->sum_timestamp;
			if(ADPD_SUM_cb)
				ADPD_SUM_cb(&pData);
			else
				std::cout << "ADPD SUM Callback Not Initialized" << std::endl;

		}

	};

	void call(std::vector<adpd_stream_cb_data_t_sum16> pkt, uint16_t sequence_num) {
		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd_stream_cb_data_t_sum16 *pAdpdItem = &pkt[i];
			adpd_stream_cb_data_t_sum pData = { 0 };

			pData.seqnum = sequence_num;
			pData.adpd_data = pAdpdItem->sum16_adpd_data;
			pData.timestamp = pAdpdItem->sum_timestamp;
			if (ADPD_SUM_cb)
				ADPD_SUM_cb(&pData);
			else
				std::cout << "ADPD SUM Callback Not Initialized" << std::endl;

		}
	};
	void call(std::vector<adpd_stream_cb_data_t_4CH16> pkt, uint16_t sequence_num) {
		for (uint32_t i = 0; i < pkt.size(); i++) {

			adpd_stream_cb_data_t_4CH16 *pAdpdItem = &pkt[i];
			adpd_stream_cb_data_t_4ch pData = { 0 };

			pData.seqnum = (uint16_t) sequence_num;
			pData.timestamp = (double)pAdpdItem->ch_timestamp;
			for (int i = 0; i < 4; i++)
			{
				pData.ch_data[i] =(uint32_t) pAdpdItem->ch_data[i];
			}
			if (ADPD_4CH_cb)
				ADPD_4CH_cb(&pData);
			else
				std::cout << "ADPD 4CH Callback Not Initialized" << std::endl;

		}

	};
	void call(std::vector<adpd_stream_cb_data_t_4CH32> pkt, uint16_t sequence_num) {
		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd_stream_cb_data_t_4CH32 *pAdpdItem = &pkt[i];
			adpd_stream_cb_data_t_4ch pData = { 0 };

			pData.seqnum = sequence_num;
			pData.timestamp = pAdpdItem->ch_timestamp;

			for (int i = 0; i < 4; i++)
			{
				pData.ch_data[i] = pAdpdItem->ch_data[i];

			}
			if (ADPD_4CH_cb)
				ADPD_4CH_cb(&pData);
			else
				std::cout << "ADPD 4CH Callback Not Initialized" << std::endl;

		}

	};
};*/


class BatteryStreamCallback : public battery_stream_callback
{
public:
	void call(std::vector<battery_stream_cb_data_t> pkt)
	{
		battery_stream_cb_data_t *pBatInfo = &pkt[0];

		pm_sys_bat_info bat_info = { 0 };
		bat_info.bat_chrg_stat = pBatInfo->bat_chrg_stat;
		bat_info.bat_lvl = pBatInfo->bat_lvl;
		bat_info.bat_mv = pBatInfo->bat_mv;
		bat_info.bat_temp = pBatInfo->bat_temp;
		bat_info.timestamp = pBatInfo->timestamp;

		if (BATTERY_cb != NULL)
			BATTERY_cb(&bat_info);
		else
			std::cout << "Battery Callback Not Initialized" << std::endl;

	}

};

class Adpd4000StreamCallback : public adpd4000_stream_callback {
public:

	void call(std::vector<adpd4000_stream_cb_data_t> pkt, uint16_t sequence_num) {
		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd4000_stream_cb_data_t *pAdpdItem = &pkt[i];
			adpd4000_stream_cb_data adpd4xxx_data;
			adpd4xxx_data.dark = pAdpdItem->dark;
			adpd4xxx_data.signal = pAdpdItem->signal;
			adpd4xxx_data.impulse = pAdpdItem->impulse;
			adpd4xxx_data.lit = pAdpdItem->lit;
			adpd4xxx_data.timestamp = pAdpdItem->timestamp;
			adpd4xxx_data.seq_num = sequence_num;
			adpd4xxx_data.channel_num = pAdpdItem->channel_num;
			adpd4xxx_data.adpd_d_length = pAdpdItem->adpddata_d.size();
			adpd4xxx_data.adpd_s_length = pAdpdItem->adpddata_s.size();
			adpd4xxx_data.adpd_l_length = pAdpdItem->adpddata_l.size();
			adpd4xxx_data.src = static_cast<ADPD4XXX_SOURCE>(pAdpdItem->adpd_stream);

			Dart_CObject obj = Dart_CObject();
            obj.type = Dart_CObject_kString;
            //char *temp_byte = new char[hexString.length() + 1];

            std::string x_str = std::to_string(pAdpdItem->timestamp) + "," +
								std::to_string(pAdpdItem->channel_num)+ "," +
								std::to_string(pAdpdItem->adpddata_s.size());


			for (int j = 0; j < pAdpdItem->adpddata_d.size(); j++)
			{
				adpd4xxx_data.adpddata_d[j] = pAdpdItem->adpddata_d[j];
			}
			for (int k = 0; k < pAdpdItem->adpddata_s.size(); k++)
			{
				adpd4xxx_data.adpddata_s[k] = pAdpdItem->adpddata_s[k];
				x_str += "," + std::to_string(pAdpdItem->adpddata_s[k]);
			}
			for (int j = 0; j < pAdpdItem->adpddata_l.size(); j++)
			{
				adpd4xxx_data.adpddata_l[j] = pAdpdItem->adpddata_l[j];
			}

			
            obj.value.as_string = new char[x_str.length() + 1];

            strcpy(obj.value.as_string, x_str.c_str());
            // strcpy(obj.value.as_string, hexString.c_str());

            const bool result = Dart_PostCObject_DL(adpd_port, &obj);

            if (!result) {
                std::cout << "C   :  Posting message to port failed." << std::endl;
            }

			// if (ADPD4XXX_cb != NULL)
			// 	ADPD4XXX_cb(&adpd4xxx_data);
			// else
			// 	std::cout << "ADPD4xxx Callback Not Initialized" << std::endl;

		}
	}
	void call(std::vector<adpd4000_optional_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		for (uint32_t i = 0; i < pkt.size(); i++) {
			adpd4000_optional_stream_cb_data_t *pAdpdItem = &pkt[i];
			adpd4000_interrupt_stream_cb_data_t adpd4xxx_data;
			adpd4xxx_data.seq_num = sequence_num;
			adpd4xxx_data.timestamp = pAdpdItem->timestamp;
			adpd4xxx_data.data_int = pAdpdItem->data_int;
			adpd4xxx_data.level0_int = pAdpdItem->level0_int;
			adpd4xxx_data.level1_int = pAdpdItem->level1_int;
			adpd4xxx_data.tia_ch1_int = pAdpdItem->tia_ch1_int;
			adpd4xxx_data.tia_ch2_int = pAdpdItem->tia_ch2_int;

			if (ADPD4XXX_INTERRUPT_cb != NULL)
			{
				ADPD4XXX_INTERRUPT_cb(&adpd4xxx_data);
			}
			else
			{
				std::cout << "ADPD4xxx Interrupt stream Callback was Not Initialized" << std::endl;
			}


		}
	}
};

class SyncPPGStreamCallback : public syncppg_stream_callback {
public:
	
	void call(std::vector<syncppg_stream_cb_data_t> pkt, uint16_t sequence_num) {

		for (int i = 0; i < pkt.size(); i++) {
			syncppg_stream_cb_data_t *syncppgStreamCbData = &pkt[i];
			syncppg_stream_cb_data pData = { 0 };
			pData.ppg = syncppgStreamCbData->ppg;
			pData.x = syncppgStreamCbData->x;
			pData.y = syncppgStreamCbData->y;
			pData.z = syncppgStreamCbData->z;
			pData.timestamp = syncppgStreamCbData->timestamp;
			pData.seqnum = sequence_num;

			Dart_CObject obj = Dart_CObject();
            obj.type = Dart_CObject_kString;
            //char *temp_byte = new char[hexString.length() + 1];

            std::string x_str = std::to_string(syncppgStreamCbData->timestamp) + "," + std::to_string(syncppgStreamCbData->ppg) + "," +
								std::to_string(syncppgStreamCbData->x) + "," + std::to_string(syncppgStreamCbData->y) + "," +
								std::to_string(syncppgStreamCbData->z);
            obj.value.as_string = new char[x_str.length() + 1];

            strcpy(obj.value.as_string, x_str.c_str());
            // strcpy(obj.value.as_string, hexString.c_str());

            const bool result = Dart_PostCObject_DL(syncppg_port, &obj);

            if (!result) {
                std::cout << "C   :  Posting message to port failed." << std::endl;
            }

			//if (SYNCPPG_cb != NULL)
			//	SYNCPPG_cb(&pData);
			//else
			//	std::cout << "SYNCPPG Callback Not Initialized" << std::endl;

		}
	};
};

class PpgStreamCallback : public ppg_stream_callback{
public:
	
	void call(std::vector<ppg_stream_cb_data_t> pkt, uint16_t sequence_num) {
		
		for (uint32_t i = 0; i < pkt.size(); i++) {
			ppg_stream_cb_data_t *pPpgItem = &pkt[i];
			ppg_stream_cb_data pData;
			pData.adpdlibstate = pPpgItem->adpdlibstate;
			pData.confidence = pPpgItem->confidence;
			pData.hr = pPpgItem->hr;
			pData.hr_type = pPpgItem->hr_type;
			pData.rr_interval = pPpgItem->rr_interval;
			pData.timestamp = pPpgItem->timestamp;
			pData.seqnum = sequence_num;

			Dart_CObject obj = Dart_CObject();
            obj.type = Dart_CObject_kString;
            
            std::string x_str = std::to_string(pPpgItem->timestamp) + "," + std::to_string(pPpgItem->hr);
            obj.value.as_string = new char[x_str.length() + 1];

            strcpy(obj.value.as_string, x_str.c_str());
            
            const bool result = Dart_PostCObject_DL(ppg_port, &obj);

            if (!result) {
                std::cout << "C   :  Posting message to port failed." << std::endl;
            }

			//if (PPG_cb != NULL)
			//	PPG_cb(&pData);
			//else
			//	std::cout << "PPG Callback Not Initialized" << std::endl;

		}
	};
};

class EcgStreamCallback : public ecg_stream_callback {
public:

	void call(std::vector<ecg_stream_cb_data_t> pkt, uint16_t sequence_num) {
		for (uint32_t i = 0; i < pkt.size(); i++) {
			ecg_stream_cb_data_t *pEcgItem = &pkt[i];
			ecg_stream_cb_data pData;
			pData.datatype = pEcgItem->datatype;
			pData.ecg_data = pEcgItem->ecg_data;
			pData.HR = pEcgItem->HR;
			pData.leadsoff = pEcgItem->leadsoff;
			pData.timestamp = pEcgItem->timestamp;
			pData.seqnum = sequence_num;

			Dart_CObject obj = Dart_CObject();
            obj.type = Dart_CObject_kString;
            
            std::string x_str = std::to_string(pEcgItem->timestamp) + "," + std::to_string(pEcgItem->ecg_data)
								+ "," + std::to_string(pEcgItem->HR);

            obj.value.as_string = new char[x_str.length() + 1];

            strcpy(obj.value.as_string, x_str.c_str());
            
            const bool result = Dart_PostCObject_DL(ecg_port, &obj);

            if (!result) {
                std::cout << "C   :  Posting message to port failed." << std::endl;
            }

			// if (ECG_cb != NULL)
			// 	ECG_cb(&pData);
			// else
			// 	std::cout << "ECG Callback Not Initialized" << std::endl;

		}
	};
};

class EdaStreamCallback : public eda_stream_callback {
public:

	void call(std::vector<eda_stream_cb_data_t> pkt, uint16_t sequence_num) {
		
		for (uint32_t i = 0; i < pkt.size(); i++) {
			eda_stream_cb_data_t *pEdaItem = &pkt[i];

			eda_stream_cb_data pData;
			pData.admittance_img = pEdaItem->admittance_img;
			pData.admittance_real = pEdaItem->admittance_real;
			pData.impedance_real = pEdaItem->impedance_real;
			pData.impedance_img = pEdaItem->impedance_img;
			pData.admittance_magnitude = pEdaItem->admittance_magnitude;
			pData.admittance_phase = pEdaItem->admittance_magnitude;
			pData.impedance_magnitude = pEdaItem->impedance_magnitude;
			pData.impedance_phase = pEdaItem->impedance_phase;
			pData.timestamp = pEdaItem->timestamp;
			pData.seqnum = sequence_num;

			Dart_CObject obj = Dart_CObject();
            obj.type = Dart_CObject_kString;
            
            std::string x_str = std::to_string(pEdaItem->timestamp)+ "," +
								std::to_string(pEdaItem->impedance_phase)+ "," + std::to_string(pEdaItem->impedance_magnitude)+ "," +
								std::to_string(pEdaItem->admittance_phase)+ "," + std::to_string(pEdaItem->admittance_magnitude);

            obj.value.as_string = new char[x_str.length() + 1];

            strcpy(obj.value.as_string, x_str.c_str());
            
            const bool result = Dart_PostCObject_DL(eda_port, &obj);

            if (!result) {
                std::cout << "C   :  Posting message to port failed." << std::endl;
            }

			// if (EDA_cb != NULL)
			// 	EDA_cb(&pData);
			// else
			// 	std::cout << "EDA Callback Not Initialized" << std::endl;

		}
	};
};

class TemperatureStreamCallback : public temperature_stream_callback {
public:
	void call(std::vector<temperature_stream_cb_data_t> pkt, uint16_t sequence_num) {
		for (uint32_t i = 0; i < pkt.size(); i++) {
			temperature_stream_cb_data_t *pTempItem = &pkt[i];
			temperature_stream_cb_data pData;
			pData.timestamp = pTempItem->timestamp;
			pData.temp_ambient = pTempItem->temp_ambient;
			pData.temp_skin = pTempItem->temp_skin;

			Dart_CObject obj = Dart_CObject();
            obj.type = Dart_CObject_kString;
            
            std::string x_str = std::to_string(pTempItem->timestamp) + "," + std::to_string(pTempItem->temp_skin)
								+ "," + std::to_string(pTempItem->temp_ambient);

            obj.value.as_string = new char[x_str.length() + 1];

            strcpy(obj.value.as_string, x_str.c_str());
            
            const bool result = Dart_PostCObject_DL(temp_port, &obj);

            if (!result) {
                std::cout << "C   :  Posting message to port failed." << std::endl;
            }

			// if (TEMP_cb != NULL)
			// 	TEMP_cb(&pData);
			// else
			// 	std::cout << "TEMPERATURE Callback Not Initialized" << std::endl;

		}
	};
};

class PingStreamCallback : public ping_stream_callback
{
public:
	void call(std::vector<ping_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		for (int i = 0; i < pkt.size(); i++) {
			ping_stream_cb_data_t *pingStreamCbData = &pkt[i];
			ping_stream_cb_data pData = { 0 };
			pData.pktSize = pingStreamCbData->pktSize;
		
			if (PING_cb != NULL)
				PING_cb(&pData);
			else
				std::cout << "PING Callback Not Initialized" << std::endl;

		}
	};
};

class BcmStreamCallback : public bcm_stream_callback
{
public:
	void call(std::vector<bcm_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		for (int i = 0; i < pkt.size(); i++) {
			bcm_stream_cb_data_t *bcmStreamCbData = &pkt[i];
			bcm_stream_cb_data pData = { 0 };
			pData.timestamp = bcmStreamCbData->timestamp;
			pData.signal_stability = bcmStreamCbData->signal_stability;
			pData.is_finger_on_leads = bcmStreamCbData->is_finger_on_leads;
			pData.impedance_real = bcmStreamCbData->impedance_real;
			pData.impedance_phase = bcmStreamCbData->impedance_phase;
			pData.impedance_magnitude = bcmStreamCbData->impedance_magnitude;
			pData.impedance_phase = bcmStreamCbData->impedance_phase;
			pData.admittance_magnitude = bcmStreamCbData->admittance_magnitude;
			pData.admittance_phase = bcmStreamCbData->admittance_phase;

			if (BCM_cb != NULL)
				BCM_cb(&pData);
			else
				std::cout << "BCM Callback Not Initialized" << std::endl;

		}
	};
};

class PedometerStreamCallback : public pedometer_stream_callback
{
public:
	void call(std::vector<pedometer_stream_cb_data_t> pkt, uint16_t sequence_num) {
		for (int i = 0; i < pkt.size(); i++) {
			pedometer_stream_cb_data_t *pedStreamCbData = &pkt[i];
			pedometer_stream_cb_data pData = { 0 };
			pData.AlgoStatus = pedStreamCbData->AlgoStatus;
			pData.NumSteps = pedStreamCbData->NumSteps;
			pData.Reserved = pedStreamCbData->Reserved;
			pData.TimeStamp = pedStreamCbData->TimeStamp;
			

			if (PED_cb != NULL)
				PED_cb(&pData);
			else
				std::cout << "Pedometer Callback Not Initialized" << std::endl;

		}
	}
};
/*
class HrvStreamCallback :public hrv_stream_callback
{
public:
	void call(std::vector<hrv_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		for (int i = 0; i < pkt.size(); i++) {
			hrv_stream_cb_data_t *hrvStreamCbData = &pkt[i];
			hrv_stream_cb_data pData = { 0 };
			pData.is_gap = hrvStreamCbData->is_gap;
			pData.rr_interval = hrvStreamCbData->rr_interval;
			pData.timestamp = hrvStreamCbData->timestamp;


			if (HRV_cb != NULL)
				HRV_cb(&pData);
			else
				std::cout << "HRV Callback Not Initialized" << std::endl;

		}
	}
};
*/
class AgcStreamCallback : public agc_stream_callback
{
public:
	void call(std::vector<agc_stream_cb_data_t> pkt, uint16_t sequence_num)
	{
		for (int i = 0; i < pkt.size(); i++) {
			agc_stream_cb_data_t *agcStreamCbData = &pkt[i];
			agc_stream_cb_data pData = { 0 };
			pData.mts0 = agcStreamCbData->mts0;
			pData.mts1 = agcStreamCbData->mts1;
			pData.mts2 = agcStreamCbData->mts2;
			pData.mts3 = agcStreamCbData->mts3;
			pData.mts4 = agcStreamCbData->mts4;
			pData.mts5 = agcStreamCbData->mts5;
			pData.setting0 = agcStreamCbData->setting0;
			pData.setting1 = agcStreamCbData->setting1;
			pData.setting2 = agcStreamCbData->setting2;
			pData.setting3 = agcStreamCbData->setting3;
			pData.setting4 = agcStreamCbData->setting4;
			pData.setting5 = agcStreamCbData->setting5;
			pData.setting6 = agcStreamCbData->setting6;
			pData.setting7 = agcStreamCbData->setting7;
			pData.setting8 = agcStreamCbData->setting8;
			pData.setting9 = agcStreamCbData->setting9;

			if (AGC_cb != NULL)
				AGC_cb(&pData);
			else
				std::cout << "AGC Callback Not Initialized" << std::endl;

		}
	}
};

class FsLsCallback : public fs_ls_callback
{
	void call(std::vector<fs_ls_data_t> pkt)
	{
		for (int i = 0; i < pkt.size(); i++) {
			fs_ls_data_t *fslsCbData = &pkt[i];
			fs_ls_data pData = { 0 };
			pData.status = fslsCbData->status;
			pData.filetype = fslsCbData->filetype;
			pData.filesize = fslsCbData->filesize;

			memcpy(pData.filename, fslsCbData->filename.c_str(), sizeof(fslsCbData->filename));
			if (FS_LS_cb != NULL)
				FS_LS_cb(&pData);
			else
				std::cout << "FS LS Callback Not Initialized" << std::endl;

		}
	};
};

class FsStreamCallback : public fs_stream_callback
{
	void call(std::vector<fs_stream_cb_data_t> pkt)
	{
		for (int i = 0; i < pkt.size(); i++) {
			fs_stream_cb_data_t *fsStreamCbData = &pkt[i];
			fs_stream_cb_data pData = { 0 };
			pData.status = fsStreamCbData->status;
			pData.length_Stream = fsStreamCbData->length_Stream;
			pData.crc16 = fsStreamCbData->crc16;

			if (FS_STREAM_cb != NULL)
				FS_STREAM_cb(&pData);
			else
				std::cout << "FS Stream Callback Not Initialized" << std::endl;

		}
	};
};


static AdxlStreamCallback ADXL_Stream_cb = AdxlStreamCallback();
//static AdpdStreamCallback ADPD_Stream_cb = AdpdStreamCallback();
static BatteryStreamCallback BAt_Stream_cb = BatteryStreamCallback();
static Adpd4000StreamCallback ADPD4xxx_stream_cb = Adpd4000StreamCallback();
static SyncPPGStreamCallback SYNCPPG_stream_cb = SyncPPGStreamCallback();
static PpgStreamCallback PPG_stream_cb = PpgStreamCallback();
static EcgStreamCallback ECG_stream_cb = EcgStreamCallback();
static EdaStreamCallback EDA_stream_cb = EdaStreamCallback();
static TemperatureStreamCallback TEMP_stream_cb = TemperatureStreamCallback();
static BcmStreamCallback BCM_stream_cb = BcmStreamCallback();
static PedometerStreamCallback PED_stream_cb = PedometerStreamCallback();
//static HrvStreamCallback HRV_stream_cb = HrvStreamCallback();
static AgcStreamCallback AGC_stream_cb = AgcStreamCallback();
static FsLsCallback FS_ls_cb = FsLsCallback();
static FsStreamCallback FS_stream_cb = FsStreamCallback();

static PingStreamCallback ping_stream_cb = PingStreamCallback();
static TXCallback g_phy_callback = TXCallback();
static watch mw = watch(g_phy_callback);

/*=========================================================================================
|									SDK TX & RX											   |
===========================================================================================*/

void  initializesdk(sdkPlatform platform)
{

	mw.initialize_sdk(static_cast<watch::platforms>(platform));
	//mw.set_platform(mw.ios);
    printf("Initialize SDK");
	std::cout << "CPP INITIALIZE SDK" << std::endl;

}

 void  dispatch_SDK(unsigned char *pBytes, int nLength)
{
	std::vector<uint8_t> vec_pkt(pBytes, pBytes + nLength);
#ifdef RXRAWPACKET
	std::cout << "Rx:" << std::endl;
	for (uint32_t i = 0; i < nLength; i++) {
		std::cout << " " << std::hex << std::setfill('0')
			<< std::setw(2) << (int)pBytes[i];
	}
	std::cout << std::endl;
#endif
	mw.dispatch(vec_pkt);
}



/*=========================================================================================
|									PM													   |
===========================================================================================*/

 void  pm_get_system_info(pm_sys_info *pSysInfo)
{
	pm_sys_info_t pm_sysinfo = { 0 };
	mw.pm_app.get_system_info(&pm_sysinfo);
	pSysInfo->hw_id = pm_sysinfo.hw_id;
	pSysInfo->batch_id = pm_sysinfo.batch_id;
	pSysInfo->bom_id = pm_sysinfo.bom_id;
	pSysInfo->device_id = pm_sysinfo.device_id;
	pSysInfo->model_number = pm_sysinfo.model_number;
	pSysInfo->version = pm_sysinfo.version;
	pSysInfo->date = pm_sysinfo.date;

	char *macstr = (char *)malloc(sizeof(char) * sizeof(pm_sysinfo.mac_addr));
	memcpy(macstr, pm_sysinfo.mac_addr.c_str(), sizeof(pm_sysinfo.mac_addr));
	pSysInfo->mac_addr = macstr;
	delete(macstr);
}

 void  pm_get_mcu_version(uint8_t *pMCU_Type)
{
	uint8_t mcuType;
	mw.pm_app.get_mcu_version(mcuType);
	*pMCU_Type = mcuType;
}

 SDK_STATUS  pm_set_date_time(sys_date_time *pTimeInfo)
{
	ret::sdk_status status;
	sys_date_time_t timeinfo = { 0 };
	timeinfo.year = pTimeInfo->year;
	timeinfo.month = pTimeInfo->month;
	timeinfo.day = pTimeInfo->day;
	timeinfo.hour = pTimeInfo->hour;
	timeinfo.minute = pTimeInfo->minute;
	timeinfo.second = pTimeInfo->second;
	timeinfo.TZ_sec = pTimeInfo->TZ_sec;
	status = mw.pm_app.set_date_time(&timeinfo);
	return static_cast<SDK_STATUS>(status);
}

 void  pm_get_date_time(sys_date_time *pTimeInfo)
{
	sys_date_time_t timeinfo = { 0 };
	mw.pm_app.get_date_time(&timeinfo);
	pTimeInfo->year = timeinfo.year;
	pTimeInfo->month = timeinfo.month;
	pTimeInfo->day = timeinfo.day;
	pTimeInfo->hour = timeinfo.hour;
	pTimeInfo->minute = timeinfo.minute;
	pTimeInfo->second = timeinfo.second;
	pTimeInfo->TZ_sec =  timeinfo.TZ_sec;
}

 void  pm_get_battery_info(pm_sys_bat_info *pSysBatInfo)
{
	pm_sys_bat_info_t sys_battery_info = { 0 };
	mw.pm_app.get_battery_info(&sys_battery_info);
	pSysBatInfo->bat_chrg_stat = sys_battery_info.bat_chrg_stat;
	pSysBatInfo->bat_lvl = sys_battery_info.bat_lvl;
	pSysBatInfo->bat_mv = sys_battery_info.bat_mv;
	pSysBatInfo->bat_temp = sys_battery_info.bat_temp;
	pSysBatInfo->timestamp = sys_battery_info.timestamp;

}


 SDK_STATUS  pm_set_battery_threshold(uint8_t nLowLvl, uint8_t nCriticalLvl)
{
	ret::sdk_status status;
	status = mw.pm_app.set_battery_threshold(nLowLvl, nCriticalLvl);
	return static_cast<SDK_STATUS>(status);
}

 SDK_STATUS  pm_set_usb_power(bool enable, bool action)
{
	ret::sdk_status status;
	status = mw.pm_app.set_usb_power(enable, action);
	return static_cast<SDK_STATUS>(status);
}
 SDK_STATUS  pm_set_power_state(uint8_t nPowerState)
{
	ret::sdk_status status;
	status = mw.pm_app.set_power_state(nPowerState);
	return static_cast<SDK_STATUS>(status);
}
 void  pm_ping(uint16_t nTimes, uint16_t pktsize, pingStreamCB pingcb)
{
	PING_cb = pingcb;
	ret::sdk_status status;
	mw.pm_app.ping(nTimes, pktsize, ping_stream_cb);
}
 SDK_STATUS  pm_enable_or_disable_battery_charger(bool enable)
{
	ret::sdk_status status;
	status = mw.pm_app.enable_or_disable_battery_charger(enable);
	return static_cast<SDK_STATUS>(status);
}
 SDK_STATUS  pm_enable_or_disable_thermistor_state_change(bool enable)
{
	ret::sdk_status status;
	status = mw.pm_app.enable_or_disable_thermistor_state_change(enable);
	return static_cast<SDK_STATUS>(status);
}

 SDK_STATUS  pm_powerboost_m3(uint8_t nEnableState, uint8_t nOVPValue)
{
	 ret::sdk_status status;
	status = mw.pm_app.powerboost_m3(nEnableState, nOVPValue);
	return static_cast<SDK_STATUS>(status);
}
 uint8_t  pm_powerboost_m4(uint8_t nEnableState)
{
	uint8_t status;
	status = mw.pm_app.powerboost_m4(nEnableState);
	return status;
}
 uint8_t  pm_boost_m4(uint8_t nEnableState, uint8_t  nBoostVoltage)
{
	uint8_t status;
	status = mw.pm_app.boost_m4(nEnableState, nBoostVoltage);
	return status;
}
 uint8_t  pm_read_EEPROM(eeprom_info *pEEPROM_Info)
{
	uint8_t status;
	eeprom_info_t eeprominfo = { 0 };
	status = mw.pm_app.read_EEPROM(&eeprominfo);
	pEEPROM_Info->manufacture_id = eeprominfo.manufacture_id;
	pEEPROM_Info->hw_id = eeprominfo.hw_id;
	pEEPROM_Info->bom_id = eeprominfo.bom_id;
	pEEPROM_Info->batch_id = eeprominfo.batch_id;
	pEEPROM_Info->date = eeprominfo.date;
	pEEPROM_Info->additional_detail = eeprominfo.additional_detail;
	return status;
}

 void  pm_subscribe_battery(battery_stream_cb battery_cb)
{
	BATTERY_cb = battery_cb;
	mw.pm_app.battery_stream.subscribe(BAt_Stream_cb);

}
 void  pm_unsubscribe_battery()
{
	BATTERY_cb = NULL;
	mw.pm_app.battery_stream.unsubscribe();

}

 uint8_t pm_get_low_touch_status()
 {
	 uint8_t status;
	 status = mw.pm_app.get_low_touch_status();
	 return status;
 }
 uint8_t pm_enable_config_log(bool enable)
 {
	 uint8_t status;
	 status = mw.pm_app.enable_config_log(enable);
	 return status;
 }

 SDK_STATUS pm_flash_reset()
 {
	 ret::sdk_status status;
	 status = mw.pm_app.flash_reset();
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS pm_system_reset()
 {
	 ret::sdk_status status;
	 status = mw.pm_app.system_reset();
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS pm_system_hardware_reset()
 {
	 ret::sdk_status status;
	 status = mw.pm_app.system_hardware_reset();
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS pm_dg502_software_control(PM_SYS_DG2502_SELECT_ENUM sw_name, bool enable)
 {
	 ret::sdk_status status;
	 status = mw.pm_app.dg502_software_control(static_cast<pm_application::PM_SYS_DG2502_SELECT_ENUM_t>(sw_name) , enable);
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS pm_ldo_control(uint8_t ldo_num, bool ldo_enable)
 {
	 ret::sdk_status status;
	 status = mw.pm_app.ldo_control(ldo_num, ldo_enable);
	 return static_cast<SDK_STATUS>(status);
 }
 uint16_t pm_get_chipID(PM_SYS_CHIP_ID_ENUM chipname)
 {
	 uint16_t chipID;
	 chipID = mw.pm_app.get_chipID(static_cast<pm_application::PM_SYS_CHIP_ID_ENUM_t>(chipname));
	 return chipID;
 }
 SDK_STATUS pm_enter_bootloader()
 {
	 ret::sdk_status status;
	 status = mw.pm_app.enter_bootloader();
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS pm_cap_sense_test(bool enable)
 {
	 ret::sdk_status status;
	 status = mw.pm_app.cap_sense_test(enable);
	 return static_cast<SDK_STATUS>(status);
 }

 SDK_STATUS pm_setMaxPktCombineCnt(uint8_t count)
 {
     ret::sdk_status status;
 	 status = mw.pm_app.setMaxPktCombineCnt(count);
 	 return static_cast<SDK_STATUS>(status);
 }


/*=========================================================================================
|									ADXL												   |
===========================================================================================*/

 void  subscribe_adxl(adxl_stream_Cb adxl_cb)
{
	ADXL_cb = adxl_cb;
	int status = mw.adxl_app.adxl_stream.subscribe(ADXL_Stream_cb);
}


 SDK_STATUS adxl_loadcfg()
 {
	 ret::sdk_status status;
	 status = mw.adxl_app.load_lcfg(static_cast<adxl_application::SENSOR_ADXL_LCFG_ID>(SENSOR_ADXL_DEVICE_ID::ADXL_LCFG_ID_362));
	 return static_cast<SDK_STATUS>(status);
 }

 SDK_STATUS adxl_selfTest()
 {
	 ret::sdk_status status;
	 status = mw.adxl_app.self_test();
	 return static_cast<SDK_STATUS>(status);
 }

/*=========================================================================================
|									ADPD												   |
===========================================================================================*/

/*
 SDK_STATUS  adpd_clockCalibration(void)
{
	ret::sdk_status status;
	status = mw.adpd_app.calibrate_clock();
	return static_cast<SDK_STATUS>(status);
}

 SDK_STATUS  adpd_loadConfig(SENSOR_ADPD_DEVICE_ID device_id)
{
	ret::sdk_status status;
	status = mw.adpd_app.load_cfg(static_cast<adpd_application::SENSOR_ADPD_DEVICE_ID>(device_id));
	return static_cast<SDK_STATUS>(status);
}


 void  adpd_darkoffset(uint32_t *pValue1, uint32_t *pValue2, uint32_t *pValue3, uint32_t *pValue4)
{
	mw.adpd_app.darkoffset(*pValue1, *pValue2, *pValue3, *pValue4);
}

 void  adpd_floatConfig(uint32_t *pValue1, uint32_t *pValue2, uint32_t *pValue3)
{
	mw.adpd_app.floatconfig(*pValue1, *pValue2, *pValue3);
}

 SDK_STATUS  adpd_getctrvalue(uint32_t *pValue1, uint32_t *pValue2, uint32_t *pValue3)
{
	 ret::sdk_status status;
	 status = mw.adpd_app.get_ctrvalue(*pValue1, *pValue2, *pValue3);
	 return static_cast<SDK_STATUS>(status);
 }

 void  adpd_set_slot(ADPD_SLOT_MODE_ENUM slot_a, ADPD_SLOT_MODE_ENUM slot_b)
{
	mw.adpd_app.set_slot(static_cast<adpd_application::ADPD_SLOT_MODE_ENUM_t>(slot_a), static_cast<adpd_application::ADPD_SLOT_MODE_ENUM_t>(slot_b));
}

 void  subscribe_adpd(adpd_stream_sum adpd_sum_cb, adpd_stream_4ch adpd_4ch_cb)
{
	ADPD_SUM_cb = adpd_sum_cb;
	ADPD_4CH_cb = adpd_4ch_cb;
	int status = mw.adpd_app.adpd_stream.subscribe(ADPD_Stream_cb);
}

*/

/*=========================================================================================
|									ADPD4xxx											   |
===========================================================================================*/

 SDK_STATUS  adpd4xxx_load_cfg(SENSOR_ADPD4000_DEVICE_ID deviceid)
{
	ret::sdk_status status;
	status = mw.adpd4000_app.load_cfg(static_cast<adpd4000_application::SENSOR_ADPD4000_DEVICE_ID>(deviceid));
	return static_cast<SDK_STATUS>(status);

}

 SDK_STATUS  adpd4xxx_set_slot(uint8_t  nSlotNum, uint8_t  nSlotEnableState, uint16_t  nSlotFormat, uint8_t  nChannelNum)
{
	ret::sdk_status status;
	status = mw.adpd4000_app.set_slot(nSlotNum, nSlotEnableState, nSlotFormat, nChannelNum);
	return static_cast<SDK_STATUS>(status);
}

 SDK_STATUS  adpd4xxx_get_slot(uint8_t nSlotNum, get_slot_info *pSlotInfo)
{
	ret::sdk_status status;
	get_slot_info_t slot_info = { 0 };
	status = mw.adpd4000_app.get_slot(nSlotNum, &slot_info);
	pSlotInfo->slot_num = slot_info.slot_num;
	pSlotInfo->channel_num = slot_info.channel_num;
	pSlotInfo->slot_enable = slot_info.slot_enable;
	pSlotInfo->slot_format = slot_info.slot_format;
	return static_cast<SDK_STATUS>(status);
}

 SDK_STATUS  adpd4xxx_set_slot_active(uint8_t nSlotNum, uint8_t nSlotActvState)
{
	ret::sdk_status status;
	status = mw.adpd4000_app.set_slot_active(nSlotNum, nSlotActvState);
	return static_cast<SDK_STATUS>(status);
}

 SDK_STATUS  adpd4xxx_get_slot_active(uint8_t nSlotNum, uint8_t *pSlotNum, uint8_t *pSlotActvState)
{
	ret::sdk_status status;
	status = mw.adpd4000_app.get_slot_active(nSlotNum, *pSlotNum, *pSlotActvState);
	return static_cast<SDK_STATUS>(status);
}

 SDK_STATUS  adpd4xxx_calibrate_clock(uint8_t nClockCalibID)
{
	ret::sdk_status status;
	status = mw.adpd4000_app.calibrate_clock(nClockCalibID);
	return static_cast<SDK_STATUS>(status);
}

 uint8_t adpd4xxx_getComMode()
 {
	 uint8_t mode;
	 mode = mw.adpd4000_app.get_com_mode();
	 return mode;
 }

 SDK_STATUS adpd4xxx_setPause(bool enable)
 {
	 ret::sdk_status status;
	 status = mw.adpd4000_app.set_pause(enable);
	 return static_cast<SDK_STATUS>(status);
 }


 void  write_agcCtrl(agcCtrl *pData)
 {
	 std::vector<uint8_t> reg_addr(pData->agcType, pData->agcType + pData->length);
	 std::vector<uint8_t> reg_val(pData->agc_ctrlValue, pData->agc_ctrlValue + pData->length);

	 std::vector<std::pair<uint8_t, uint8_t>> ret_vals;

	 for (int i = 0; i < pData->length; i++)
	 {
		 std::pair<uint8_t, uint8_t> v;
		 v.first = reg_addr[i];
		 v.second = reg_val[i];
		 ret_vals.push_back(v);
	 }

	 mw.adpd4000_app.agc_ctrl(ret_vals);
 }


 void adpd4xxx_write_ecg4kreg(SENSORS sensor, registers *pReg)
 {
	 std::vector<uint8_t> reg_addr(pReg->address, pReg->address + pReg->length);
	 std::vector<uint16_t> reg_val(pReg->value, pReg->value + pReg->length);

	 std::vector<std::pair<uint8_t, uint16_t>> ret_vals;

	 for (int i = 0; i < pReg->length; i++)
	 {
		 std::pair<uint8_t, uint16_t> v;
		 v.first = reg_addr[i];
		 v.second = reg_val[i];
		 ret_vals.push_back(v);
	 }

	 mw.adpd4000_app.write_ecg4k_lcfg(ret_vals);
 }

 void adpd4xxx_read_ecg4kreg(SENSORS sensor, unsigned short *pAddress, int nLength, registers *pReg)
 {
	 std::vector<uint8_t> reg_addr(pAddress, pAddress + nLength);
	 std::vector<std::pair<uint8_t, uint16_t>> ret_vals;

	 ret_vals = mw.adpd4000_app.read_ecg4k_lcfg(reg_addr);

	 for (int i = 0; i < ret_vals.size(); i++)
	 {
		 pReg->address[i] = ret_vals[i].first;
		 pReg->value[i] = ret_vals[i].second;
	 }

	 pReg->length = nLength;


 }

 void adpd4xxx_create_ecg4kreg(SENSORS sensor, registers *pReg)
 {
	 std::vector<uint16_t> reg_addr(pReg->address, pReg->address + pReg->length);
	 std::vector<uint16_t> reg_val(pReg->value, pReg->value + pReg->length);

	 std::vector<std::pair<uint16_t, uint16_t>> ret_vals;

	 for (int i = 0; i < pReg->length; i++)
	 {
		 std::pair<uint16_t, uint16_t> v;
		 v.first = reg_addr[i];
		 v.second = reg_val[i];
		 ret_vals.push_back(v);
	 }

	 mw.adpd4000_app.create_adpd4k_dcfg(ret_vals);
 }


 SDK_STATUS adpd4xxx_setadpd4k_fs(uint16_t odr)
 {
	 ret::sdk_status status;
	 status = mw.adpd4000_app.set_adpd4k_fs(odr);
	 return static_cast<SDK_STATUS>(status);
 }

 SDK_STATUS adpd4xxx_disable_Adpd4k_Slots(void)
 {
	 ret::sdk_status status;
	 status = mw.adpd4000_app.disable_Adpd4k_Slots();
	 return static_cast<SDK_STATUS>(status);
 }

 SDK_STATUS adpd4xxx_agc_info(uint8_t led_index, get_agc_info *agc_info)
 {
	 ret::sdk_status status;
	 get_agc_info_t l_agc_info = { 0 };
	 status = mw.adpd4000_app.agc_info(led_index, &l_agc_info);
	 agc_info->DC0_LEDcurrent = l_agc_info.DC0_LEDcurrent;
	 agc_info->led_index = l_agc_info.led_index;
	 agc_info->TIA_ch1_i = l_agc_info.TIA_ch1_i;
	 agc_info->TIA_ch2_i = l_agc_info.TIA_ch2_i;
	 for (int i = 0; i < 10; i++)
	 {
		 agc_info->led_ch1[i] = l_agc_info.led_ch1[i];
		 agc_info->led_ch2[i] = l_agc_info.led_ch2[i];

	 }

	 return static_cast<SDK_STATUS>(status);
 }



 void  subscribe_adpd4xxx(ADPD4XXX_SOURCE src) 
{	
	ret::sdk_status status;
	switch (src)
	{
	case ADPD4XXX_SOURCE::STREAM1:
		mw.adpd4000_app.adpd4000_stream1.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM2:
		mw.adpd4000_app.adpd4000_stream2.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM3:
		mw.adpd4000_app.adpd4000_stream3.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM4:
		mw.adpd4000_app.adpd4000_stream4.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM5:
		mw.adpd4000_app.adpd4000_stream5.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM6:
		mw.adpd4000_app.adpd4000_stream6.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM7:
		mw.adpd4000_app.adpd4000_stream7.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM8:
		mw.adpd4000_app.adpd4000_stream8.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM9:
		mw.adpd4000_app.adpd4000_stream9.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM10:
		mw.adpd4000_app.adpd4000_stream10.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM11:
		mw.adpd4000_app.adpd4000_stream11.subscribe(ADPD4xxx_stream_cb);
		break;
	case ADPD4XXX_SOURCE::STREAM12:
		mw.adpd4000_app.adpd4000_stream12.subscribe(ADPD4xxx_stream_cb);
		break;
	default:
		status = ret::sdk_status::SDK_ERR_INVALID_VALUE;
		std::cout << "Unknown ADPD4XXX source" << std::endl;
		break;
	}
}

 void  unsubscribe_adpd4xxx(ADPD4XXX_SOURCE src)
{
	ret::sdk_status status;
	switch (src)
	{
	case ADPD4XXX_SOURCE::STREAM1:
		mw.adpd4000_app.adpd4000_stream1.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM2:
		mw.adpd4000_app.adpd4000_stream2.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM3:
		mw.adpd4000_app.adpd4000_stream3.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM4:
		mw.adpd4000_app.adpd4000_stream4.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM5:
		mw.adpd4000_app.adpd4000_stream5.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM6:
		mw.adpd4000_app.adpd4000_stream6.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM7:
		mw.adpd4000_app.adpd4000_stream7.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM8:
		mw.adpd4000_app.adpd4000_stream8.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM9:
		mw.adpd4000_app.adpd4000_stream9.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM10:
		mw.adpd4000_app.adpd4000_stream10.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM11:
		mw.adpd4000_app.adpd4000_stream11.unsubscribe();
		break;
	case ADPD4XXX_SOURCE::STREAM12:
		mw.adpd4000_app.adpd4000_stream12.unsubscribe();
		break;
	default:
		status = ret::sdk_status::SDK_ERR_INVALID_VALUE;
		std::cout << "Unknown ADPD4XXX source" << std::endl;
		break;
	}
}



 void  subscribe_adpd4xxx_interrupt(adpd4xxx_interrupt_stream_cb adpd4xxx_cb)
{
	ADPD4XXX_INTERRUPT_cb = adpd4xxx_cb;
	mw.adpd4000_app.adpd4000_optional_stream.subscribe(ADPD4xxx_stream_cb);
}
 void  unsubscribe_adpd4xxx_interrupt()
{
	ADPD4XXX_INTERRUPT_cb = NULL;
	mw.adpd4000_app.adpd4000_optional_stream.unsubscribe();
}


/*=========================================================================================
|									PPG													   |
===========================================================================================*/

 SDK_STATUS  ppg_load_lcfg(SENSOR_PPG_LCFG_ID lcfg_id)
{
	ret::sdk_status status;
	status = mw.ppg_app.load_lcfg(static_cast<ppg_application::SENSOR_PPG_LCFG_ID>(lcfg_id));
	return static_cast<SDK_STATUS>(status);
}
 void  subscribe_ppg(ppg_stream_cb ppg_cb)
{
	//ret::sdk_status status;
	PPG_cb = ppg_cb;
	mw.ppg_app.ppg_stream.subscribe(PPG_stream_cb);

	//return static_cast<SDK_STATUS>(status);
}

 void  subscribe_syncppg(syncppg_stream_cb syncppg_cb)
{
	SYNCPPG_cb = syncppg_cb;
	mw.syncppg_app.syncppg_stream.subscribe(SYNCPPG_stream_cb);
}


 void ppg_getLastState(uint8_t *statesarr, uint8_t *length)
 {
	 std::vector<uint8_t> states;
	 states = mw.ppg_app.get_last_state();

	 for (int i = 0; i < states.size(); i++)
	 {
		 statesarr[i] = states[i];
	 }
	 *length = states.size();
 }

/*=========================================================================================
|									EDA													   |
===========================================================================================*/

 void  subscribe_eda(eda_stream_cb eda_cb)
 {
	 EDA_cb = eda_cb;
	 mw.eda_app.eda_stream.subscribe(EDA_stream_cb);
 }


 SDK_STATUS eda_dcb_set_lcfg()
 {
	 ret::sdk_status status;
	 status = mw.eda_app.dcb_set_lcfg();
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS eda_dynamic_scaling(bool isEnable, uint16_t minScale, uint16_t maxScale, uint16_t lprtiasel)
 {
	 ret::sdk_status status;
	 status = mw.eda_app.dynamic_scaling(isEnable, minScale, maxScale, lprtiasel);
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS eda_set_dft_num(uint8_t dftnum)
 {
	 ret::sdk_status status;
	 status = mw.eda_app.set_dft_num(dftnum);
	 return static_cast<SDK_STATUS>(status);
 }


/*=========================================================================================
|									ECG													   |
===========================================================================================*/
 void  subscribe_ecg(ecg_stream_cb ecg_cb)
 {
	 ECG_cb = ecg_cb;
	 mw.ecg_app.ecg_stream.subscribe(ECG_stream_cb);
 }

 SDK_STATUS ecg_dcb_set_lcfg(void)
 {
	 ret::sdk_status status;
	 status = mw.ecg_app.dcb_set_lcfg();
	 return static_cast<SDK_STATUS>(status);
 }


 /*=========================================================================================
 |									TEMPERATURE													   |
 ===========================================================================================*/
 void subscribe_temperature(temperature_stream_cb temp_cb)
 {
	 TEMP_cb = temp_cb;
	 mw.temperature_app.temperature_stream.subscribe(TEMP_stream_cb);	 
 }


 /*=========================================================================================
 |										BCM												   |
 ===========================================================================================*/
 void subscribe_bcm(bcm_stream_cb bcm_cb)
 {
	 BCM_cb = bcm_cb;
	 mw.bcm_app.bcm_stream.subscribe(BCM_stream_cb);
 }

 SDK_STATUS bcm_set_dft_num(uint32_t dft_num)
 {
	 ret::sdk_status status;
	 status = mw.bcm_app.set_dft_num(dft_num);
	 return static_cast<SDK_STATUS>(status);
 }
 
 SDK_STATUS bcm_enable_or_disable_sweep_frequency(bool enable)
 {
	 ret::sdk_status status;
	 status = mw.bcm_app.enable_or_disable_sweep_frequency(enable);
	 return static_cast<SDK_STATUS>(status);
 }
 
 SDK_STATUS bcm_set_hsrtia_cal(uint16_t value)
 {
	 ret::sdk_status status;
	 status = mw.bcm_app.set_hsrtia_cal(value);
	 return static_cast<SDK_STATUS>(status);
 }

 /*=========================================================================================
 |									Pedometer											   |
 ===========================================================================================*/
 void subscribe_pedometer(pedometer_stream_cb ped_cb)
 {
	 PED_cb = ped_cb;
	 mw.ped_app.pedometer_stream.subscribe(PED_stream_cb);
 }


 /*=========================================================================================
 |									 HRV												  |
 ===========================================================================================*/
/*
 void subscribe_hrv(hrv_stream_cb hrv_cb)
 {
	 HRV_cb = hrv_cb;
	 mw.ppg_app.hrv_stream.subscribe(HRV_stream_cb);
 }
*/

 /*=========================================================================================
 |									AGC													   |
 ===========================================================================================*/
/*
 void subscribe_agc(agc_stream_cb agc_cb)
 {
	 AGC_cb = agc_cb;
	 mw.ppg_app.agc_stream.subscribe(AGC_stream_cb);
 }
*/
 /*=========================================================================================
 |									Display												   |
 ===========================================================================================*/
 SDK_STATUS display_set_colour(DISPLAY_SET_COMMAND_ENUM display_color)
 {
	 ret::sdk_status status;
	 status = mw.display_app.set_display_colour(static_cast<display_application::DISPLAY_SET_COMMAND_ENUM_t>(display_color));
	 return static_cast<SDK_STATUS>(status);
 }

 SDK_STATUS display_backlight_control(bool enable)
 {
	 ret::sdk_status status;
	 status = mw.display_app.backlight_control(enable);
	 return static_cast<SDK_STATUS>(status);
 }

 SDK_STATUS display_key_press_test(bool enable)
 {
	 ret::sdk_status status;
	 status = mw.display_app.key_press_test(enable);
	 return static_cast<SDK_STATUS>(status);
 }

 /*=========================================================================================
	|									FS													   |
	===========================================================================================*/

 SDK_STATUS fileSystem_format(void)
 {
	 ret::sdk_status status;
	 status = mw.fs_app.fs_format();
	 return static_cast<SDK_STATUS>(status);
 }
 void fileSystem_volume_info(uint32_t* total_Memory,
	 uint32_t* used_Memory,
	 uint16_t* free_Memory)
 {
	 uint32_t fs_totalmemory, fs_usedmemory;
	 uint16_t fs_freememory;

	 mw.fs_app.fs_volume_info(fs_totalmemory, fs_usedmemory, fs_freememory);
	 *total_Memory = fs_totalmemory;
	 *used_Memory = fs_usedmemory;
	 *free_Memory = fs_freememory;
 }

 void fileSystem_debug_info(ADDRS_ENUM_STREAM stream,
	 uint32_t& packets_received,
	 uint32_t& packets_missed)
 {
	 uint32_t fs_packtreceived, fs_packetmissed;
	 mw.fs_app.fs_debug_info(static_cast<fs_application::ADDR_ENUM_STREAM>(stream), fs_packtreceived, fs_packetmissed);
	 packets_received = fs_packtreceived;
	 packets_missed = fs_packetmissed;
 }
 uint8_t fileSystem_stream_subscription_status(ADDRS_ENUM_STREAM stream)
 {
	 uint8_t status;
	 status = mw.fs_app.fs_stream_subscription_status(static_cast<fs_application::ADDR_ENUM_STREAM>(stream));
	 return status;
 }


 SDK_STATUS fileSystem_keyvaluepair(char* value_id)
 {
	 std::string fs_ValueID = std::string(value_id);
	 ret::sdk_status status;
	 status = mw.fs_app.fs_keyvaluepair(fs_ValueID);
	 return static_cast<SDK_STATUS>(status);
 }


 uint8_t fileSystem_status(void)
 {
	 uint8_t status;
	 status = mw.fs_app.fs_status();
	 return status;
 }
 SDK_STATUS fileSystem_subscribe(ADDRS_ENUM_STREAM stream)
 {
	 ret::sdk_status status;
	 status = mw.fs_app.fs_subscribe(static_cast<fs_application::ADDR_ENUM_STREAM>(stream));
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS fileSystem_unsubscribe(ADDRS_ENUM_STREAM stream)
 {
	 ret::sdk_status status;
	 status = mw.fs_app.fs_unsubscribe(static_cast<fs_application::ADDR_ENUM_STREAM>(stream));
	 return static_cast<SDK_STATUS>(status);
 }
 void fileSystem_get_file_count(uint16_t& maxFileCount)
 {
	 uint16_t fs_fileCount;
	 mw.fs_app.fs_get_file_count(fs_fileCount);
	 maxFileCount = fs_fileCount;
 }

 SDK_STATUS fileSystem_find_config_file(void)
 {
	 ret::sdk_status status;
	 status = mw.fs_app.fs_find_config_file();
	 return static_cast<SDK_STATUS>(status);
 }

 uint8_t fileSystem_delete_config_file(void)
 {
	 uint8_t status;
	 status = mw.fs_app.delete_config_file();
	 return status;
 }
 uint8_t fileSystem_config_log(bool enable)
 {
	 uint8_t status;
	 status = mw.fs_app.fs_config_log(enable);
	 return status;
 }

 SDK_STATUS fileSystem_logStart(void)
 {
	 ret::sdk_status status;
	 status = mw.fs_app.fs_logStart();
	 return static_cast<SDK_STATUS>(status);
 }
 SDK_STATUS fileSystem_logStop(void)
 {
	 ret::sdk_status status;
	 status = mw.fs_app.fs_logStop();
	 return static_cast<SDK_STATUS>(status);
 }
 uint8_t fileSystem_mount(void)
 {
	 uint8_t status;
	 status = mw.fs_app.fs_mount();
	 return status;
 }

 void fileSystem_refhr(uint16_t refhr, sys_date_time *ptime_info)
 {
	 sys_date_time_t timeinfo = { 0 };
	 timeinfo.year = ptime_info->year;
	 timeinfo.month = ptime_info->month;
	 timeinfo.day = ptime_info->day;
	 timeinfo.hour = ptime_info->hour;
	 timeinfo.minute = ptime_info->minute;
	 timeinfo.second = ptime_info->second;
	 timeinfo.TZ_sec = ptime_info->TZ_sec;
	 mw.fs_app.fs_refhr(refhr, &timeinfo);
 }

 uint32_t fileSystem_get_number_of_bad_blocks(void)
 {
	 uint32_t status;
	 status = mw.fs_app.fs_get_number_of_bad_blocks();
	 return status;
 }
 uint8_t fileSystem_abort(void)
 {
	 uint8_t status;
	 status = mw.fs_app.fs_abort();
	 return status;
 }

 void fileSystem_ls(fs_ls_cb fs_ls_cb)
 {
	 FS_LS_cb = fs_ls_cb;
	 mw.fs_app.fs_ls(FS_ls_cb);
 }

 void fileSystem_get(char *filename,
	 fs_stream_cb fs_stream_cb)
 {
	 std::string fs_filename = std::string(filename);
	 FS_STREAM_cb = fs_stream_cb;
	 mw.fs_app.fs_get(fs_filename, FS_stream_cb);
 }

 void fileSystem_get_stream_chunk(uint8_t roll_over, uint16_t chunk_num, char *filename, fs_stream_cb fs_stream_cb)
 {
	 std::string fs_filename = std::string(filename);
	 FS_STREAM_cb = fs_stream_cb;
	 mw.fs_app.fs_get_stream_chunk(roll_over, chunk_num, fs_filename, FS_stream_cb);
 }


/*=========================================================================================
|							Common APPlication(ADPD/ADXL/ADPD4000/PPG)					   |
===========================================================================================*/


 void  get_version(SENSORS sensor, common_app_version *pVersion)
{
	common_app_version_t app_version = { 0 };

	switch (sensor)
	{
	case SENSORS::ADXL:
		mw.adxl_app.get_version(&app_version);
		break;
//	case SENSORS::ADPD:
//		mw.adpd_app.get_version(&app_version);
//		break;
	case SENSORS::ADPD4000:
		mw.adpd4000_app.get_version(&app_version);
		break;
	case SENSORS::PM:
		mw.pm_app.get_version(&app_version);
		break;
	case SENSORS::PPG:
		mw.ppg_app.get_version(&app_version);
		break;
	case SENSORS::SYNCPPG:
		mw.syncppg_app.get_version(&app_version);
		break;
	case SENSORS::ECG:
		mw.ecg_app.get_version(&app_version);
		break;
	case SENSORS::EDA:
		mw.eda_app.get_version(&app_version);
		break;
	case SENSORS::BCM:
		mw.bcm_app.get_version(&app_version);
		break;
	case SENSORS::PED:
		mw.ped_app.get_version(&app_version);
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}

	pVersion->major = app_version.major;
	pVersion->minor = app_version.minor;
	pVersion->patch = app_version.patch;

	
 	int nVersize = sizeof(app_version.verstr); 
   	int nVendstrsize = sizeof(app_version.vendstr); 

  /* Stored in heap segment like other dynamically allocated things */
  	char *verstr = (char *)malloc(sizeof(char)*nVersize); 

    char *vendstr = (char *)malloc(sizeof(char)*nVendstrsize); 


	memcpy(verstr, app_version.verstr.c_str(), nVersize);
	memcpy(vendstr, app_version.vendstr.c_str(), nVendstrsize);

	pVersion->vendstr = vendstr;
	pVersion->verstr = verstr;

	delete(verstr);
	delete(vendstr);

	// memcpy(pVersion->vendstr, app_version.vendstr.c_str(), sizeof(pVersion->vendstr));
	// memcpy(pVersion->verstr, app_version.verstr.c_str(), sizeof(pVersion->verstr));
/*	strcpy_s(pVersion->vendstr, strlen(pVersion->vendstr), app_version.vendstr.c_str());
	strcpy_s(pVersion->verstr, strlen(pVersion->verstr), app_version.verstr.c_str())*/;

}

 void  read_registers(SENSORS sensor, unsigned short *pAddress, int nLength, registers *pReg)
{
	std::vector<uint16_t> reg_addr(pAddress, pAddress + nLength);
	std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
	switch (sensor)
	{
	case SENSORS::ADXL:
		ret_vals = mw.adxl_app.register_read(reg_addr);
		break;
//	case SENSORS::ADPD:
//		ret_vals = mw.adpd_app.register_read(reg_addr);
//		break;
	case SENSORS::ADPD4000:
		ret_vals = mw.adpd4000_app.register_read(reg_addr);
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}

	for (int i = 0; i < ret_vals.size(); i++)
	{
		pReg->address[i] = ret_vals[i].first;
		pReg->value[i] = ret_vals[i].second;
	}

	pReg->length = nLength;
}

 void  get_dcfg(SENSORS sensor, registers *pReg)
{
	std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
	switch (sensor)
	{
	case SENSORS::ADXL:
		ret_vals = mw.adxl_app.get_dcfg();
		break;
//	case SENSORS::ADPD:
//		ret_vals = mw.adpd_app.get_dcfg();
//		break;
	case SENSORS::ADPD4000:
		ret_vals = mw.adpd4000_app.get_dcfg();
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}

	for (int i = 0; i < ret_vals.size(); i++)
	{
		pReg->address[i] = ret_vals[i].first;
		pReg->value[i] = ret_vals[i].second;
	}
	pReg->length = ret_vals.size();
}

 void  write_register(SENSORS sensor, registers *pReg)
{
	std::vector<uint16_t> reg_addr(pReg->address, pReg->address + pReg->length);
	std::vector<uint16_t> reg_val(pReg->value, pReg->value + pReg->length);

	std::vector<std::pair<uint16_t, uint16_t>> ret_vals;

	for (int i = 0; i < pReg->length; i++)
	{
		std::pair<uint16_t, uint16_t> v;
		v.first = reg_addr[i];
		v.second = reg_val[i];
		ret_vals.push_back(v);
	}

	switch (sensor)
	{
	case SENSORS::ADXL:
		mw.adxl_app.register_write(ret_vals);
		break;
//	case SENSORS::ADPD:
//		mw.adpd_app.register_write(ret_vals);
//		break;
	case SENSORS::ADPD4000:
		mw.adpd4000_app.register_write(ret_vals);
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
}

 void start_stream(SENSORS sensor)
{
	switch (sensor)
	{
	case SENSORS::ADXL:
		mw.adxl_app.adxl_stream.start();
		break;
//	case SENSORS::ADPD:
//		mw.adpd_app.adpd_stream.start();
//		break;
	case SENSORS::ADPD4000:
		mw.adpd4000_app.adpd4000_stream1.start();
		break;
	case SENSORS::AGC:
//	case SENSORS::HRV:
	case SENSORS::PPG:
		mw.ppg_app.ppg_stream.start();
		break;
	case SENSORS::ECG:
		mw.ecg_app.ecg_stream.start();
		break;
	case SENSORS::EDA:
		mw.eda_app.eda_stream.start();
		break;
//	case SENSORS::AD5940:
//		mw.ad5940_app.start();
//		break;
	case SENSORS::TEMPERATURE:
		mw.temperature_app.temperature_stream.start();
		break;
	case SENSORS::BCM:
		mw.bcm_app.bcm_stream.start();
		break;
	case SENSORS::PED:
		mw.ped_app.pedometer_stream.start();
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
}

 void stop_stream(SENSORS sensor)
{
	switch (sensor)
	{
	case SENSORS::ADXL:
		mw.adxl_app.adxl_stream.stop();
		break;
//	case SENSORS::ADPD:
//		mw.adpd_app.adpd_stream.stop();
//		break;
	case SENSORS::ADPD4000:
		mw.adpd4000_app.adpd4000_stream1.stop();
		ADPD4XXX_cb = NULL;
		break;
	case SENSORS::AGC:
//	case SENSORS::HRV:
	case SENSORS::PPG:
		mw.ppg_app.ppg_stream.stop();
		break;
	case SENSORS::ECG:
		mw.ecg_app.ecg_stream.stop();
		break;
	case SENSORS::EDA:
		mw.eda_app.eda_stream.stop();
		break;
//	case SENSORS::AD5940:
//		mw.ad5940_app.stop();
//		break;
	case SENSORS::TEMPERATURE:
		mw.temperature_app.temperature_stream.stop();
		break;
	case SENSORS::BCM:
		mw.bcm_app.bcm_stream.stop();
		break;
	case SENSORS::PED:
		mw.ped_app.pedometer_stream.stop();
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
}

void subscribe_stream(SENSORS sensor)
{
	int status = 0;
	switch (sensor)
	{
	case SENSORS::ADXL:		
		status = mw.adxl_app.adxl_stream.subscribe(ADXL_Stream_cb);
		break;
//	case SENSORS::ADPD:
//		ADPD_SUM_cb = NULL;
//		ADPD_4CH_cb = NULL;
//		status = mw.adpd_app.adpd_stream.unsubscribe();
//		break;
	case SENSORS::PPG:
		PPG_cb = NULL;		
		mw.syncppg_app.syncppg_stream.set_callback(SYNCPPG_stream_cb);
		status = mw.ppg_app.ppg_stream.subscribe(PPG_stream_cb);
		break;
	case SENSORS::SYNCPPG:
		SYNCPPG_cb = NULL;
		status = mw.syncppg_app.syncppg_stream.subscribe(SYNCPPG_stream_cb);
		break;
	case SENSORS::PM:
		BATTERY_cb = NULL;
		status = mw.pm_app.battery_stream.subscribe(BAt_Stream_cb);
		break;
	case SENSORS::ECG:
		ECG_cb = NULL;
		status = mw.ecg_app.ecg_stream.subscribe(ECG_stream_cb);
		break;
	case SENSORS::EDA:
		EDA_cb = NULL;
		status = mw.eda_app.eda_stream.subscribe(EDA_stream_cb);
		break;
	case SENSORS::TEMPERATURE:
		TEMP_cb = NULL;
		mw.temperature_app.temperature_stream.subscribe(TEMP_stream_cb);
		break;
	case SENSORS::BCM:
		BCM_cb = NULL;
		mw.bcm_app.bcm_stream.subscribe(BCM_stream_cb);
		break;
	case SENSORS::PED:
		PED_cb = NULL;
		mw.ped_app.pedometer_stream.subscribe(PED_stream_cb);
		break;
//	case SENSORS::HRV:
//		HRV_cb = NULL;
//		mw.ppg_app.hrv_stream.unsubscribe();
//		break;
	case SENSORS::AGC:
		AGC_cb = NULL;
		mw.ppg_app.agc_stream.subscribe(AGC_stream_cb);
		break;
	default:
		status = ret::sdk_status::SDK_ERR_INVALID_VALUE;
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
}

 void unsubscribe_stream(SENSORS sensor)
{
	int status = 0;
	switch (sensor)
	{
	case SENSORS::ADXL:
		ADXL_cb = NULL;
		status = mw.adxl_app.adxl_stream.unsubscribe();
		break;
//	case SENSORS::ADPD:
//		ADPD_SUM_cb = NULL;
//		ADPD_4CH_cb = NULL;
//		status = mw.adpd_app.adpd_stream.unsubscribe();
//		break;
	case SENSORS::PPG:
		PPG_cb = NULL;
		status = mw.ppg_app.ppg_stream.unsubscribe();
		break;
	case SENSORS::SYNCPPG:
		SYNCPPG_cb = NULL;
		status = mw.syncppg_app.syncppg_stream.unsubscribe();
		break;
	case SENSORS::PM:
		BATTERY_cb = NULL;
		status = mw.pm_app.battery_stream.unsubscribe();
		break;
	case SENSORS::ECG:
		ECG_cb = NULL;
		status = mw.ecg_app.ecg_stream.unsubscribe();
		break;
	case SENSORS::EDA:
		EDA_cb = NULL;
		status = mw.eda_app.eda_stream.unsubscribe();
		break;
	case SENSORS::TEMPERATURE:
		TEMP_cb = NULL;
		mw.temperature_app.temperature_stream.unsubscribe();
		break;
	case SENSORS::BCM:
		BCM_cb = NULL;
		mw.bcm_app.bcm_stream.unsubscribe();
		break;
	case SENSORS::PED:
		PED_cb = NULL;
		mw.ped_app.pedometer_stream.unsubscribe();
		break;
//	case SENSORS::HRV:
//		HRV_cb = NULL;
//		mw.ppg_app.hrv_stream.unsubscribe();
//		break;
	case SENSORS::AGC:
		AGC_cb = NULL;
		mw.ppg_app.agc_stream.unsubscribe();
		break;
	default:
		status = ret::sdk_status::SDK_ERR_INVALID_VALUE;
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
}

void  setTimeout(SENSORS sensor, uint16_t ntime)
{
	switch (sensor)
	{
	case SENSORS::ADXL:
		mw.adxl_app.setTimeout(ntime);
		break;
//	case SENSORS::ADPD:
//		mw.adpd_app.setTimeout(ntime);
//		break;
	case SENSORS::PPG:
		mw.ppg_app.setTimeout(ntime);
		break;
	case SENSORS::SYNCPPG:
		mw.syncppg_app.setTimeout(ntime);
		break;
	case SENSORS::PM:
		mw.pm_app.setTimeout(ntime);
		break;
	case SENSORS::ADPD4000:
		mw.adpd4000_app.setTimeout(ntime);
		break;
	case SENSORS::ECG:
		mw.ecg_app.setTimeout(ntime);
		break;
	case SENSORS::EDA:
		mw.eda_app.setTimeout(ntime);
		break;
	case SENSORS::TEMPERATURE:
		mw.temperature_app.setTimeout(ntime);
		break;
	case SENSORS::BCM:
		mw.bcm_app.setTimeout(ntime);
		break;
	case SENSORS::PED:
		mw.ped_app.setTimeout(ntime);
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
}

uint16_t  getTimeout(SENSORS sensor)
{
	uint16_t time_intrvl = 0;
	switch (sensor)
	{
	case SENSORS::ADXL:
		time_intrvl = mw.adxl_app.GetTimeout();
		break;
//	case SENSORS::ADPD:
//		time_intrvl = mw.adpd_app.GetTimeout();
//		break;
	case SENSORS::PPG:
		time_intrvl = mw.ppg_app.GetTimeout();
		break;
	case SENSORS::SYNCPPG:
		time_intrvl = mw.syncppg_app.GetTimeout();
		break;
	case SENSORS::PM:
		time_intrvl = mw.pm_app.GetTimeout();
		break;
	case SENSORS::ADPD4000:
		time_intrvl = mw.adpd4000_app.GetTimeout();
		break;
	case SENSORS::ECG:
		time_intrvl = mw.ecg_app.GetTimeout();
		break;
	case SENSORS::EDA:
		time_intrvl = mw.eda_app.GetTimeout();
		break;
	case SENSORS::TEMPERATURE:
		time_intrvl = mw.temperature_app.GetTimeout();
		break;
	case SENSORS::BCM:
		time_intrvl = mw.bcm_app.GetTimeout();
		break;
	case SENSORS::PED:
		time_intrvl = mw.ped_app.GetTimeout();
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
	return time_intrvl;
}

void  stream_status(SENSORS sensor, uint8_t *nSubreq, uint8_t *nStartreq)
{
	uint8_t nStatus, nNumsubscribers, nStartReq;
	switch (sensor)
	{
	case SENSORS::ADXL:
		mw.adxl_app.adxl_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
//	case SENSORS::ADPD:
//		mw.adpd_app.adpd_stream.get_status(nStatus, nNumsubscribers, nStartReq);
//		break;
	case SENSORS::PPG:
		mw.ppg_app.ppg_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::SYNCPPG:
		mw.syncppg_app.syncppg_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::PM:
		mw.pm_app.battery_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::ADPD4000:
		mw.adpd4000_app.adpd4000_stream1.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::ECG:
		mw.ecg_app.ecg_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::EDA:
		mw.eda_app.eda_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::TEMPERATURE:
		mw.temperature_app.temperature_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::BCM:
		mw.bcm_app.bcm_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	case SENSORS::PED:
		mw.ped_app.pedometer_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
//	case SENSORS::HRV:
//		mw.ppg_app.hrv_stream.get_status(nStatus, nNumsubscribers, nStartReq);
//		break;
	case SENSORS::AGC:
		mw.ppg_app.agc_stream.get_status(nStatus, nNumsubscribers, nStartReq);
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
	*nSubreq = nNumsubscribers;
	*nStartreq = nStartReq;
}

 void  get_algo_vendor_version(SENSORS sensor, common_app_version *pVersion)
{
	common_app_version_t app_version = { 0 };
	switch (sensor)
	{
	case SENSORS::PPG:
		mw.ppg_app.get_algo_vendor_version(&app_version);
		break;
	case SENSORS::ECG:
		mw.ecg_app.get_algo_vendor_version(&app_version);
		break;
	case SENSORS::PED:
		mw.ped_app.get_algo_vendor_version(&app_version);
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
	pVersion->major = app_version.major;
	pVersion->minor = app_version.minor;
	pVersion->patch = app_version.patch;

 	int nVersize = sizeof(app_version.verstr); 
   	int nVendstrsize = sizeof(app_version.vendstr); 

  /* Stored in heap segment like other dynamically allocated things */
  	char *verstr = (char *)malloc(sizeof(char)*nVersize); 

    char *vendstr = (char *)malloc(sizeof(char)*nVendstrsize); 


	memcpy(verstr, app_version.verstr.c_str(), nVersize);
	memcpy(vendstr, app_version.vendstr.c_str(), nVendstrsize);

	pVersion->vendstr = vendstr;
	pVersion->verstr = verstr;

	delete(verstr);
	delete(vendstr);
}

void getPPGAlgo(app_version *pVersion)
{
	common_app_version_t app_version = { 0 };
	mw.ppg_app.get_algo_vendor_version(&app_version);

	 int size = 20; 
  
  /* Stored in heap segment like other dynamically allocated things */
  char *str = (char *)malloc(sizeof(char)*size); 

	memcpy(str, app_version.verstr.c_str(), size);



	// memcpy(pVersion->vendstr, app_version.vendstr.c_str(), sizeof(pVersion->vendstr));
	pVersion->verstr = str;
}

 void  get_lcfg(SENSORS sensor, registers *pReg)
{
	std::vector<uint32_t> ret_vals;
	switch (sensor)
	{
	case SENSORS::PPG:
		ret_vals = mw.ppg_app.get_lcfg();
		break;
	case SENSORS::ECG:
		ret_vals = mw.ecg_app.get_lcfg();
		break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}

	for (int i = 0; i < ret_vals.size(); i++)
	{
		pReg->value[i] = ret_vals[i];
	}
	pReg->length = ret_vals.size();

}

 void  write_lcfgreg(SENSORS sensor, registers *pReg)
{
	switch (sensor)
	{
	case SENSORS::PPG:
	{
		std::vector<uint8_t> reg_addr(pReg->address, pReg->address + pReg->length);
		std::vector<uint32_t> reg_val(pReg->value, pReg->value + pReg->length);

		std::vector<std::pair<uint8_t, uint32_t>> ret_vals;

		for (int i = 0; i < pReg->length; i++)
		{
			std::pair<uint8_t, uint32_t> v;
			v.first = reg_addr[i];
			v.second = reg_val[i];
			ret_vals.push_back(v);
		}

		mw.ppg_app.lcfg_write(ret_vals);
	}
	break;
	case SENSORS::ECG:
	{
		std::vector<uint8_t> reg_addr(pReg->address, pReg->address + pReg->length);
		std::vector<uint16_t> reg_val(pReg->value, pReg->value + pReg->length);

		std::vector<std::pair<uint8_t, uint16_t>> ret_vals;

		for (int i = 0; i < pReg->length; i++)
		{
			std::pair<uint8_t, uint16_t> v;
			v.first = reg_addr[i];
			v.second = reg_val[i];
			ret_vals.push_back(v);
		}

		mw.ecg_app.lcfg_write(ret_vals);
	}
	break;
	case SENSORS::EDA:
	{
		std::vector<uint8_t> reg_addr(pReg->address, pReg->address + pReg->length);
		std::vector<uint32_t> reg_val(pReg->value, pReg->value + pReg->length);

		std::vector<std::pair<uint8_t, uint32_t>> ret_vals;

		for (int i = 0; i < pReg->length; i++)
		{
			std::pair<uint8_t, uint32_t> v;
			v.first = reg_addr[i];
			v.second = reg_val[i];
			ret_vals.push_back(v);
		}

		mw.eda_app.lcfg_write(ret_vals);
	}
	break;
	case SENSORS::BCM:
	{
		std::vector<uint8_t> reg_addr(pReg->address, pReg->address + pReg->length);
		std::vector<uint32_t> reg_val(pReg->value, pReg->value + pReg->length);

		std::vector<std::pair<uint8_t, uint32_t>> ret_vals;

		for (int i = 0; i < pReg->length; i++)
		{
			std::pair<uint8_t, uint32_t> v;
			v.first = reg_addr[i];
			v.second = reg_val[i];
			ret_vals.push_back(v);
		}

		mw.bcm_app.lcfg_write(ret_vals);
	}
	break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
}

 void  read_lfcgreg(SENSORS sensor, unsigned short *pAddress, int nLength, registers *pReg)
{

	switch (sensor)
	{
	case SENSORS::PPG:
	{
		std::vector<uint8_t> reg_addr(pAddress, pAddress + nLength);
		std::vector<std::pair<uint8_t, uint32_t>> ret_vals;
		ret_vals = mw.ppg_app.lcfg_read(reg_addr);

		for (int i = 0; i < ret_vals.size(); i++)
		{
			pReg->address[i] = ret_vals[i].first;
			pReg->value[i] = ret_vals[i].second;
		}

		pReg->length = nLength;
	}
	break;
	case SENSORS::ECG:
	{
		std::vector<uint8_t> reg_addr(pAddress, pAddress + nLength);
		std::vector<std::pair<uint8_t, uint16_t>> ret_vals;
		ret_vals = mw.ecg_app.lcfg_read(reg_addr);

		for (int i = 0; i < ret_vals.size(); i++)
		{
			pReg->address[i] = ret_vals[i].first;
			pReg->value[i] = ret_vals[i].second;
		}

		pReg->length = nLength;
	}
	break;
	case SENSORS::EDA:
	{
		std::vector<uint8_t> reg_addr(pAddress, pAddress + nLength);
		std::vector<std::pair<uint8_t, uint32_t>> ret_vals;
		ret_vals = mw.eda_app.lcfg_read(reg_addr);

		for (int i = 0; i < ret_vals.size(); i++)
		{
			pReg->address[i] = ret_vals[i].first;
			pReg->value[i] = ret_vals[i].second;
		}

		pReg->length = nLength;
	}
	break;
	case SENSORS::BCM:
	{
		std::vector<uint8_t> reg_addr(pAddress, pAddress + nLength);
		std::vector<std::pair<uint8_t, uint32_t>> ret_vals;
		ret_vals = mw.bcm_app.lcfg_read(reg_addr);

		for (int i = 0; i < ret_vals.size(); i++)
		{
			pReg->address[i] = ret_vals[i].first;
			pReg->value[i] = ret_vals[i].second;
		}

		pReg->length = nLength;
	}
	break;
	default:
		std::cout << "Unknown sensor detected" << std::endl;
		break;
	}
	
}

 SDK_STATUS dcb_deleteConfig(SENSORS sensor)
 {
	 ret::sdk_status status;	
	 switch (sensor)
	 {
	 case SENSORS::ADXL:
		 status = mw.adxl_app.dcb_delete_config();
		 break;
	 case SENSORS::ADPD4000:
		 status = mw.adpd4000_app.dcb_delete_config();
		 break;
	 case SENSORS::ECG:
		 status = mw.ecg_app.dcb_delete_config();
		 break;
	 case SENSORS::EDA:
		 status = mw.eda_app.dcb_delete_config();
		 break;
	 default:
		 status = ret::sdk_status::SDK_ERR_INVALID_VALUE;
		 std::cout << "Unknown sensor detected" << std::endl;
		 break;
	 }
	 return static_cast<SDK_STATUS>(status);
 }

 void  write_dcgreg(SENSORS sensor, registers *pReg)
 {

	 if (sensor == SENSORS::PPG)
	 {
		 std::vector<uint16_t> reg_addr(pReg->address, pReg->address + pReg->length);
		 std::vector<uint32_t> reg_val(pReg->value, pReg->value + pReg->length);

		 std::vector<std::pair<uint16_t, uint32_t>> ret_vals;

		 for (int i = 0; i < pReg->length; i++)
		 {
			 std::pair<uint16_t, uint32_t> v;
			 v.first = reg_addr[i];
			 v.second = reg_val[i];
			 ret_vals.push_back(v);
		 }

		 mw.ppg_app.write_dcb_config(ret_vals);
	 }
	 else
	 {
		 std::vector<uint16_t> reg_addr(pReg->address, pReg->address + pReg->length);
		 std::vector<uint16_t> reg_val(pReg->value, pReg->value + pReg->length);

		 std::vector<std::pair<uint16_t, uint16_t>> ret_vals;

		 for (int i = 0; i < pReg->length; i++)
		 {
			 std::pair<uint16_t, uint16_t> v;
			 v.first = reg_addr[i];
			 v.second = reg_val[i];
			 ret_vals.push_back(v);
		 }

		 switch (sensor)
		 {
		 case SENSORS::ADXL:
			 mw.adxl_app.write_dcb_config(ret_vals);
			 break;
		 case SENSORS::ADPD4000:
			 mw.adpd4000_app.write_dcb_config(ret_vals);
			 break;
		 case SENSORS::ECG:
			 mw.ecg_app.write_dcb_config(ret_vals);
			 break;
		 case SENSORS::EDA:
			 mw.eda_app.write_dcb_config(ret_vals);
			 break;
		 default:
			 std::cout << "Unknown sensor detected" << std::endl;
			 break;
		 }
	 }
	
 }

 void read_dcb_registers(SENSORS sensor, registers *pReg)
 {

	 if (sensor == SENSORS::PPG)
	 {
		 std::vector<std::pair<uint16_t, uint32_t>> ret_vals;
		 ret_vals = mw.ppg_app.read_dcb_config();

		 for (int i = 0; i < ret_vals.size(); i++)
		 {
			 pReg->address[i] = ret_vals[i].first;
			 pReg->value[i] = ret_vals[i].second;
		 }

		 pReg->length = ret_vals.size();
	 }
	 else
	 {
		 std::vector<std::pair<uint16_t, uint16_t>> ret_vals;
		 switch (sensor)
		 {
		 case SENSORS::ADXL:
			 ret_vals = mw.adxl_app.read_dcb_config();
			 break;
		 case SENSORS::ADPD4000:
			 ret_vals = mw.adpd4000_app.read_dcb_config();
			 break;
		 case SENSORS::ECG:
			 ret_vals = mw.ecg_app.read_dcb_config();
			 break;
		 case SENSORS::EDA:
			 ret_vals = mw.eda_app.read_dcb_config();
			 break;
		 default:
			 std::cout << "Unknown sensor detected" << std::endl;
			 break;
		 }

		 for (int i = 0; i < ret_vals.size(); i++)
		 {
			 pReg->address[i] = ret_vals[i].first;
			 pReg->value[i] = ret_vals[i].second;
		 }

		 pReg->length = ret_vals.size();
	 }
	
	
 }


/*=========================================================================================
|									LEVEL 2 API											   |
===========================================================================================*/

 SDK_ERR_STATUS  startADXL()
{
	 ret::err_status status;
	status = mw.start_adxl(ADXL_Stream_cb);
	return static_cast<SDK_ERR_STATUS>(status);
}

 SDK_ERR_STATUS  stopADXL()
{
	 ret::err_status status;
	ADXL_cb = NULL;
	status = mw.stop_adxl();
	return static_cast<SDK_ERR_STATUS>(status);

}
/*
 SDK_ERR_STATUS  startADPD(adpd_stream_sum adpd_sum_cb, adpd_stream_4ch adpd_4ch_cb)
{
	 ret::err_status status;
	ADPD_4CH_cb = adpd_4ch_cb;
	ADPD_SUM_cb = adpd_sum_cb;
	status = mw.start_adpd(ADPD_Stream_cb);
	return static_cast<SDK_ERR_STATUS>(status);

}

 SDK_ERR_STATUS  stopADPD()
{
	 ret::err_status status;
	ADPD_4CH_cb = NULL;
	ADPD_SUM_cb = NULL;
	status = mw.stop_adpd();
	return static_cast<SDK_ERR_STATUS>(status);

}
*/
 SDK_ERR_STATUS  startPPG()
{
	 ret::err_status status;
	status = mw.start_syncppg(PPG_stream_cb, SYNCPPG_stream_cb);
	return static_cast<SDK_ERR_STATUS>(status);

}

 SDK_ERR_STATUS  stopPPG()
{
	 ret::err_status status;
	PPG_cb = NULL;
	SYNCPPG_cb = NULL;
	status = mw.stop_syncppg();
	return static_cast<SDK_ERR_STATUS>(status);

}

 SDK_ERR_STATUS  startADPD4xxx()
{
	ret::err_status status;
	status = mw.start_adpd4000(ADPD4xxx_stream_cb);
	return static_cast<SDK_ERR_STATUS>(status);
}

 SDK_ERR_STATUS  stopADPD4xxx()
{
	 ret::err_status status;
	ADPD4XXX_cb = NULL;
	status = mw.stop_adpd4000();
	return static_cast<SDK_ERR_STATUS>(status);
}

 SDK_ERR_STATUS  startECG()
{
	 ret::err_status status;
	status = mw.start_ecg(ECG_stream_cb);
	return static_cast<SDK_ERR_STATUS>(status);
}

 SDK_ERR_STATUS  stopECG()
{
	 ret::err_status status;
	ECG_cb = NULL;
	status = mw.stop_ecg();
	return static_cast<SDK_ERR_STATUS>(status);
}

 SDK_ERR_STATUS  startEDA()
{
	 ret::err_status status;
	status = mw.start_eda(EDA_stream_cb);
	return static_cast<SDK_ERR_STATUS>(status);

}
 SDK_ERR_STATUS  stopEDA()
{
	 ret::err_status status;
	EDA_cb = NULL;
	status = mw.stop_eda();
	return static_cast<SDK_ERR_STATUS>(status);

}


 SDK_ERR_STATUS  startTEMPARATURE()
 {
	 ret::err_status status;
	 status = mw.start_temperature(TEMP_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);

 }
 SDK_ERR_STATUS  stopTEMPARATURE()
 {
	 ret::err_status status;
	 TEMP_cb = NULL;
	 status = mw.stop_temperature();
	 return static_cast<SDK_ERR_STATUS>(status);

 }

 SDK_ERR_STATUS startADPD4000_g()
 {
	 ret::err_status status;
	 status = mw.start_adpd4000_g(ADPD4xxx_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopADPD4000_g()
 {
	 ret::err_status status;
	 status = mw.stop_adpd4000_g();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS startADPD4000_r()
 {
	 ret::err_status status;
	 status = mw.start_adpd4000_r(ADPD4xxx_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopADPD4000_r()
 {
	 ret::err_status status;
	 status = mw.stop_adpd4000_r();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS startADPD4000_b()
 {
	 ret::err_status status;
	 status = mw.start_adpd4000_b(ADPD4xxx_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopADPD4000_b()
 {
	 ret::err_status status;
	 status = mw.stop_adpd4000_b();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS startADPD4000_ir()
 {
	 ret::err_status status;
	 status = mw.start_adpd4000_ir(ADPD4xxx_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopADPD4000_ir()
 {
	 ret::err_status status;
	 status = mw.stop_adpd4000_ir();
	 return static_cast<SDK_ERR_STATUS>(status);
 }



 SDK_ERR_STATUS startBCM(bcm_stream_cb bcm_cb)
 {
	 ret::err_status status;
	 BCM_cb = bcm_cb;
	 status = mw.start_bcm(BCM_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopBCM()
 {
	 ret::err_status status;
	 BCM_cb = NULL;
	 status = mw.stop_bcm();
	 return static_cast<SDK_ERR_STATUS>(status);
 }
/*
 SDK_ERR_STATUS startHRV(hrv_stream_cb hrv_cb)
 {
	 ret::err_status status;
	 HRV_cb = hrv_cb;
	 status = mw.start_hrv(HRV_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopHRV()
 {
	 ret::err_status status;
	 HRV_cb = NULL;
	 status = mw.stop_hrv();
	 return static_cast<SDK_ERR_STATUS>(status);
 }
*/

 
 SDK_ERR_STATUS startPEDOMETER(pedometer_stream_cb ped_cb)
 {
	 ret::err_status status;
	 PED_cb = ped_cb;
	 status = mw.start_pedometer(PED_stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopPEDOMETER()
 {
	 ret::err_status status;
	 PED_cb = NULL;
	 status = mw.stop_pedometer();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS startBATTERY(battery_stream_cb battery_cb)
 {
	 ret::err_status status;
	 BATTERY_cb = battery_cb;
	 status = mw.start_battery(BAt_Stream_cb);
	 return static_cast<SDK_ERR_STATUS>(status);
 }
 SDK_ERR_STATUS stopBATTERY()
 {
	 ret::err_status status;
	 BATTERY_cb = NULL;
	 status = mw.stop_battery();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logADXL(void)
 {
	 ret::err_status status;
	 status = mw.start_log_adxl();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logADXL(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_adxl();
	 return static_cast<SDK_ERR_STATUS>(status);
 }
/*
 SDK_ERR_STATUS start_logADPD(void)
 {
	 ret::err_status status;
	 status = mw.start_log_adpd();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logADPD(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_adpd();
	 return static_cast<SDK_ERR_STATUS>(status);
 }
*/

 SDK_ERR_STATUS start_logSYNCPPG(void)
 {
	 ret::err_status status;
	 status = mw.start_log_syncppg();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logSYNCPPG(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_syncppg();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logECG(void)
 {
	 ret::err_status status;
	 status = mw.start_log_ecg();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logECG(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_ecg();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logTemperature(void)
 {
	 ret::err_status status;
	 status = mw.start_log_temperature();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logTemperature(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_temperature();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logEDA(void)
 {
	 ret::err_status status;
	 status = mw.start_log_eda();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logEDA(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_eda();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logPedometer(void)
 {
	 ret::err_status status;
	 status = mw.start_log_pedometer();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logPedometer(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_pedometer();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logBCM(void)
 {
	 ret::err_status status;
	 status = mw.start_log_bcm();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logBCM(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_bcm();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

/*
 SDK_ERR_STATUS start_logHRV(void)
 {
	 ret::err_status status;
	 status = mw.start_log_hrv();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logHRV(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_hrv();
	 return static_cast<SDK_ERR_STATUS>(status);
 }
*/

 SDK_ERR_STATUS start_logBattery(void)
 {
	 ret::err_status status;
	 status = mw.start_log_battery();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logBattery(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_battery();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logADPD4000_g(void)
 {
	 ret::err_status status;
	 status = mw.start_log_adpd4000_g();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logADPD4000_g(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_adpd4000_g();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logADPD4000_r(void)
 {
	 ret::err_status status;
	 status = mw.start_log_adpd4000_r();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logADPD4000_r(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_adpd4000_r();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logADPD4000_ir(void)
 {
	 ret::err_status status;
	 status = mw.start_log_adpd4000_ir();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logADPD4000_ir(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_adpd4000_ir();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 SDK_ERR_STATUS start_logADPD4000_b(void)
 {
	 ret::err_status status;
	 status = mw.start_log_adpd4000_b();
	 return static_cast<SDK_ERR_STATUS>(status);
 }

 SDK_ERR_STATUS stop_logADPD4000_b(void)
 {
	 ret::err_status status;
	 status = mw.stop_log_adpd4000_b();
	 return static_cast<SDK_ERR_STATUS>(status);
 }


 /*=========================================================================================
 |									WATCH												   |
 ===========================================================================================*/

 SDK_DEVICEID getDeviceID()
 {
	 ret::device_identifiers deviceID;
	 deviceID = mw.getDeviceID();
	 return static_cast<SDK_DEVICEID>(deviceID);
 }

 /*=========================================================================================
 |									DART												   |
 ===========================================================================================*/

 // Initialize `dart_api_dl.h`
 DART_EXPORT intptr_t InitDartApiDL(void* data) {
   return Dart_InitializeApiDL(data);
 }

 DART_EXPORT void RegisterSendPort(Dart_Port _send_port) {
   send_port = _send_port;
}

DART_EXPORT void RegisterADPDPort(Dart_Port _adpd_port) {
    adpd_port = _adpd_port;
}

DART_EXPORT void RegisterADXLPort(Dart_Port _adxl_port) {
    adxl_port = _adxl_port;
}

DART_EXPORT void RegisterSyncPPGPort(Dart_Port _syncppg_port) {
	syncppg_port = _syncppg_port;
}

DART_EXPORT void RegisterPPGPort(Dart_Port _ppg_port) {
	ppg_port = _ppg_port;
}

DART_EXPORT void RegisterECGPort(Dart_Port _ecg_port) {
	ecg_port = _ecg_port;
}

DART_EXPORT void RegisterEDAPort(Dart_Port _eda_port) {
	eda_port = _eda_port;
}

DART_EXPORT void RegisterTempPort(Dart_Port _temp_port) {
	temp_port = _temp_port;
}