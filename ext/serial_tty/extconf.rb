require "mkmf"

abort "missing malloc()" unless have_func "malloc"
abort "missing free()"   unless have_func "free"

if RUBY_PLATFORM =~ /darwin/
  sdk_path = `xcrun --show-sdk-path`.strip
  framework_path = File.join(sdk_path, 'System/Library/Frameworks/ApplicationServices.framework')
  $LDFLAGS  << " -framework ApplicationServices"
end

create_header
create_makefile "serial_tty/serial_tty"