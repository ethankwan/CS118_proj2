NAME: Ethan Kwan
UID: 004899710

Design:
Non-persistent HTTP/1.1 protocol local server that supports the transmission of .jpg, .gif, .html, and binary files. 

Problems:
At first, due to the abrupt restarting of my server, my address was being blocked when I tried to bind the socket. To solve this, I used getsockopt() to set theREUSE_ADDR option for my socket, allowing the connection to go through. 

Then once my connection was established, I could not properly read the HTTP requests because I was not properly managing the memory of my buffer. 

For part B, my responses were not properly displaying to my broswer. However, I realized that Chrome sends additional HTTP requests which was affecting the socket communication between the client and server. Once I switched to Mozilla, the contents of the test files were correctly displayed to browser. 
