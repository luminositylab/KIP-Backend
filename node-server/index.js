const i2c = require('i2c-bus');
const ADDR = 0x08;
console.log("test");
const i2c1 = i2c.openSync(1);
//for(var i = 0; i < 5; i++){
	i2c1.writeByteSync(ADDR, 255, 0);
	i2c1.writeByteSync(ADDR, 0, 10)
//}
console.log("done.");
i2c1.closeSync();

