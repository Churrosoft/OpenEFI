#if (dev == 1) //si se activo la depuracion
int freeRam () { //funcion para saber la ram disponible
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif