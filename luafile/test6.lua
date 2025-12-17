function sumtable(a)
	local sum_ = 0
	for i, v in ipairs(a) do
		sum_ = sum_ + v
	end
	return sum_
end