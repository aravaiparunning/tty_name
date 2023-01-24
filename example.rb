require 'serial_tty'

puts "Checking for a TTY matching 4292 60000"
name = SerialTTY.tty_name(4292, 60000)
puts "TTY: #{name.inspect}"
