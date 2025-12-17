function checktable(t)
	local re1 = 0
	local re2 = 0
	
	for k, v in pairs(t) do 
		if(k == "pos") then
			for i, v2 in pairs(v) do 
				re1 = re1 + v2
			end
		end
		
		if(k == "items") then
			for i, v2 in ipairs(v) do 
				re2 = re2 + 1
			end
		end
			
	end
	
	return re1,re2
end