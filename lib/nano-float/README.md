# nano-float: Tiny floating point library for Arm Cortex-M0 and M3

- Direct replacement for standard C `<math.h>` and `libm.a` functions

- Based on qfplib: https://github.com/lupyuen/qfplib

- Math operations on `double` like the code below can also be converted to use `nano-float`:

    ```c
    double x = 2205.1969;
    double y = 270.8886; 
    double r = x / y;
    ```

- `gcc` compiles the code above into `__aeabi_dmul(x, y)`. Add the option `-Wl,-wrap,__aeabi_dmul` to `gcc` so that `__aeabi_dmul` becomes wrapped as `__wrap___aeabi_dmul`, which is defined in `nano-float`.

- See this for the complete list of wrap options: https://github.com/lupyuen/newlib/blob/master/CMakeLists.txt.  The `aeabi` functions are defined in http://infocenter.arm.com/help/topic/com.arm.doc.ihi0043d/IHI0043D_rtabi.pdf.

- Warning: There will be loss of accuracy and deviations from IEEE standards. We will be computing in single-precision floats rather than double-precision.  Accuracy is roughly 6 significant decimal digits. See https://www.quinapalus.com/qfplib.html
