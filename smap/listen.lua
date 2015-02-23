
cord=require "cord"
--LED=require("contrib/lib/led")

-- create echo server as handler
server = function()
   ssock = storm.net.udpsocket(7, -- create a socket on port 7 (server is listening on port 7)
			       function(payload, from, port) -- from: src ip, port- src port
				 
				  print (string.format("from %s port %d: %s",from,port,payload))
				  print(storm.net.sendto(ssock, payload, from, cport))
	
			       end)
end

server()

	-- enable a shell
sh = require "stormsh"
sh.start()
cord.enter_loop() -- start event/sleep loop		-- every node runs the echo server
