function updatetable(t)
	length = 0
	for i, v in ipairs(t) do 
		length = length + 1
	end
	table.insert(t,length + 1,{level = 10});
	return t
end