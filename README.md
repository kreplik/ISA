# ISA
## Funkce:
### void printInfo(DNS_HEADER *dns):

Popis: Vypisuje informace o DNS hlavičce.

Parametry: 

* dns: Ukazatel na DNS hlavičku.

### void printQuesions(Params *params):

Popis: Vypisuje informace o sekci s otázkami v DNS odpovědi.

Parametry:

* params: Ukazatel na strukturu Params.

### void printAnswers(RES_RECORD answers[], Params *params, int i, sockaddr_in a, in_addr ipv4_addr, in6_addr ipv6_addr):

Popis: Vypisuje informace o jednotlivých odpovědích v DNS odpovědi.

Parametry:

* answers[]: Pole struktur obsahujících informace o DNS odpovědích.
* params: Ukazatel na strukturu Params.
* i: Index aktuální odpovědi.
* a: Struktura sockaddr_in pro IPv4 adresy.
* ipv4_addr: Struktura in_addr pro IPv4 adresy.
* ipv6_addr: Struktura in6_addr pro IPv6 adresy.

### u_char* ReadName(unsigned char* reader, unsigned char* buffer, int* count):

Popis: Čte a zpracovává názvy z DNS odpovědi.

Parametry:

* reader: Ukazatel na aktuální pozici v DNS zprávě.
* buffer: Ukazatel na buffer s DNS zprávou.
* count: Ukazatel na proměnnou pro počet přečtených znaků.

### void ChangetoDnsNameFormat(unsigned char* dns, char* host):

Popis: Převádí adresu do formátu DNS názvu.

Parametry:

* dns: Ukazatel na pole znaků pro výsledný DNS název.
* host: Cílová adresa, která se má převést.

### void reversedFormat(char* host):

Popis: Převádí IPv4 adresu do obráceného formátu pro PTR dotaz.

Parametry:

* host: Ukazatel na pole znaků obsahující IPv4 adresu.

### void expandIPv6(const char* compressedIPv6, char* expandedIPv6, size_t expandedIPv6Size):

Popis: Rozšiřuje zkrácený formát IPv6 adresy.

Parametry:
* compressedIPv6: Zkrácený formát IPv6 adresy.
* expandedIPv6: Ukazatel na pole znaků pro rozšířený formát IPv6 adresy.
* expandedIPv6Size: Velikost pole pro rozšířený formát.