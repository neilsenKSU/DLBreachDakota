#
# parseOut.py - parse .out file to extract peak breach discharge
# 
#    Time(hr),     Qbreach,    Qspill,    zs_upstr, zs_downstr, zb_breach, B_bottom,    B_top,  Areabreach, Brchsideslope, Vol_watout, USSedRate, DSSedRate 
#     0.00132      0.18203      0.00000    13.5000     0.0023     2.2996     0.2008     0.2008     0.0403 **********          0.906    0.00000    0.00384
#     0.00271      0.18382      0.00000    13.5000     0.0023     2.2992     0.2015     0.2015     0.0406 **********          1.821    0.00000    0.00387
#
# Output fields:
#  0. time (in hours), 
#  1. breach flow discharge (m^3/s), 
#  2. flow discharge through spillways/gates on embankment (m^3/s),
#  3. upstream water level with reference to embankment base (m), 
#  4. downstream water level (m), 
#  5. breach bottom elevation (m), 
#  6. breach bottom width (m), 
#  7. breach top width (m), 
#  8. flow area at the breach (m^2), 
#  9. breach side slope (V/H), 
# 10. cumulative water volume from breach and spillway/gate (m^3),
# 11. sediment discharge at upstream (m^3/s),
# 12. sediment discharge at downstream (m^3/s). 
#


def isPresent(str):
	try:
		float(str)
		return True
	except ValueError:
		return False

def readdata(file_name,start_line=2): # ignore first two lines
	with open(file_name,'r') as f:
		data = f.read().split('\n')
	data = [i.split() for i in data[start_line:len(data)-1]]
	for i in range(len(data)):
		row = [(sub) for sub in data[i] if len(sub)!=0]
		yield float(row[0])*3600,float(row[1]),float(row[7]),isPresent(row[9])
    	
iterator = readdata('BigBay_cards.out')

timeVal, flowVal, breachTopWidth, isCollapsed = zip(*iterator)

max_Flow = max(flowVal)
max_Index = flowVal.index(max_Flow)
max_Time = timeVal[max_Index]
max_Width = max(breachTopWidth)

try:
	prc_Index = isCollapsed.index(True)
	prc_Time = timeVal[prc_Index]
	prc_Width_b = breachTopWidth[prc_Index-1]
	prc_Width_a = breachTopWidth[prc_Index]
except:
	prc_Time = -1
	prc_Width_b = -1
	prc_Width_a = -1

print('Max flow: ', max_Flow, 'at time: ', max_Time,', max breach top width: ', max_Width)
print('Collapsed at time: ', prc_Time, ', breach top width: ', prc_Width_b, '-', prc_Width_a)

with open('results.out', "w") as of:
	of.write("%.5f" % max_Flow + "    f\n")
	of.write("%.5f" % max_Time + "    t\n")
	of.write("%.5f" % max_Width + "    w\n")
	of.write("%.5f" % prc_Time + "    t\n")
	of.write("%.5f" % prc_Width_b + "    w\n")
	of.write("%.5f" % prc_Width_a + "    w\n")
