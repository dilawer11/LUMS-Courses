# 445 Filtered 440 Closed 80 Open for scanme.nmap.org
import socket
import sys
from threading import Thread
import time


class PortScanner:
	def __init__(self):
		print "Please specify a target name"
		sys.exit()
	def __init__(self, destName):
		self.connectToDestination(destName,81)

	def connectToDestination(self,destName,portsToScan):
		startTime = time.time()
		results = []
		try:
			s =socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		except socket.error as err:
			print "socket creation failed with error %s" %(err)
		try:
			ip = socket.gethostbyname(destName)
		except socket.gaierror:
			print "there was an error resolving the host"
			sys.exit()
		print "Server IP Found : %s" %(ip)
		s.close()
		for port in range(portsToScan):
			try:
				s =socket.socket(socket.AF_INET,socket.SOCK_STREAM)
			except socket.error as err:
				print "socket creation failed with error %s" %(err)
			try:
				response = s.connect_ex((ip,port))
				if response==110:
					print "Filtered"
				elif response==111:
					print "Closed"
				else:
					print "Open"
					temp = {}
					temp["Port"]=port
					temp["Service"]=socket.getservbyport(port,'tcp')
					results.append(temp)


			except socket.error as err:
				print err
			s.close();
		timeTaken = time.time()-startTime
		print "Socket Closed"
		print "Per Port = %f Seconds" %(timeTaken/portsToScan)
		print results


def threaded_function(arg):
    for i in range(arg):
        print "running %d" %(arg)
        sleep(1)
		

if __name__ == "__main__":
	if len(sys.argv)==2:
		ps = PortScanner(sys.argv[1])
	else:
		print "Error arguments not as many as expected"	
		thread = Thread(target = threaded_function, args = (7, ))
		thread2 = Thread(target = threaded_function, args = (5, ))
		thread.start()
		thread2.start()
		thread.join()
		thread2.join()
		print "thread finished...exiting"
   