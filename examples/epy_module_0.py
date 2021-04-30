# this module will be imported in the into your flowgraph
import socket
import string
import sys

def jmf_server(tt):
	while True:
		sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
		sock.bind(('192.168.1.168',4242))
		print("Waiting for connection")
		sock.listen(1)
		conn,addr=sock.accept()
		with conn:
			print("connected from: ",addr)
			while True:
				data=conn.recv(1)
				if data:
					data=data.decode()
					print(data)
					if '+' in data:
						tt.f=tt.f+1e4
					if '-' in data:
						tt.f=tt.f-1e4
					tt.set_f(tt.f)
					if 'q' in data:
						sock.shutdown(socket.SHUT_RDWR)
						sock.close()
						break
