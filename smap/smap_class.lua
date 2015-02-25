require ("math")
local SMAP = {}

function SMAP:init(data_key, ip, port)

	obj= {  key=data_key,
        	archiver_ip= ip,
		archiver_port= port,
                cport = 49152, -- random port to send
             }
      
	setmetatable(obj,self)
	self._index=self
	return obj
end


function SMAP:send(uuid, data)

  	csock = storm.net.udpsocket(cport, 
	function(payload, from, port)
--	print (string.format("echo from %s port %d: %s",from,port,payload))
        end)
       
	local smap_msg={ 
                key= self.key, -- enter proper key
                UUID= uuid,
                Readings={storm.os.now(storm.os.SHIFT_0), data},
               }
        
	to_send= storm.mp.pack(smap_msg)
        storm.net.sendto(csock, to_send, self.archiver_ip, self.archiver_port) 
        cord.await(storm.os.invokeLater, storm.os.SECOND) -- send data every second

end --end send()

function SMAP:send_new(data)

--generate uuid before sending
  	

	csock = storm.net.udpsocket(self.cport, 
	function(payload, from, port)
	--print (string.format("echo from %s port %d: %s",from,port,payload))
        end)
       
	local smap_msg={ 
                key= self.key,
                UUID= uuid,
                Readings={storm.os.now(storm.os.SHIFT_0), data},
               }
        
	to_send= storm.mp.pack(smap_msg)
        storm.net.sendto(csock, to_send, self.archiver_ip, self.archiver_port) 
        cord.await(storm.os.invokeLater, storm.os.SECOND) -- send data every second

end --end send()

