parseIn params.in newParams.in
perl dprepro newParams.in ARSP1.WDT ARSP1.WDC
WinDamSim.exe ARSP1.WDC
parseRes ARSP1.OUT ARSP1.BRI results.out
