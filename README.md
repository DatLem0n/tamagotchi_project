# Tamagotchi
>
> **Ryhmä 3430**  
Matias Björklund  
Ville Kujala  
Kalle Asmundi

## TODO

### Pakolliset

- [ ]  Jupiter-notebook algoritmien protoiluun
- [ ]  MPU-datan käsittely
  - [ ]  Testidatan keräys
  - [ ]  Sensoridatan siivoaminen
    - [ ]  esim. liukuva keskiarvo, liian pienien ja suurien mittausten poisto
  - [ ]  Eleiden tunnistus datasta
    - [ ]  Ainakin pöydällä pompottelu koska helppo tunnistaa
    - [ ]  1-2 lisää elettä?
- [ ]  Nukkuminen: kun valosensori tunnistaa pimeyden, soitetaan tuutulaulua
- [ ]  Exercise: Pompottelu pöydällä kutsuu Exercise() ja soittaa Doom?
- [ ]  Pet: Valosensorilla tunnistetaan kun käden varjo pyyhkii laitteen yli?
- [ ]  Eat: ????
- [ ]  `id,BEEP` viestien käsittely
  - ohjeet: <https://github.com/UniOulu-Ubicomp-Programming-Courses/jtkj-sensortag-gateway#sending-messages-from-the-gateway>
  - [ ]  Jotakin tekemistä molemmille napeille ja LEDille

### Bonus

- [ ] Viestitaskin muokkaus niin että bufferin tyhjyyden sijaan käytetään tiloja `{BUFFER_EMPTY, BUFFER_NOT_EMPTY, BUFFER_FULL}`
  - Määrittele `enum MsgBufferState`
  - Luo siitä muuttuja `msgBufferState`
  - Anna muuttuja parametrina funktioille että ne voivat muuttaa sen tilaa kirjoittaessaan messageBufferiin
  - Kun uartTask lähettää ja tyhjentää bufferin, aseta `msgBufferState = BUFFER_EMPTY`
- [ ]  `MSG1` ja `MSG2`-kentille jotakin järkevää lähetettävää, esim. sensoreilta

## Sensorit

- [Paine ja lämpötila: BMP280](sensors/bmp280.c)
- [Lämpötila ja ilmankosteus: HDC1000](sensors/hdc1000.c) ei käytössä
- [Gyro ja kiihtyvyys: mpu9250](sensors/mpu9250.c)
- [Valoisuus: opt3001](sensors/opt3001.c)
- [IR lämpötila: tmp007](sensors/tmp007.c) ei käytössä

## Vaatimukset

- Käyttöliittymä
  - Painonapit, ledit ja äänimerkit
- Ohjauskomennot
  - MPU9250 datankeräys ja tulkinta
- Viesti taustajärjestelmälle
- Äänimerkki tai ledi kun komento tunnistettu
- Viestien vastaanotto taustajärjestelmältä
- Tilakone ja tehtäväpohjaisuus

## Suunnitelma

### Backend

1. Käyttäjätunnuksen rekisteröinti ja Tamagotchi luonti
1. UART yhteyden luominen
1. `EAT, EXERCISE, PET, ACTIVATE` komentojen lähetys
1. `id,BEEP` viestin vastaanotto
1. bonus: 2 omaa tietoa, esim. valosensorin ja lämpötilasensorin tulkinta
1. Raakadatan lähetys csv-muodossa

### Anturidata

#### Liikkeentunnistus

1. Käytetään datarakennetta `struct acceleration_events{bool positive_x, bool negative_x, bool positive_y ...}`
    1. Kerätään dataa sensoreilta sopivan hitaasti ja lasketaan ajanhetkien välinen muutos.
    2. Jos muutos ylittää kynnysarvon, vaihdetaan positiiviseksi muutoksen akselia ja suuntaa vastaavat Booleanit.
    3. Nukutaan hetki ja nollataan booleanit.
2. Päätellään `acceleration_events` arvoista ja senhetkisestä sensorilukemista millainen liike on menossa
3. Liikkeen loputtua lisätään viesti jonoon
4. Bonus: Virherajojen käyttö

### Tilakone ja taskit

1. Koodin jakaminen itsenäisiin taskeihin
2. main-looppiin enimmäkseen tilakoneen hallintaa

### Napit, ledit ja ääni

1. Virtapainike
2. Jokaiselle tunnistetulle toiminnolle oma äänimerkki ja/tai ledin vilkutus
3. Bonus: musiikki

## Projektin kääntäminen virtuaalikoneessa

### Jaa tarvittavat kansiot Virtualboxille

1. [Lataa tyhjä projekti johonkin kansioon.](https://github.com/UniOulu-Ubicomp-Programming-Courses/jtkj-sensortag-examples/blob/main/emptyProjects/empty_CC2650STK_TI_2023.zip)
1. Mene VirtualBox/Settings/Shared Folders/ ja lisää edellisen kohdan kansio
1. Lisää myös paikallisen git-repon kansio ja ruksita auto-mount
1. Aja VM:n terminaalissa komento `sudo usermod -aG vboxsf $USER`, salasana 1234

### Tuo koodi VM:n Code Composer Studioon

1. Avaa VM
1. Käynnistä CCS ja mene yläreunassa Projects/Import CCS Projects
1. Ruksita "Copy projects into workspace" ja importtaa tyhjä projekti
1. Raahaa git-repon tiedostot CCS projektiin ja "Overwrite all"

### Sensortagin debugaus

1. Jos olet muuttanut koodia: raahaa git-repon tiedostot uudestaan, Overwrite all, Rebuild project
2. Kiinnitä USB-johto
3. Mene VirtualBoxin yläreunassa Devices/USB/ ja varmista että Sensortagin kohdalla on ruksi.
4. Paina Debug-nappia
5. Jos haluat nähdä UART-liikenteen: Paina Crtl-Shift-Alt-T, valitse Serial Terminal. Serial portiksi "/dev/tty/ACM0" ja Baud Rateksi 9600
