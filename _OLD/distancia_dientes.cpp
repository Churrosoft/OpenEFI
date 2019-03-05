float distanciaEntreDientes(int revolucionesPM){
    //ESTA FUNCIÓN DEVUELVE LA DISTANCIA ENTRE DIENTES EN MILISEGUNDOS
    float calculo = ((rpm/60) * (dnt)) / 1000;
   
    //dividimos las rpm en 60 segundos
    //lo que nos da las vueltas por segundo, lo multiplicamos por dientes del volante
    //para que nos diga cuantos dientes pasan en un segundo. Lo dividimos por 1000
    //para que nos diga cuantos dientes pasan por milisegundo
    //al hacer "1/calculo" nos da la distancia entre dientes en milisegundos
   
    return (1/calculo);//SE DEVUELVE LA DISTANCIA ENTRE DIENTES EN MS
}