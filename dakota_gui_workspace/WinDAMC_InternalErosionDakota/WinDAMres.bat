parseIn params.in newParams.in
perl dprepro newParams.in BigBay.WDT BigBay.WDC
WinDamSim.exe BigBay.WDC
parseRes BigBay.OUT BigBay.BRI results.out