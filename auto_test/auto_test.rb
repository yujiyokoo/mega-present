#
#
#

if ARGV.count == 0 then
  puts ''
  puts 'Compare the results from mruby and mruby/c.'
  puts 'Usage: auto_test.rb <file.rb>'
  puts 'file.rb = "xxxx.rb" or "*.rb"'
  puts 'Place .mrb file in the same folder of .rb file.'
  exit
end


$mruby_exe = '../../mruby/bin/mruby'
$mrbc_exe = '../../mruby/bin/mrbc'
$mrubyc_exe = '../sample_c/mrubyc'

unless File.exists?($mruby_exe) then
  puts "mruby.exe: '#{$mruby_exe}' not found."
  exit
end

unless File.exists?($mrbc_exe) then
  puts "mrbc.exe: '#{$mrbc_exe}' not found."
  exit
end

unless File.exists?($mrubyc_exe) then
  puts "mrubyc.exe: '#{$mrubyc_exe}' not found."
  exit
end

ARGV.each do |file|
  mrb_file = file.gsub('.rb', '.mrb')

  unless File.exists?(mrb_file) then
    `#{$mrbc_exe} -E #{file}`
  end

  if File.exists?(mrb_file) then
    result_mruby = `#{$mruby_exe} #{file}`
    result_mrubyc = `#{$mrubyc_exe} #{mrb_file}`

    if result_mruby == result_mrubyc then
      puts "Success: #{file}"
    else
      puts "Fail: #{file}"
      puts "=====mruby====="
      puts result_mruby
      puts "=====mruby/c====="
      puts result_mrubyc
    end
  end
end
