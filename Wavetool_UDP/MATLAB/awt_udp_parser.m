[notfound,warnings] = loadlibrary('awt_modules','awt_modules.h');
portnum = 50007
totalloopiter = 5000
totalslots = 12
totalch = 6
MaxFrameSz = 64
aAdpdGraphData = zeros(MaxFrameSz, totalch, totalslots);
aTimestamp = zeros(MaxFrameSz, totalslots);
aFrameSz = [32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32];
pFrameSz = libpointer('int32Ptr', aFrameSz);
aRxFrameCntSlot = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
pRxFrameCnt = libpointer('int32Ptr', aRxFrameCntSlot);
sockstatus = calllib('awt_modules', 'awt_opensocket', portnum, MaxFrameSz);
nInst = 0;
n=0;

%initialize buffers and variables to plot
nPlotSz = 256;
aAdpdGrphDataSlotA = zeros(1, nPlotSz);
aAdpdGrphDataSlotB = zeros(1, nPlotSz);
plotslotA = 0;
plotslotB = 0;
nPlotSlotAFrmCnt = 0;
nPlotSlotBFrmCnt = 0;

while (n<totalloopiter)
    aAdpdGraphData = zeros(MaxFrameSz, totalch, totalslots);
    aTimestamp = zeros(MaxFrameSz, totalslots);
    pAdpdGraphdata = libpointer('doublePtr', aAdpdGraphData);
    pTimestamp = libpointer('int32Ptr', aTimestamp);
    calllib('awt_modules', 'awt_transfermodule', nInst, pFrameSz, pAdpdGraphdata, pTimestamp, pRxFrameCnt);
    aAdpdGraphData = reshape(pAdpdGraphdata.Value, MaxFrameSz, totalch, totalslots);
    aTimestamp = reshape(pTimestamp.Value, MaxFrameSz, totalslots);
    
    % Channel information - 4xxx
    % 1 - D1
    % 2 - S1
    % 3 - L1
    % 4 - D2
    % 5 - S2
    % 6 - L2
    
    % Channel information - 108x
    % 1 - SUM32
    % 1-4 - 4CH
    
    % Slot Information
    % 1-A, 2-B, 3-C, 4-D, 5-E, 6-F, 7-G, 8-H, 9-I, 10-J, 11-K, 12-L
    
    chid = 2; 
    plot1slotid = 1;
    
    %Plot SlotA - S1
    if (aTimestamp(1,plot1slotid) > 0)
        slotFs = aFrameSz(plot1slotid);
        plotTimestamp = transpose(aTimestamp);
        if (nPlotSlotAFrmCnt >= 1)
            aAdpdGrphDataSlotA(1:(nPlotSz-slotFs)) = aAdpdGrphDataSlotA(slotFs+1:nPlotSz);
            aAdpdGrphDataSlotA((nPlotSz-slotFs+1):nPlotSz) = aAdpdGraphData(1:slotFs,chid,plot1slotid);
        else
            aAdpdGrphDataSlotA((plotslotA*slotFs)+1:((plotslotA*slotFs)+slotFs)) = aAdpdGraphData(1:slotFs,chid,plot1slotid);
        end
        plotslotA = plotslotA + 1;
        
        if (plotslotA >= (nPlotSz / aFrameSz(plot1slotid)))
            aAdpdSampCntSlotA = ((slotFs*nPlotSlotAFrmCnt)+1):((slotFs*nPlotSlotAFrmCnt)+nPlotSz);
            figure(1)
            plot(aAdpdSampCntSlotA,aAdpdGrphDataSlotA,'g');
            drawnow
            nPlotSlotAFrmCnt = nPlotSlotAFrmCnt + 1;
        end
    end      

    chid = 2; 
    plot2slotid = 2;
    %Plot SlotB - S1
    if (aTimestamp(1,plot2slotid) > 0)
        slotFs = aFrameSz(plot2slotid);
        plotTimestamp = transpose(aTimestamp);
        if (nPlotSlotBFrmCnt >= 1)
            aAdpdGrphDataSlotB(1:(nPlotSz-slotFs)) = aAdpdGrphDataSlotB(slotFs+1:nPlotSz);
            aAdpdGrphDataSlotB((nPlotSz-slotFs+1):nPlotSz) = aAdpdGraphData(1:slotFs,chid,plot2slotid);
        else
            aAdpdGrphDataSlotB((plotslotB*slotFs)+1:((plotslotB*slotFs)+slotFs)) = aAdpdGraphData(1:slotFs,chid,plot2slotid);
        end
        plotslotB = plotslotB + 1;
        
        if (plotslotB >= (nPlotSz / aFrameSz(plot2slotid)))
            aAdpdSampCntSlotB = ((slotFs*nPlotSlotBFrmCnt)+1):((slotFs*nPlotSlotBFrmCnt)+nPlotSz);
            figure(2)
            plot(aAdpdSampCntSlotB,aAdpdGrphDataSlotB,'m');
            drawnow
            nPlotSlotBFrmCnt = nPlotSlotBFrmCnt + 1;
        end
    end
    n = n+1 % Comment this line to have an infinite loop  in
end

%Please make sure to run the below code if running infinite loop and
%terminating in between. 
% Copy paste the code in command window to properly close the socket and 
% library before running the next instance of the UDP parser.
%Close Socket Code Block
calllib('awt_modules', 'awt_closesocket', nInst)
unloadlibrary('awt_modules')