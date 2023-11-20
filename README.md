# Projekt ISA
### Autor: Adam Nieslanik
### Login: xniesl00
### 20.11.2023

### Popis programu

Napište program dns, který bude umět zasílat dotazy na DNS servery a v čitelné podobě vypisovat přijaté odpovědi na standardní výstup. Sestavení a analýza DNS paketů musí být implementována přímo v programu dns. Stačí uvažovat pouze komunikaci pomocí UDP.

### Spuštění

<b>./dns [-r] [-x] [-6] -s server [-p port] adresa<b>

*	-r Volitelný přepínač, který určuje, že je vyžadovaná rekurze.
*	-x Volitelný přepínač, který určuje, že se má zaslat reverzní dotaz
*	-6 Volitelný přepínač, který určuje zaslání dotazu typu AAAA. (výchozí A)
*	-s server IP adresa nebo doménové jméno serveru, kam se má zaslat dotaz.
*	-p port Volitelný přepínač, který určuje, číslo portu na který se má poslat dotaz. (výchozí 53)
*	adresa dotazovaná adresa
*	-h Vypíše nápovědu.

### Nedostatky
* Mezi hlavní nedostatky implementace patří nefunkční konverze IPv6 adresy pomocí funkce expandIPv6()

### Seznam odevzdaných souborů v adresáří xniesl00.tar
* Makefile
* dns.cpp
* dns.hpp
* manual.pdf
* README.md