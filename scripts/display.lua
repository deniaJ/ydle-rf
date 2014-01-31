--[[
for i=1,10 do
	createNewFrame(1,1,1)
	addDataToFrame(1,2)
	send()
end
]]--
function receive()
	print("Frame received")
	data_i = 0
	data_i = extractInt(1, data_i)
	data_f = 0
	data_f = extractFloat(0, data_f)
	print("Transported value : pos 0 : " .. data_i .. " pos 1: " .. data_f)
end
