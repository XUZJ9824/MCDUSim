import sys
import time

sys.path.append("C:\\Users\\macrae\\Documents\\Visual Studio 2005\\Projects\\mcduSim\\PythonMod")
import emstools
m = emstools.McduSimCom()

print "Which MCDUs are connected"
for i in range(1,5):
    print "    ",i,m.IsMcduConnected(i)

for i in range(1,5):
    m.SendKey("MENU",i)
print "All MCDUs should now be showing: Menu page"
time.sleep(1)

for i in range(1,5):
    m.SendKey("SEL1",i)
print "All MCDUs should now be showing: SDU page"
time.sleep(1)

outgoing = ["ABC", "DEF", "GHI", "JKL"]
for i in range(1,5):
	m.SendText(outgoing[i-1],i)
results = ""
j=14 # scratchpad row
bOK = True
for i in range(1,5):
    if m.IsMcduConnected(i):
	incoming = m.GetScreenTxt(j, 1, 24, i).strip()
        results = results + " " + str(i) + "{" + incoming + "}"
	if incoming != outgoing[i-1]:
	    bOK = False

print "Scratchpad has: " + results + " -- " + ('Wrong!', 'OK')[bOK]
time.sleep(2)

print  "    +------------------------+    +------------------------+"

for j in range(1,15):
    l1 = "                        "
    if m.IsMcduConnected(1):
        l1 = m.GetScreenTxt(j, 1, 24, 1)
    l2 = "                        "
    if m.IsMcduConnected(2):
        l2 = m.GetScreenTxt(j, 1, 24, 2)
    print  "   1|" + l1.ljust(24,'?') + "|   2|" + l2.ljust(24,'?') + "|", j

print  "    +------------------------+    +------------------------+"

for j in range(1,15):
    l3 = "                        "
    if m.IsMcduConnected(3):
        l3 = m.GetScreenTxt(j, 1, 24, 3)
    l4 = "                        "
    if m.IsMcduConnected(4):
        l4 = m.GetScreenTxt(j, 1, 24, 4)
    print  "   3|" + l3.ljust(24,'?') + "|   4|" + l4.ljust(24,'?') + "|", j

print  "    +------------------------+    +------------------------+"
time.sleep(2)

results = ""
for i in range(1,5):
    if m.IsMcduConnected(i):
        xml = m.GetXmlScreenshot(i)
        results = results + " " + str(i) + "{len=" + str(len(xml)) + "}"
print "Xml Screen shots:" + results
time.sleep(2)

time.sleep(5)

for i in range(1,5):
    m.SendKey("MENU",i)
print "All MCDUs should now be showing: Menu page"

del m
