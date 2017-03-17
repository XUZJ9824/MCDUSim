import socket
from time import sleep

class McduSimCom:
    """
        McduSimCom class (from emstools module) abstracts the interface with EMS-Satcom MCDU Simulator.
        Its constructor attempts to connect to all 4 MCDUs. McduSimCom constructor takes two optional
        parameters; the first is used to enable/disable the logging (1->enable, 0->disable). The second
        is the IP address of a remote McduSim. Defaults (no parameter given) are log disabled and
        "localhost".
    """
    def __init__(self,log=0,mcdusim_ip="localhost"):
        self.__version = "0.01"
        self.__log = log
        self.__Port = 8000
        self.__ConnectionST = [0,0,0,0] # 1 if connected
        self.__s = []
        self.__rx = ["","","",""]
        self.__lastKeySent = 0
        self.__keyDelayTime = 0.3
        self.__valchar = " 0123456789ABCDEFGHIJKLMNOPQRSTUVXYWZ./-"
        if self.__log != 0:
            print "Creating McduSimCom object"
        for i in range(0,4):
            self.__s.append (socket.socket(socket.AF_INET,socket.SOCK_STREAM))
            self.__s[i].settimeout(3)
            try:
                self.__s[i].connect((mcdusim_ip, self.__Port + i))
                self.__ConnectionST[i] = 1
                if self.__log != 0:
                    print "Connect to MCDU", i+1
            except:
                self.__ConnectionST[i] = 0 # error
                if self.__log != 0:
                    print "Fail to connect to MCDU", i+1
        if self.__log != 0:
            print "ConnectionST = ", self.__ConnectionST

    def __del__(self):
        if self.__log != 0:
            print "Destroying McduSimCom object"
        for i in range(0,4):
            if self.__ConnectionST[i] != 0:
                try:
                    self.__s[i].close()
                except:
                    if self.__log != 0:
                        print "Fail to close socket"

    def GetVersion(self):
        """
            Return a text with the current version number on format: 'N.NN'.
        """
        return self.__version

    def IsMcduConnected(self, mcdu = 1):
        """
            IsMcduConnected returns True or False according to the MCDU connection state.
        """
        if self.__checkMcdu(mcdu) == False:
            return False
        if self.__ConnectionST[mcdu - 1] != 0:
            return True
        else:
            return False

    def __checkMcdu(self, mcdu):
        if mcdu == 1:
            return True
        if mcdu == 2:
            return True
        if mcdu == 3:
            return True
        if mcdu == 4:
            return True
        if self.__log != 0:
            print "Bad mcdu: ",mcdu
        return False


    def __send(self, mcduIdx, string):
        if self.__log != 0:
            print "Sending: ",string
        try:
            bytes_sent = self.__s[mcduIdx].send(string)
            assert(bytes_sent == len(string))
        except:
            if self.__log != 0:
                print "Fail to send: ",string
                self.__ConnectionST[mcduIdx] = 0
            return False
        return True

    def __rcv(self, mcduIdx):
        try:
            self.__rx[mcduIdx] = self.__s[mcduIdx].recv(1024)  # limit rx to 1K
            cr_pos = self.__rx[mcduIdx].find('\r')
            if cr_pos > 0:
                self.__rx[mcduIdx] = self.__rx[mcduIdx][0:cr_pos]
            cr_pos = self.__rx[mcduIdx].find('\n')
            if cr_pos > 0:
                self.__rx[mcduIdx] = self.__rx[mcduIdx][0:cr_pos]
            if self.__log != 0:
                print "rxlen = %d text:%s" % (len(self.__rx[mcduIdx]),self.__rx[mcduIdx])
        except:
            self.__ConnectionST[mcduIdx] = 0
            return ""
        return self.__rx[mcduIdx]

    def SendKey(self, key, mcdu=1):
        """
            SendKey accepts the following characters:
              \" 0123456789ABCDEFGHIJKLMNOPQRSTUVXYWZ./-\"
            It also accepts the follwing characters represented by uppercase words:
                CLR, BACKSPACE, SEL1~SEL6, SER1~SER6, PREV_PAGE, NEXT_PAGE, MENU
                
            Returns:
                'OK'  if OK;
                'BAD_MCDU' is mcdu is not 1~4;
                'MCDU_NOT_CONNECTED' no connection with mcdu;
                    note: connection attempt happens only during Obj construction;
                'BAD_KEY_CHAR' or 'BAD_KEY_TXT' if provided a bad key;
                'FAIL_TO_SEND_DATA' if it was not able to transmit data to MCDU;
                'TIMEOUT' MCDU did not respond;
                'ERROR_FROM_MCDU' MCDU had replied with error.
        """
        if self.__checkMcdu(mcdu) == False:
            return "BAD_MCDU"
        if self.IsMcduConnected(mcdu) == False:
            return "MCDU_NOT_CONNECTED"
        mcduIdx = mcdu - 1
        # lest validate keyword

        code = ""
        if self.__log != 0:
            print "Sending key: ",key
        ### Translate characters ###

        if key == "BACKSPACE":
            code = "8"
        if key == "CLR":
            code = "8"
            
        if key == "SEL1":
            code = "112"
        if key == "SEL2":
            code = "113"
        if key == "SEL3":
            code = "114"
        if key == "SEL4":
            code = "115"
        if key == "SEL5":
            code = "116"
        if key == "SEL6":
            code = "117"

        if key == "PREV_PAGE":
            code = "118"
        if key == "NEXT_PAGE":
            code = "119"
            
        if key == "SER1":
            code = "120"
        if key == "SER2":
            code = "121"
        if key == "SER3":
            code = "122"
        if key == "SER4":
            code = "123"
        if key == "SER5":
            code = "124"
        if key == "SER6":
            code = "125"
            
        if key == "MENU":
            code = "96"

            
        if code == "":
            # let check if it is a single valid char
            if len(key) != 1:
                return "BAD_KEY_TXT"
            if self.__valchar.find(key) == -1:
                return "BAD_KEY_CHAR"
            code = '%d' % ord(key)
        txtToSend = "keycode " + code + "\r\n"
        ret = self.__send(mcduIdx, txtToSend)
        if ret == False:
            return "FAIL_TO_SEND_DATA"
        ret = self.__rcv(mcduIdx)
        if ret == "":
            return "TIMEOUT"
        if ret != "OK":
            return "ERROR_FROM_MCDU"
        self.__lastKeySent = int(code)
        sleep(self.__keyDelayTime)
        return "OK"

    def SendLastKey(self, mcdu=1):
        """
            SendLastKey will resend the last key with repeat flag set.
            Note: the last key value is a global among MCDUs. Therefore, it is just valid
            to call SendLastKey after a SendKey to the same MCDU. Otherwise, it can send a wrong
            repeat ket. Here is a wrong situation:
                mcduObj = emstoolsMcduSimCom()
                mcduObj.SendKey("A",1) # char A is sent to MCDU 1
                mcduObj.SendKey("B",2) # char B is sent to MCDU 2
                mcdu.SendLastKey(1)    # WRONG... as B is sent as repeat key to MCDU 1 (which had A).

            Returns:
                'OK'  if OK;
                'BAD_MCDU' is mcdu is not 1~4;
                'MCDU_NOT_CONNECTED' no connection with mcdu;
                    note: connection attempt happens only during Obj construction;
                'FAIL_TO_SEND_DATA' if it was not able to transmit data to MCDU;
                'TIMEOUT' MCDU did not respond;
                'ERROR_FROM_MCDU' MCDU had replied with error.
        """
        if self.__checkMcdu(mcdu) == False:
            return "BAD_MCDU"
        if self.IsMcduConnected(mcdu) == False:
            return "MCDU_NOT_CONNECTED"
        mcduIdx = mcdu - 1
        code = '%d' % (self.__lastKeySent + 128)
        txtToSend = "keycode " + code + "\r\n"
        ret = self.__send(mcduIdx, txtToSend)
        if ret == False:
            return "FAIL_TO_SEND_DATA"
        ret = self.__rcv(mcduIdx)
        if ret == "":
            return "TIMEOUT"
        if ret != "OK":
            return "ERROR_FROM_MCDU"
        sleep(self.__keyDelayTime)
        return "OK"


    def GetScreenTxt(self, line, col, numchars, mcdu=1):
        """
            Returns the text from the active MCDU screen.
            range: line 1~14, col 1~24. If the request overflows it will return an error
            (ex: mcduObj.GetScreenTxt(1, 23, 2) ->as len = 2 it is requesting a 25th character).

            Retuns:
                screen text or:
                'badParameter'
                'BAD_MCDU' is mcdu is not 1~4;
                'MCDU_NOT_CONNECTED' no connection with mcdu;
                    note: connection attempt happens only during Obj construction;
                'BAD_KEY_CHAR' or 'BAD_KEY_TXT' if provided a bad key;
                'FAIL_TO_SEND_DATA' if it was not able to transmit data to MCDU;
                'TIMEOUT' MCDU did not respond;

                
        """
        if self.__checkMcdu(mcdu) == False:
            return "BAD_MCDU"
        if self.IsMcduConnected(mcdu) == False:
            return "MCDU_NOT_CONNECTED"
        mcduIdx = mcdu - 1
        lineTxt = '%d' % line
        colTxt = '%d' % col
        numcharsTxt = '%d' % numchars
        txtToSend = "screentxt " + lineTxt + " " + colTxt + " " + numcharsTxt + "\r\n"
        ret = self.__send(mcduIdx, txtToSend)
        if ret == False:
            return "FAIL_TO_SEND_DATA"
        ret = self.__rcv(mcduIdx)
        if ret == "":
            return "TIMEOUT"
        return ret


    def GetXmlScreenshot(self, mcdu=1):
        """Return current MCDU screen as a string in XML format."""
        if self.__checkMcdu(mcdu) == False:
            return "BAD_MCDU"
        if self.IsMcduConnected(mcdu) == False:
            return "MCDU_NOT_CONNECTED"
        mcduIdx = mcdu - 1
        xml_request = "screenxml\r\n"
        send_result = self.__send(mcduIdx, xml_request)
        if not send_result:
            raise AssertionError, ("Failed to send data to MCDU" +
                                   str(self.mcdu))
        xml_screenshot = ""
        data_end = "</mcdu>"
        while xml_screenshot.find(data_end) == -1:
            """Receive large amounts of data; don't catch any errors raised."""
            rx_buffer_size = 2 ** 17 # larger than the buffer size in pythonInt.cpp
            next_data = self.__s[mcduIdx].recv(rx_buffer_size)
            xml_screenshot = xml_screenshot + next_data
        return xml_screenshot

    def MakeScreenshot(self, mcdu=1):
        """
            MCDU stores a screenshot on its default directory. The filename is: "exportedMcduScreen.jpg"
            It is recommended to have the Python Application to change the name after MCDU has saved the
            file. This request is asynchronous  and there is a 1 second delay to return.
            
            Returns:
                'OK'  if OK;
                'BAD_MCDU' is mcdu is not 1~4;
                'MCDU_NOT_CONNECTED' no connection with mcdu;
                    note: connection attempt happens only during Obj construction;
                'FAIL_TO_SEND_DATA' if it was not able to transmit data to MCDU;
                'TIMEOUT' MCDU did not respond;
                'ERROR_FROM_MCDU' MCDU had replied with error.
        """

        if self.__checkMcdu(mcdu) == False:
            return "BAD_MCDU"
        if self.IsMcduConnected(mcdu) == False:
            return "MCDU_NOT_CONNECTED"
        mcduIdx = mcdu - 1
        txtToSend = "makescreenshot\r\n"
        ret = self.__send(mcduIdx, txtToSend)
        if ret == False:
            return "FAIL_TO_SEND_DATA"
        ret = self.__rcv(mcduIdx)
        if ret == "":
            return "TIMEOUT"
        if ret != "OK":
            return "ERROR_FROM_MCDU"
        sleep(1)
        return "OK"

    def SetKeyDelay(self, timeInSeconds ):
        """
            Once Python gets an ack back from SDU and MCDU it waits
            to return to the calles. This function sets this time.
            The default time is 0.3 seconds

            Return: non.
        """
        self.__keyDelayTime = timeInSeconds

    def SendText(self, text, mcdu=1):
        """
            SendText calls SendKey for each character int the text.
            returns same then SendKey.
        """
        for k in text[:]:
            ret = self.SendKey(k, mcdu)
            if ret != 'OK':
                return ret
        return ret



if __name__ == "__main__":
    m = McduSimCom(1)
    print "********* Lets send a key *********"
    ret = m.SendKey("H",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("E",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("L",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("L",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("O",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey(" ",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("P",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("Y",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("T",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("H",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("O",1)
    print ret
    print "********* Lets send a key *********"
    ret = m.SendKey("N",1)
    print ret
    #print "********* Repeat a key *********"
    #ret = m.SendLastKey(1)
    #print ret
    print "********* Lets get a screen text *********"
    ret = m.GetScreenTxt(13,2,7,1)
    print "screen text:", ret
    print "********* Lets get a screen text *********"
    ret = m.GetScreenTxt(13,1,24,1)
    print "screen text:", ret

    print "********* Lets create a screenshot *********"
    ret = m.MakeScreenshot()
    print "screen text:", ret

    del m
    print "done"
    #print ret
    #r = m.Rcv()
    #print "got: ", r
    #del m

