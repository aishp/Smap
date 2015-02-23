send= function(data)

        --hard-coded archiver ip
	archiver_ip="2001:470:4956:1::1"

	ipaddr = storm.os.getipaddr()
  
  	cport = 49152 -- random port to send
  	archiver_port=8003 -- pre-defined
  
  	csock = storm.net.udpsocket(cport, 
        
	function(payload, from, port)
	print (string.format("echo from %s port %d: %s",from,port,payload))
        end)
       
	local smap_msg={ 
                key= "abc", -- enter proper key
                UUID= "bc885b19-b3c1-11e4-b14c-28cfe91cf4ef",
                Readings={storm.os.now(storm.os.SHIFT_0), data},
               }
        
	to_send= storm.mp.pack(smap_msg)
        storm.net.sendto(csock, to_send, archiver_ip, archiver_port) 
        cord.await(storm.os.invokeLater, storm.os.SECOND) -- send data every second

end --end send()
