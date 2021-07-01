import re
import sys

rgx= '.+(:PASS|:FAIL:)?.+'

data = open('qemu_output.log', "r+")
report = open('qemu_report.xml', 'a')

lines = data.readlines()

pattern = re.compile(r"^.+(:PASS|:FAIL)")

successs_test = []
failed_test = []

for i, line in enumerate(lines):
    for match in re.findall(pattern, line):
        if match == ':PASS':
            successs_test.append(lines[i].rstrip())
        if match == ':FAIL':
            failed_test.append(lines[i].rstrip())

total_test = len(successs_test) + len(failed_test)
failures = len(failed_test)

report.write('<?xml version="1.0" encoding="UTF-8"?>')
report.write(f"""
<testsuites 
    disabled="0"
    errors="0" 
    failures="{failures}"
    name="OpenEFI Testing results"
    tests="{total_test}"
    time="15s"
> 
  <testsuite name="openefi" errors="0" failures="0" skipped="0" time="2" tests="0">
""")


if len(failed_test):
    for test_case in failed_test:
        test_data = test_case.split(':')
        report.write(f""" 
    <testcase name="{test_data[2]}"
	    assertions="1"
	    classname="{test_data[2]}"
	    status="{test_data[3]}"
	    time="1s"
    >
        <failure
            message="{test_data[4]}"
	        type="FULL ASSERT" 
	    >
        FAIL: {test_data[4]}
        {test_data[2]} on file {test_data[0]} , on line {test_data[1]}
        </failure>
    </testcase>
""")

if len(successs_test):
    for test_case in successs_test:
        test_data = test_case.split(':')
        report.write(f""" 
    <testcase name="{test_data[2]}"
	    assertions="1"
	    classname="{test_data[2]}"
	    status="{test_data[3]}"
	    time="1s"
    >
    </testcase>
""")

report.write('</testsuite>')
report.write('</testsuites>')
report.close()