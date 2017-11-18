README
Kat Herring (kherring)
Usage: ./udpclient <server host name> <port number> <file/string>

To compile this code the following command was used:
gcc -W -o udpclient udpclient.c

An example of running this code with a file:
./udpclient student06.cse.nd.edu 41108 File1.txt

Sample result of call made above:
Decrypted Message: Welcome to Computer Networks! Enjoy Network Programming.
 Timestamp: 0:33:12.176338
Round Trip Time (RTT): 462266 microseconds

Can also be called with a random string in place of the file name.
