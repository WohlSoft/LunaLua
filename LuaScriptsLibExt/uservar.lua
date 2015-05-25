uservar = setmetatable({}, {

	__newindex = function (tarTable, key, value)
		UserData.setValue(key, tonumber(value))
	end,

	__index = function (tarTable, key)
		local value = UserData.getValue(key)
		if(value)then
			return value
		end
		return 0
	end
})
return uservar