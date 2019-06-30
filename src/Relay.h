// Relay.h

#ifndef _RELAY_h
#define _RELAY_h

//clase para manejar los relay por el demux (ademas del paso a paso)
class Relay {
public:
	Relay(byte DATA, byte CLK , byte CLR); //pines del 74HC164
	void Flip(byte relay);
	void Set(byte relay, byte state);
	void PaP(int grados);
private:
	void Sal(byte pos, byte state); //void para controlar todo el 74HC164
	byte _DATA, _CLK, _CLR;
	byte st[8] = { 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 };
	int pos_actual; //Posicion del PaP
};
#endif

