TEMP = require "temp"
temp = TEMP:new()
print("after temp:new")
cord.new(function ()
  
  
  temp:init()
--print("after init")  
--  archiver_ip="2001:470:4956:1::5"    
archiver_ip="2001:470:4956:1::1"
  ipaddr = storm.os.getipaddr()
  ipaddrs = string.format("%02x%02x:%02x%02x:%02x%02x:%02x%02x::%02x%02x:%02x%02x:%02x%02x:%02x%02x",
			ipaddr[0],
			ipaddr[1],ipaddr[2],ipaddr[3],ipaddr[4],
			ipaddr[5],ipaddr[6],ipaddr[7],ipaddr[8],	
			ipaddr[9],ipaddr[10],ipaddr[11],ipaddr[12],
			ipaddr[13],ipaddr[14],ipaddr[15])

  print("ip addr", ipaddrs)
  --print("node id", storm.os.nodeid())
  cport = 49152
  archiver_port=8003
 -- sport = 49153
  
  csock = storm.net.udpsocket(cport, 
			    function(payload, from, port)
			       --in case archiver responds
			       print (string.format("echo from %s port %d: %s",from,port,payload))
			    end)

        while true do
  --    print("entered while loop")
        local temp_now= temp:getTemp()
        print("Temp now:"..temp_now)
        
local smap_msg={ 
                key= "abc",
                UUID= "bc885b19-b3c1-11e4-b14c-28cfe91cf4ef",
                Readings={1000, temp_now},
               }
print(smap_msg.UUID)
print(smap_msg.Readings[1])
print(smap_msg.Readings[2])
--print(smap_msg.value) 
       -- print("created smap_msg")     
        to_send= storm.mp.pack(smap_msg)
       -- print("Created and packed smap msg")
        storm.net.sendto(csock, to_send, archiver_ip, archiver_port) 
        
        cord.await(storm.os.invokeLater, 500 * storm.os.MILLISECOND)
       end -- end while loop
  end)-- end cord.await
		
cord.enter_loop()
