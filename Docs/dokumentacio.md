# Téma: Riasztórendszer

### Cél

Egy kóddal működtethető riasztórendszer modellezése, mely készenléti állapotból fényviszony változásra riasztási állapotba kerülhet.
Bekapcsolt rendszer esetén a készenléti állapot egy kód beírása utáni gomb lenyomással aktiválható/deaktiválható, a riasztási állapot a szenzorokon keresztül aktiválható,
egy kód beírása utáni gomb lenyomásával pedig deaktiválható. Hogy milyen állapotban van éppen a riasztó berendezés
(kikapcsolt, bekapcsolt, de nem készenléti, bekapcsolt és készenléti, riasztási, lezárt riasztási), azt egy lámpa működési -esetenként pedig hangszóró - jelzi.
Egy lezárási funkció - hogy láthassuk, éppen zárt-e az általunk védett terület/objektum - szervomotorral lesz megvalósítva.

### Alkatrészlista

- 1 db mikrokontroller (irányításhoz)
- 1 db kijelző (tudjuk rajta követni a kódot)
- 2 db nyomógomb (egyik a kód beírás utánra, másik a ki- és bekapcsoláshoz)
- 1 db potméter (ezzel állítjuk be a kódot)
- 2 db LED, egy zöld és eg piros (jelzik, hogy milyen állapotban van a rendszerünk)
- 1 db szervomotor (zárás szimulációjához)
- 1 db szenzor, egy fényérzékelő (a riasztás kiváltásához)
- 1 db hangszóró
- 1 db RFID

### Állapotok

1. Kikapcsolt
	- Egyik LED sem világít
	- Kijelző üres, sötét
	- Hangszóró néma
	- Szervo nyitott állásban
2. Bekapcsolt, nem készenéti
	- A zöld LED világít
	- Kijelző aktív
	- Hangszóró néma
	- Szervo nyitott állásban
3. Bekapcsolt, készenléti
	- A piros LED világít
	- Kijelző aktív
	- Hangszóró néma
	- Szervo zárt állásban
	- Szenzor "figyel"
4. Riasztási:
	- A piros LED villn hármat, majd világít
	- Kijelző aktív
	- Hangszóró néma
	- Szervo nyitott állásban
5. Lezárt riasztási:
	- A piros LED világít
	- Kijelzőn "Closed!" felirat
	- Hangszóró aktív
	- Szervo zárt állásban

### A rendszer működése/állapotátmenetek

- Kezdetben a rendszer kikapcsolt állapotban van, melyet a bekapcsológomb segítségével hozhatunk bekapcsolt, de nem készenléti állapotba. 
- Bekapcsolt, de nem készenléti állapotból a bekapcsológomb lenyomásával ismét kikapcsolt állapotba kerülhetünk. A kódgomb lenyomása után - amennyiben megfelelő kód szerepel a kijelzőn - készenléti állapotba kerülünk, amennyiben a kód nem megfelelő, nem történik semmi.
- Bekapcsolt készenléti állapot esetén a kódgomb lenyomásával - megfelelő kód esetén - visszajutunk nem készenléti állapotba, nem megfelelő kód esetén nem történik semmi. Bármely szenzor jelzése riasztási állapotba viszi a rendszert.
- Riasztási állapot esetén a kódgomb lenyomásával - megfelelő kód esetén - visszajutunk nem készenléti állapotba, azonban háromszori rossz kódnál történő gombnyomás után lezárt riasztási állapotba kerülünk. Továbbá 30 másodperc elteltével szintén lezárt riasztási állapotba kerülünk.
- Lezárt riasztási állapotból a rendszer nem vihető ki, csak RFID használatával. Ekkor ebből kikapcsolt állapotba kerülünk.

### Állapotátmenet szemléltetése gráffal

Megjegyzés: Az eredeti tervhez képest módosult, hogy nincs mozgásérzékelő, illetve nem csak három rossz kód viszi lezárt
riasztási állapotba a rendszert, hanem az is, ha a riasztási állapotból nem visszük ki 30 másodpercen belül.
Ezen változások az állapotátmenet gráfban nincsenek benne.

![h](https://github.com/rezlevi/RoboProjYellow/assets/113850216/bb5b0300-71f9-4020-a772-07e9ca7eb989)
