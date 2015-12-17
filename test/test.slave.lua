--

local thread = require "thread"
local trace = require "trace"
local zmq = require "zmq"

--

print(zmq.context():debug())
