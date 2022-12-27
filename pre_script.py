
from os import path
from subprocess import check_output

print('-D TRACE')
print('-D USE_FULL_ASSERT')
print('-D _W25QXX_DEBUG')

command = "grep --include=\\*.{c,h,cpp,hpp} --exclude=speed_n.cpp -rnw './' -e 'speedN::calculate_injection_time()' | wc -l"
cmd_output = check_output(command, shell=True)
if(str(cmd_output).find('1')):
    print('-D speed_n_on_file')

if(path.isfile('TESTING_CI')):
    print('-D TESTING')
    print(' -D TRACE')
