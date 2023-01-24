Gem::Specification.new do |s|
  s.name        = "serial_tty"
  s.version     = "0.0.1"
  s.summary     = "Retrieve info about connected serial ports"
  s.description = "C extension to natively query connected USB devices and derive a tty name"
  s.authors     = ["Andy Clink"]
  s.email       = "andy@aravaiparunning.com"
  s.files       = ['lib/serial_tty.rb']
  s.files      += Dir['ext/serial_tty/**.c']
  s.homepage    = "http://github.com/aravaiparunning/serial_tty"
  s.license     = "GPL-3.0"
  s.extensions = %w[ext/serial_tty/extconf.rb]
end