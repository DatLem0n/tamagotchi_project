# Tamagotchi
> **Ryhmä 3430**  
Matias Björklund  
Ville Kujala  
Kalle Asmundi

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
2. UART yhteyden luominen
2. `EAT, EXERCISE, PET, ACTIVATE` komentojen lähetys
3. `id,BEEP` viestin vastaanotto
4. bonus: 2 omaa tietoa, esim. valosensorin ja lämpötilasensorin tulkinta
5. Raakadatan lähetys csv-muodossa

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
