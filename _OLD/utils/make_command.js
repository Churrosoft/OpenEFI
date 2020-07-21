var readline = require("readline-sync");

let mydata = [];

const uint16 = n => n & 0xffff;

const crc16 = data_p => {
  let crc = 0xffff;
  let x = 0x0;
  data_p.map(element => {
    x = uint16((crc >> 8) ^ element);
    x ^= uint16(x >> 4);
    crc = uint16((crc << 8) ^ (x << 12) ^ (x << 5) ^ x);
  });
  return crc;
};

const getData = txt => {
  let data = [];
  for (;;) {
    data = readline.question(txt);
    data = data.split(/[.,\/ -]/); //vos mandale cualquier verga que corta igual
    if (data.length > 1) {
      return data;
    } else {
      console.log("la informacion ingresada no es valido, intente otra vez");
    }
  }
};

getData("ingrese el comando:").map(e => {
  mydata.push(parseInt(e, 16));
});
getData("ingrese el sub-comando:").map(e => {
  mydata.push(parseInt(e, 16));
});
getData("ingrese el payload:").map(e => {
  mydata.push(parseInt(e, 16));
});
mydata.push(crc16(mydata));
console.log(mydata.toString("hex"));
