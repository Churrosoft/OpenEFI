// 
// clase para manejar los relay por el demux (ademas del paso a paso)
// 

#include "Relay.h"

Relay::Relay( byte DATA, byte CLK, byte CLR ){
	_DATA = DATA;
	_CLK  = CLK;
	_CLR  = CLR;
}

void Relay::Flip( byte relay ){
	Relay::Sal( st[relay], !st[relay] );
}

void Relay::Set( byte relay, byte state) {
	Relay::Sal(st[relay], state );
}

void Relay::PaP( int grados ){
}

void Relay::Sal( byte pos, byte state ){

	digitalWrite(_CLR, HIGH);
	delayMicroseconds(6);
	digitalWrite(_CLR,LOW);
	st[pos] = state;

	for (size_t i = 0; i < 8; i++){

		digitalWrite(_DATA, st[i] );
		digitalWrite(_CLK, HIGH);
		delayMicroseconds(2);
		digitalWrite(_CLK, LOW);
	}
}
