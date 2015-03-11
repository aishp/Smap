TEMP = require "temp"
temp = TEMP:new()
print("after temp:new")
cord.new(function ()
  temp:init()
  smap_obj= storm.n.smap_init("mykey", "2001:0470:4956:0002::0012:6d02:0000:301e", 7, 49152)
        while true do
  --    print("entered while loop")
        local temp_now= temp:getTemp()
        print("Temp now:"..temp_now)
        
	smap_obj:smap_send("bc885b19-b3c1-11e4-b14c-28cfe91cf4ef", temp_now)
        
        cord.await(storm.os.invokeLater, storm.os.SECOND)
       end -- end while loop
  end)-- end cord.await
		
cord.enter_loop()
