import socket, sys, time

#host = sys.argv[1]
host = "localhost"

textport = "6000"

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
port = int(textport)
s.bind(( host, 5000 ))
s.connect((host, port))
print "Enter data to transmit: "
data = sys.stdin.readline().strip()
s.sendall(data)
s.shutdown(1)
print "Looking for replies; press Ctrl-C or Ctrl-Break to stop."
while 1:
    buf = s.recv(2048)
    if not len(buf):
        break
    print "Received: %s" % buf
