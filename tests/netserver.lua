require 'netserver'

local t = { connect = function(s)
end,
disconnect = function(s)
end,
newdata = function(s, d)
end
}

status, err = netserver.open(4000, t)

while true do
netserver.check(100)
end
