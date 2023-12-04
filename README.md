# Interaktivní test

Cílem tohoto projektu je vytvořit program pro vyplňování a vyhodnocování interaktivních testů. Jedná se o testy ze studentem zvoleného oboru podané interaktivní (a zábavnou) formou.

### Základní parametry programu:
1. Po spuštění programu bude uživatel vyzván k vyplnění jména a základních údajů (věk, pohlaví apod.), dále bude možno zvolit test z několika oborů.
2. Každý test bude obsahovat jiný způsob interakce s člověkem (např. zvolení z možností a,b,c,d či odpověď textem).
3. Všechny testy se budou načítat ze souboru a stejně tak jejich výsledky budou archivovány pro možnost následného vyhodnocení.
4. Po ukončení testu budou zobrazeny statistiky daného testu - například počet správně zodpovězených otázek, doba trvání apod.
5. Po každém vyplnění testu budou uložena statistická data, která se budou průměrovat ze všech testů ze stejné skupiny (dělených dle základních údajů apod).

__Bonusový úkol:__ Implementujte do testů jeden z následujících způsobů interakce: “hra riskuj”, odkrývání “tajné” informace správnými odpověďmi

***

## Fungování

Mohlo by to jít tak, že se udělá několik menu, ve kterých půjde vybírat šipkama a enterem (hlavní menu, výběr témat,...). Po navolení všech možností to z textovýho souboru nahraje otázky. Po zodpovězení všech otázek se vyhodnotí odpovědi a skóre se uloží do jinýho texťáku. \
Pak tam má být ještě cosi s tou statistikou, ale to už nějak půjde dožďuchat nakonec.

### Otázky:

Bude potřeba vůbec vymyslet nějaký otázky. Vymyslíme několik témat a ke každýmu třeba 10 otázek. Otázky pro daný téma budou uložený v souboru, ještě uvidíme jak. Ta hra pak může třeba náhodně vybrat 5-6 otázek pro danej test. Odpovědi budou buď přímo textem, kde se asi jen porovná, jestli je zadaná odpověď stejná jako ta správná uložená, nebo abcd. Tam to snad taky půjde jen nějakým _if_ porovnáním.

***

Pro začátek možná zkusím udělat nějakou zjednodušenou verzi, kde se půjde proklikat aspoň k jedné otázce.
