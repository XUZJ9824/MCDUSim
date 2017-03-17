
import sys
from time import sleep
sys.path.append("C:\Program Files\EMS Satcom\MCDUSIM\Python")
import emstools

def ReturnToMainMenuStd3(mcduComObj,mcdu):
    """
        Call this function to make MCDU to go to its main menu.
        This is for screen type 3 (Honeywell).
    """
    if mcduComObj.GetScreenTxt (1,5,16,mcdu) == "SATCOM MAIN MENU":
        return
    for k in range(0,10):
        mcduComObj.SendKey("SER6",mcdu)
        sleep(0.5)
        if mcduComObj.GetScreenTxt (1,5,16,mcdu) == "SATCOM MAIN MENU":
            return
    raise Exception

def ReturnToMainMenuStd2(mcduComObj,mcdu):
    """
        Call this function to make MCDU to go to its main menu.
        This is for screen type 2 (EMS).
    """
    if mcduComObj.GetScreenTxt (1,2,22,mcdu) == "SATCOM MAIN MENU   1/2":
        return
    for k in range(0,25):
        mcduComObj.SendKey("SEL6",mcdu)
        sleep(0.5)
        if mcduComObj.GetScreenTxt (1,2,22,mcdu) == "SATCOM MAIN MENU   1/2":
            return
    raise Exception

def MakeA2GCall(mcduComObj,destNum,mcdu,std=2):
    print "*** go to main menu"
    if std == 2:
        ReturnToMainMenuStd2(mcduComObj,mcdu)
    elif std == 3:
        ReturnToMainMenuStd3(mcduComObj,mcdu)
    else:
        print "Only std= 2 or std = 3 are supported now"
        raise Exception
    print "*** Make sure system is log on"
    if mcduComObj.GetScreenTxt(2,2,3,mcdu) == "NOT":
        print "******* Error: System not logged on"
        raise Exception
    
    print "typing destination number"
    mcduComObj.SendText(destNum,mcdu)
    print "Dial button"
    if std == 2:
        mcduComObj.SendKey("SEL1",mcdu)
    else:
        mcduComObj.SendKey("SEL1",mcdu)

if __name__ == "__main__":
    ### This script takes two parameters:
    ###   - first is the MCDU number (default 1)
    ###   - second is the screen layout standard
    ###     2 or 3; default is 2.
    try:
        mcdu = sys.argv[1]
    except:
        print "mcdu not provided...assume = 1"
        mcdu = 1
    try:
        std = sys.argv[2]
    except:
        print "std not provided...assume = 2"
        std = 2
    print "std = ", std
    print "*** connecting to MCDU Simulator"
    mcduComObj = emstools.McduSimCom()
    MakeA2GCall(mcduComObj,"0016135919208",mcdu,std)
    sleep(2)
    del mcduComObj

