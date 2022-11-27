parseIn params.in newParams.in
perl dprepro newParams.in BigBay.WDT BigBay.WDC
WinDamSim.exe BigBay.WDC
parseBri BigBay.BRI results.out
