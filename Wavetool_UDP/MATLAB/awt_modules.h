#pragma once
#ifndef __AWT_MODULES__
#define __AWT_MODULES__

//#define STDDEF_CALLCONV
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT     50007 
#define MAXLINE 4096 
/* Max slots set to accommodate both 4000 and ADPD */
#define NUM_SLOTS (12)
/* Max channels set to accommodate both 4000 and ADPD */
#define NUM_CH	(6)
#define FRAME_SZ (2048)
#define MAX_SLOTINFO_ITER	(50)

#define UDP_SOCKET_OPEN_ERROR		(-1)
#define UDP_SOCKET_BIND_ERROR		(-2)

#define AWT_CTRL_PARAM_ERROR		(-1)
#define AWT_UDP_SEND_ERROR			(-2)
#define AWT_UDP_RESPRX_ERROR		(-3)

#define AWT_SOCKET_TIMEOUT			(-1)
#define AWT_SOCKET_ERROR			(-2)
#define AWT_TRANSFER_SUCCESS		(0)

#define AWT_TIMEOUT_SEC				(5)

typedef unsigned char       uint8;

typedef struct awt_pkt_header {

	uint8 source;
	uint8 length[2];
	uint8 seqnum[2];
	uint8 timestamp[4];
} awt_pkt_header;

typedef struct awt_adpd_pkt {
	uint8 slotinfo[NUM_SLOTS];
	uint8 nViewIntStInfo;
	uint8 nPlot1UnitOffInfo;
	uint8 nPlot2UnitOffInfo;
} awt_adpd_pkt;

typedef struct awt_adpd_intstatus_pkt {
	uint8 bIsIntDataAvail;
	double dtimestamp;
	double nSeqId;
	double nIntFla[5];
	double nLeadsStat;
} awt_adpd_intstatus_pkt;

typedef enum {
	ADPD_GRAPH_STREAM = 0xA0,
	ADPD4000_GRAPH_STREAM = 0xA1,
	ADXL_GRAPH_STREAM = 0xB0,
	PPG_GRAPH_STREAM = 0xC0,
	SPO2_GRAPH_STREAM = 0xC0,
	ECG_GRAPH_STREAM = 0xC2,
	EDA_GRAPH_STREAM = 0xC3,
	TEMP_GRAPH_STREAM = 0xC4
} ADPD_GRAPH_STREAMINFO;

typedef enum {
	ADPD_SLOTOFF = 0,
	ADPD_SUM32 = 1,
	ADPD_4CH = 2,
	ADPD_DIG = 3
} ADPD_SLOTINFO;

__declspec(dllexport) int awt_opensocket(int port, int nMaxFrameSz);
__declspec(dllexport) int awt_transfermodule(int nInstId, int *pSlotSampCnt, double *pAdpdGraphData, int *pTimestampData, int* pSlotFrmCnt);
__declspec(dllexport) void awt_closesocket(int nInstId);
__declspec(dllexport) int awt_getadpdslotinfo(int nInstId, awt_pkt_header* pPktHdr, awt_adpd_pkt* pAdpdSlotInfo);
__declspec(dllexport) int awt_getinterruptStatus(int nInstId, double *pIntTimestamp, double *pSeqID, double *pIntData);


__declspec(dllexport) int awt_ctrl_opensocket(int txport, int rxport);
__declspec(dllexport) void awt_ctrl_closesocket(int nInstId);
__declspec(dllexport) int awt_connect(int nInstId, uint8 portNum, uint8* recvdBuf);
__declspec(dllexport) int awt_disconnect(int nInstId, uint8* recvdBuf);
__declspec(dllexport) int awt_listView(int nInstId, uint8* recvdBuf);
__declspec(dllexport) int awt_openView(int nInstId, int viewSel, uint8* recvdBuf);
__declspec(dllexport) int awt_listDcfg(int nInstId, uint8* recvdBuf);
__declspec(dllexport) int awt_loadDcfg(int nInstId, int dcfgSel, uint8* recvdBuf);
__declspec(dllexport) int awt_listCloudDcfg(int nInstId, uint8* recvdBuf);
__declspec(dllexport) int awt_loadCloudDcfg(int nInstId, int cloudDcfgSel, uint8* recvdBuf);
__declspec(dllexport) int awt_channelSelect(int nInstId, int plotNo, int streamByte, uint8* recvdBuf);
__declspec(dllexport) int awt_registerRead(int nInstId, int regAddr, uint8* recvdBuf, int* pRegVal);
__declspec(dllexport) int awt_registerWrite(int nInstId, int regAddr, int regVal, uint8* recvdBuf);
__declspec(dllexport) int awt_play(int nInstId, uint8* recvdBuf);
__declspec(dllexport) int awt_stop(int nInstId, uint8* recvdBuf);
__declspec(dllexport) int awt_slotPlotSelect(int nInstId, int plot, int slot, uint8* recvdBuf);
__declspec(dllexport) int awt_tabView(int nInstId, int view, uint8* recvdBuf);
__declspec(dllexport) int awt_m2m2PacketEnable(int nInstId, int m2m2Enable, uint8* recvdBuf);
__declspec(dllexport) int awt_softwareReset(int nInstId, uint8* recvdBuf);
__declspec(dllexport) int awt_startUDPTransfer(int nInstId, int udpTransferEnable, uint8* recvdBuf);
__declspec(dllexport) int awt_logTransferEnable(int nInstId, int logTransferEnable, uint8* recvdBuf);






























#endif