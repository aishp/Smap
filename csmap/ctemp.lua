TEMP = require "temp"
temp = TEMP:new()
cord.new(function ()
  temp:init()
  smap_obj= storm.n.smap_init("mykey", "2001:0470:4956:1::1", 8003, 49152)
        while true do
  --    print("entered while loop")
        local temp_now= temp:getTemp()
	smap_obj:smap_send("u", temp_now)
        cord.await(storm.os.invokeLater, 2*storm.os.SECOND)
       end -- end while loop
  end)-- end cord.await
		
cord.enter_loop()
