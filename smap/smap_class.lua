local SMAP = {}

function SMAP:init(data_key, ip, port)

	obj= {  key=data_key,
        	archiver_ip= ip,
		archiver_port= port,
                cport = 49152, -- random port to send
                
  		csock = storm.net.udpsocket(cport, 
		function(payload, from, port)
--		print (string.format("echo from %s port %d: %s",from,port,payload))
        	end),
                smap_msg.key=key,
                smap_msg.UUID=nil,
                smap_msg.Readings=nil,
             }
      
	setmetatable(obj,self)
	self._index=self
	return obj
end

function SMAP:create_msg(uuid,data)

	self.smap_msg={ 
                key= self.key, -- enter proper key
                UUID= uuid,
                Readings={storm.os.now(storm.os.SHIFT_0), data},
               }
  
	to_send= storm.mp.pack(self.smap_msg)
        return to_send
end


function SMAP:send(uuid, data)

        local id=uuid
        local d=data
        to_send=create_msg(id,d)
        storm.net.sendto(csock, to_send, self.archiver_ip, self.archiver_port) 
        cord.await(storm.os.invokeLater, storm.os.SECOND) -- send data every second

end --end send()


--[[ no way to generate uuid as of now
function SMAP:send_new(data)

--generate uuid before sending
  	
        local uuid
        storm.n.gen_uuid(uuid)
       
        local d=data
	to_send= create_msg(uuid,d)
        storm.net.sendto(csock, to_send, self.archiver_ip, self.archiver_port) 
        cord.await(storm.os.invokeLater, storm.os.SECOND) -- send data every second

end --end send()]]--

