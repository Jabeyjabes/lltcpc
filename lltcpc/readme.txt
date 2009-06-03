Low Level TCP/IP Client (MS Winows)

This is simple win console application, for my study TCP/IP sockets under MS Windows OS.

It will (I hope) send any data to specified IP address and port of remote server, and wait responce.

It will have two work mode:
1. (Default) Silent - without any message to stdout, exclusion server response.
2. With information about sended, received data and sumary in bytes.

Command line usage:
lltcpc [-m mode] [-t receive_timeout] <-a address> <-p port> [-d send_data| -f send_file]

mode: 0|1 0 - silent (default) in the stdout input server's responce in HEX bytes
          1 - show all info about send and received operation, for example:

receive_timeout: mC for waiting recponse from remote server.

sended 12 bytes to address 127.0.0.1 port 502:
0x00 0x00 0x00 0x00 0x00 0x06 0x01 0x03 0x00 0x00 0x00 0x01
receved 11 bytes:
0x00 0x00 0x00 0x00 0x00 0x05 0x01 0x03 0x02 0x40 0x49

address: the server name or his IP.

port: port of server.

send_data: the HEX bytes to send the server

send_file: (under construction - may be restricted by "cin" ex: <filename)