import emstools
from time import sleep



def readPhoneBook( filename ):
    """
        line format: "lastName, firstName"\t1162\tADM\tB2\t
        """
    f = open(filename)
    ret = []
    for line in f.readlines():
        if line[0] == '\t':
            continue
        lastname = line[(line.find('\"') + 1 ):( line.find(','))]
        firstname = line[(line.find(',') + 2 ):( line.find('\"',2))]
        number = line[(line.find('\t') + 1 ):( line.find('\t',(line.find('\t') + 1)))]
        fullname = firstname.upper() + " " + lastname.upper()
        fullname = fullname[0:14]
        #print fullname
        ret.append( [fullname, number] )
        
    f.close()
    return ret


def ReturnToMainMenu(mcduComObj):
    if mcduComObj.GetScreenTxt (1,5,16) == "SATCOM MAIN MENU":
        return
    for k in range(0,10):
        mcduComObj.SendKey("SER6")
        sleep(0.5)
        if mcduComObj.GetScreenTxt (1,5,16) == "SATCOM MAIN MENU":
            return
    raise Exception
    

def DownloadPhonebook(filename, numEntries = 0, categoty=1):
    ### Lets create an Object to communicate with MCDU
    mcduComObj = emstools.McduSimCom()
    counter = 0

    ### Lets send 6 SER6 key to return to the main phone scheen
    print "Goto main menu"
    ReturnToMainMenu(mcduComObj)

    ### Send a hello FOR 0.5 SECOND and clear it just for visual confirmation
    print "Write HELLO for a second"
    mcduComObj.SendKey("CLR")
    mcduComObj.SendLastKey()
    mcduComObj.SendText("HELLO")
    sleep(0.5)
    mcduComObj.SendKey("CLR")
    mcduComObj.SendLastKey()

    ### Go to Directory menu
    print "Goto Directory menu"
    mcduComObj.SendKey("SER6")

    ### Make sure we are in Dir page
    if mcduComObj.GetScreenTxt (1,5,16) != "SATCOM DIRECTORY":
        raise Exception

    sleep(1)

    ### goto phonebook page
    print "Goto Phonebook page"
    if categoty == 1:
        mcduComObj.SendKey("SEL1")
    elif categoty == 2:
        mcduComObj.SendKey("SEL2")
    elif categoty == 3:
        mcduComObj.SendKey("SER1")
    elif categoty == 4:
        mcduComObj.SendKey("SER2")
    else:
        raise Exception

    sleep(1)

    ### Make sure we are in phonebook page based on SAT and PRI labels
    if mcduComObj.GetScreenTxt (2,22,3) != "SAT":
        raise Exception
    if mcduComObj.GetScreenTxt (4,22,3) != "PRI":
        raise Exception

    ### go to last page ###
    print "Goto the last Phonebook page"
    page = -1
    lastpage = 0
    maxCounter = 22
    while page != lastpage:
        c = mcduComObj.GetScreenTxt (1,20,5)
        i = c.find('/')
        if i < 0:
            raise Exception
        page = int(c[0:i])
        lastpage = int(c[(i+1):])
        print "  page %d of %d"  % (page,lastpage)
        if page == lastpage:
            break
        maxCounter = maxCounter - 1
        if maxCounter == 0:
            raise Exception
        mcduComObj.SendKey("NEXT_PAGE")
        sleep(1)

    ################# Phone book insertion loop ###########
    r = readPhoneBook( filename )
    for nameAndNumber in r[:]:
        mcduComObj.SendKey("NEXT_PAGE")
        sleep(1)
        ### Search for '[' character ###
        if mcduComObj.GetScreenTxt (5,2,2) == "[ ":
            button = "SEL2"
        elif mcduComObj.GetScreenTxt (7,2,2) == "[ ":
            button = "SEL3"
        elif mcduComObj.GetScreenTxt (9,2,2) == "[ ":
            button = "SEL4"
        elif mcduComObj.GetScreenTxt (11,2,2) == "[ ":
            button = "SEL5"
        else:
            raise Exception

        sleep(1)

        print "Inserting Name = \"%s\", Number = 613-591-6040-%s" % ( nameAndNumber[0], nameAndNumber[1] )
        mcduComObj.SendText(nameAndNumber[0])
        sleep(0.5)
        mcduComObj.SendKey(button)
        sleep(0.5)
        mcduComObj.SendText("613-591-6040-")
        mcduComObj.SendText(nameAndNumber[1])
        sleep(1)
        mcduComObj.SendKey(button)
        mcduComObj.SendKey("SEL6")
        sleep(2)
        counter = counter + 1
        if counter >= numEntries:
            break;

    print "Done"

if __name__ == "__main__":
    #mcduComObj = emstools.McduSimCom(0)
    DownloadPhonebook("PhoneList.txt",3,2)
        

