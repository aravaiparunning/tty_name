require 'serial_tty'

name = SerialTTY.tty_name(4292, 60000)
puts "TTY: #{name.inspect}" unless name.nil?
