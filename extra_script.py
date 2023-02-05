import subprocess
from os import path, remove

Import("env")

git_tag = str(subprocess.check_output(
    "git rev-parse --short HEAD", shell=True))[2:9]

if path.isfile('TESTING_CI') or path.isfile('RELEASE_CI'):
    env.Replace(PROGNAME="firmware")
else:
    # access to global construction environment
    env.Replace(PROGNAME=f"firmware_{env['PIOENV']}_{git_tag}")


#esto no me agrada, pero si funca funca:
flash_config = "lib/w25qxx/w25qxxConf.h"
remove(flash_config)

config_file = open(flash_config,"w+")
config_file.write("""

#ifndef _W25QXXCONFIG_H
#define _W25QXXCONFIG_H

#define _W25QXX_SPI                   hspi2
#define _W25QXX_CS_GPIO               MEMORY_CS_GPIO_Port
#define _W25QXX_CS_PIN                MEMORY_CS_Pin
#define _W25QXX_USE_FREERTOS          0
#define _W25QXX_DEBUG                 0

#endif

""")
config_file.close()

# Add semihosting feature
env.Append(
    LINKFLAGS=["--specs=rdimon.specs"],
    LIBS=["rdimon"]
)


def skip_json_example_files(env, node):
    # Ignoring json lib example files
    return None


env.AddBuildMiddleware(skip_json_example_files, "*/json/docs/*")
