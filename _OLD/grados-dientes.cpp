//convierte grados en dientes del sensor hall
int dientes(float grados){
    float grad = 360 / dnt; //dividimos 360 grados por la cantidad de dientes
    //dividimos grados por grad, luego multiplicamos por 100 para transformar el float en int
    int x2     = (grados / grad) * 100; 
    //dividimos por 100, al hacer esto se eliminan los decimales, en prox ver redondear
    int dnt2    = x2 / 100;
    return dnt2; 
}
