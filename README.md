# delay-calculation-system

A TCP communication is established between a client with dynamically assigned Port number and a main server with static Port number assigned to it. The host name is hardcoded to "127.0.0.1" for all instances. A UDP communication is also established from the main server to the database and the calculation servers with statically assigned Port numbers for each server. The client sends the id and size of the file to the main server, which is then passed on to the database server for searching in the database.txt file. If found, it is further sent to the calculation server for calculating the transmission delay, propagation delay and total delay. If not found in the database, then it sends an appropriate message accordingly. A separate monitor server is connected to the main server using TCP, which monitors all the transactions happening in Main Server.

# monitor.cpp
   This file tracks all the details received to or sent from the main server using a TCP connection. It listens for all the communications happening between the mainServer and the client, the mainServer and the dbServer, and the mainServer and the calcServer. Since it listens for all of them, that is why it has to be executed in the beginning, before executing any of the files.

# mainServer.cpp
   This file is responsible for socket creation, binding, listening and accepting connection from the client, and closing the connection and socket connection to the client after the job is done. It receives the id and size from the client. Then, this id is sent to database server for checking in the database. If any data is sent from databaseServer.cpp, it is then sent to calcServer.cpp along with the data from databaseServer.cpp and the size from client.cpp for further calculation. The result, then received from calcServer.cpp, is displayed on mainServer.cpp and is also sent back to the client.

# dbServer.cpp
   This file is responsible for checking if the given ID exists in the text file(database.txt) and send out the required information upon successfully matching the IDs or send a message notifying it is not found.

# calcServer.cpp
   This file is responsible for calculating the transmission, propagation and total delay from the size given by the client and the corresponding data from the database. If the database returns a "no match found" label, this file does not run at all.

# client.cpp
   This file is responsible for socket creation and making connection from client, and closing the socket connection after the job is done. Once all the files are executed, this file is executed. It, then, sends id and file size to the server during execution and receives a reply from the main server with the appropriate message or result.

# The boot sequence will be monitor.cpp, mainServer.cpp, dbServer.cpp, calcServer.cpp, client.cpp

 Format of the messages exchanged:-
  
   client
   
      The Client is up and running.
      Sent Link 1 and file size 100MB to main server.
      Received transmission delay 800.00ms, propagation delay 0.33ms, and total delay 800.33ms.

   mainServer
      
      The Main Server is up and running.
      Received link 1, file size 100MB.
      Sent Link 1 to database server.
      Received link capacity 1000Mbps, link length 100km, and propagation velocity 300000km/s.
      Sent information to calculation server.
      Received transmission delay 800.00ms, propagation delay 0.33ms, and total delay 800.33ms.

   dbServer
   
      The Database Server is up and running.
      Received request from Main Server.
      Sent link 1, capacity 1000Mbps, link length 100km, propagation velocity 300000km/s.

   calcServer
   
      The Calculation Server is up and running.
      Received request from Main Server.
      Sent transmission delay 800.00ms, propagation delay 0.33 ms, total delay 800.33ms.

   monitor
   
      The Monitor is up and running.
      Main server receives Link 1 and file size 100MB from client.
      Main server sends Link 1 to database server.
      Main server receives information from database server: link capacity 1000 Mbps, link length 100km, and propagation velocity 300000km/s.
      Main Server sends information to calculation server.
      Main Server receives information from calculation server.
      Main Server sends information to client: transmission delay 800.00ms, propagation delay 0.33ms, total delay 800.33ms.
