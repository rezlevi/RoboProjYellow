# Robotika Sárga Csapat dokumentáció

# Téma: Mozgásérzékelő riasztó

### Cél

Egy kóddal működtethető riasztórendszer modellezése, mely készenléti állapotból mozgásra vagy fényre riasztási állapotba kerülhet.
Bekapcsolt rendszer esetén a készenléti állapot egy kód beírása utáni gomb lenyomással aktiválható/deaktiválható, a riasztási állapot a szenzorokon keresztül aktiválható,
egy kód beírása utáni gomb lenyomásával pedig deaktiválható. Hogy milyen állapotban van éppen a riasztó berendezés
(kikapcsolt, bekapcsolt, de nem készenléti, bekapcsolt és készenléti, riasztási, lezárt riasztási), azt egy lámpa működési jelzi, mely a projekt során egy RGB LED lesz.
Egy lezárási funkció - hogy éppen zárt-e az általunk védett terület/objektum - szervomotorral lesz megvalósítva.

### Alkatrészlista

1. 1 db mikrokontroller (irányításhoz)
2. 1 db kijelző (tudjuk rajta követni a kódot)
3. 2 db nyomógom (egyik a kód beírás utánra, másik a ki- és bekapcsoláshoz)
4. 1 db potméter (ezzel állítjuk be a kódot)
5. 1 db RGB LED (jelzi, hogy milyen állapotban van a rendszerünk)
6. 1 db szervomotor (zárás szimulációjához)
7. 2 db szenzor, egy fény-, illetve mozgásérzékelő (a riasztás kiváltásához)

### Állapotok

1. Kikapcsolt
		- A bekapcsológomb aktív, jelre vár
		- A LED nem világít
		- Kijelző üres
		- Potméter inaktív
		- Kódgomb inaktív
		- Szervo nyitott állásban
		- Szenzorok inaktívak
2. Bekapcsolt, nem készenéti
		- A bekapcsológomb aktív, jelre vár
		- A LED zölden világít
		- Kijelzőn kezdetben 0, tekerés után a potméter által meghatározott szám
		- Kódgomb aktív
		- Szervo nyitott állásban
		- Szenzorok inaktívak
3. Bekapcsolt, készenléti
		- A bekapcsológomb nem aktív
		- A LED vörösen világít
		- Kijelzőn kezdetben 0, tekerés után a potméter által meghatározott szám
		- Kódgomb aktív
		- Szervo zárt állásban
		- Szenzorok aktívak
4. Riasztási:
		- A bekapcsológomb inaktív
		- A LED vörösen villog
		- ijelzőn kezdetben 0, tekerés után a potméter által meghatározott szám
		- dgomb aktív
		- Szervo zárt állásban
		- Szenzorok inaktívak
		- Belső hibaszámláló: Kezdetben 0, hibás kód esetén növekszik 1-gyel
		- Lezárt riasztási: Ugyanaz, mint a riasztási, csak belső hibaszámláló nélkül

### A rendszer működése/állapotátmenetek

Kezdetben a rendszer kikapcsolt állapotban van, melyet a bekapcsológomb segítségével hozhatunk bekapcsolt, de nem készenléti állapotba. 
Bekapcsolt, de nem készenléti állapotból a bekapcsológomb lenyomásával ismét kikapcsolt állapotba kerülhetünk. A kódgomb lenyomása után - amennyiben megfelelő kód szerepel a kijelzőn - készenléti állapotba kerülünk, amennyiben a kód nem megfelelő, nem történik semmi.
Bekapcsolt készenléti állapot esetén a kódgomb lenyomásával - megfelelő kód esetén - visszajutunk nem készenléti állapotba, nem megfelelő kód esetén nem történik semmi. Bármely szenzor jelzése riasztási állapotba viszi a rendszert.
Riasztási állapot esetén a kódgomb lenyomásával - megfelelő kód esetén - visszajutunk nem készenléti állapotba, azonban háromszori rossz kódnál történő gombnyomás után lezárt riasztási állapotba kerülünk.Lezárt riasztási állapotból a rendszer nem vihető ki, csak áramtalanítás segítségével.

### Állapotátmenet szemléltetése gráffal

![h](https://github.com/rezlevi/RoboProjYellow/assets/113850216/bb5b0300-71f9-4020-a772-07e9ca7eb989)
