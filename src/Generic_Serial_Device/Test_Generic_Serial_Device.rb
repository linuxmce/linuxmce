require 'Ruby_Generic_Serial_Device'


class TestIOHandler < Ruby_Generic_Serial_Device::RubySerialIOWrapper 
    def testmtd()
        print "in TestIOHandler\n"
		p @conn	
		@conn.Open()
    end
end

c = TestIOHandler.new()
c.testmtd()

