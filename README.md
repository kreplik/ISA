# ISA

Popis zadání:

Cílem projektu bylo vytvořit program dns, který bude umět zasílat dotazy na DNS servery a následně vypisovat přijaté odpovědi.

### Spuštění

V souboru se nachází *Makefile*, který umožní sestavit program pomocí příkazu *make*

Samotný dns program se spouští s použitím přepínačů v podobě:

./dns [-r] [-x] [-6] -s server [-p port] adresa

* *-r* Volitelný přepínač, který určuje, že je vyžadovaná rekurze.
* *-x* Volitelný přepínač, který určuje, že se má zaslat reverzní dotaz
* *-6* Volitelný přepínač, který určuje zaslání dotazu typu AAAA. (výchozí A)
* *-s server* IP adresa nebo doménové jméno serveru, kam se má zaslat dotaz.
* *-p port* Volitelný přepínač, který určuje, číslo portu na který se má poslat dotaz. (výchozí 53)
* *adresa* dotazovaná adresa
* *-h* Vypíše nápovědu.


### Implementace

Program je implementován pomocí jazyka C++ v souboru dns.cpp. Na začátku se pomocí funkce *void getArgs(int argc, char *argv[], Params *params)* zpracují zadané přepínače z příkazové řádky a zapíše tyto informace do struktury *Params*

Mezi hlavní funkce programu patří funkce *query(Params *params)* Tato funkce vytváří UDP socket, sestavuje DNS dotaz, odesílá jej na DNS server, přijímá odpověď a následně zpracovává tuto odpověď. V kódu jsou použity struktury pro reprezentaci DNS zprávy a odpovědí, a to jak pro IPv4, tak i pro IPv6 adresy. Funkce také využívá další pomocné funkce, jako je ChangetoDnsNameFormat() pro konverzi adresy do formátu DNS názvu a ReadName() pro čtení DNS názvu ze zprávy. Jejich popis je uveden níže.

Po vytvoření socketu se získá adresa serveu, na který se bude dotaz posílat. Podle zadaných parametrů z příkazové rádky se nastaví jednotlivé proměnné ve struktuře DNS_HEADER, která reprezentuje hlavičku DNS zprávy. Tato struktura obsahuje informace o celkovém formátu zprávy, identifikátoru, příznacích a dalších důležitých údajích.

Pokud byl zadán přepínač *-x* pro reverzní dotaz, změní se formát adresy pomocí funkce *char* reversedFormat(char* host)* pro adresu typu IPv4, nebo funkce *void expandIPv6(const char* compressedIPv6, char* expandedIPv6, size_t expandedIPv6Size)* pro typ IPv6.


* Funkce reversedFormat je zodpovědná za převod IPv4 adresy do reverzní podoby, která je používána v DNS pro PTR záznamy. Při PTR dotazu je potřeba převést běžnou IPv4 adresu na formu vhodnou pro reverzní dotaz, kde části adresy jsou zpětně pořadovány a přidána je koncovka "in-addr.arpa".

* Funkce expandIPv6 slouží k rozšíření zkrácené IPv6 adresy na plnou délku v čitelnějším tvaru, převede ji na formu pro reverzní dotaz a přidá koncovku "ip6.arpa".

Následuje konverze adresy do formátu DNS pomocí funkce *void ChangetoDnsNameFormat (unsigned char* dns,char* host)*.

DNS názvy jsou v zprávě reprezentovány speciálním formátem, kde jednotlivé části názvu jsou odděleny tečkou. Navíc délka každé části je zakódována na začátku, což umožňuje efektivnější reprezentaci názvů.
* dns: Ukazatel na buffer, kam bude zapsán výsledný DNS název.
* host: Řetězec obsahující adresu nebo název.

Pomocí funkce *sendto()* se odešle zpráva na zadaný server. Následně se příjme odpověď pomocí funkce *recvfrom*. Obě funkce jsou z knihovny *sys/socket.h*. Předtím, než se začne zpracovávat přijatá zprává, se na standardní výstup vypíše informace o získané odpovědi a odeslaném dotazu. Následuje zpracování přijaté zprávy pomocí funkce *unsigned char* ReadName (unsigned char* reader, unsigned char* buffer, int* count)*

*unsigned char* ReadName (unsigned char* reader, unsigned char* buffer, int* count)*
Funkce ReadName je zodpovědná za čtení DNS názvu ze zprávy. Tato funkce je základním stavebním blokem při zpracování DNS odpovědí, protože DNS názvy jsou v zprávě reprezentovány speciálním formátem.
* reader: Ukazatel na aktuální pozici v zprávě.
* buffer: Ukazatel na začátek bufferu s kompletní zprávou.
* count: Ukazatel na proměnnou, do které se uloží počet kroků při čtení.




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