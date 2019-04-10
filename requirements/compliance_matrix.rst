================================================================================
Projekt QBot - macierz zgodności
================================================================================


Wstęp
--------------------------------------------------------------------------------

Celem tego dokumentu jest ukazanie zaimplementowanych wymagań na dzień 2018-04-10.

Wymagania
--------------------------------------------------------------------------------

Poziom L0 - mikrokontroler
................................................................................

.. list-table:: Wymagania L0
   :widths: 15 20 35 35
   :header-rows: 1

   * - ID
     - Wymaganie
     - Opis
     - Zgodność

   * - REQ-L0-01
     - Regulacja mocy silnika
     - Ustawia zadane wypełnienie PWM w otwartej pętli
     - Wymaganie w pełni zrealizowane

   * - REQ-L0-02
     - Częstotliwość PWM min. 20kHz
     - Ustawia PWM na częstotliwość niesłyszalną dla ludzkiego ucha
     - Wymaganie częściowo zrealizowane. PWM ustawiony na ok 8kHz, co i tak jest więcej niż analogWrite()

   * - REQ-L0-03
     - Odczyt zużycia prądu w danej chwili
     - Odczytuje z ADC chwilowe pobór prądu przez silnik
     - Wymaganie w pełni zrealizowane

   * - REQ-L0-05
     - Odczyt enkoderów
     - Odczytuje tick-i z enkodera w przerwaniu
     - Nie zrealizowano

   * - REQ-L0-06
     - JSON API
     - Udostępnia właściwe API do komputera pokładowego w postaci JSON'a
     - Wymaganie w pełni zrealizowane

   * - REQ-EXTRAL0-01
     - Nastawia wybraną prędkość kątową kół
     - Udostępnia API do zadania prędkośći kątowej na koło
     - Nie zrealizowano ze względu na brak REQ-L0-05

   * - REQ-EXTRAL0-02
     - Kontroler PID
     - Implementuje kontroler PID. Zachowuje zadaną prędkość kątową
     - Nie zrealizowano ze względu na brak REQ-L0-05

   * - REQ-EXTRAL0-03
     - Dynamiczna zmiana parametrów PID
     - Udostępnia API do zmiany stałych PID, w celu przyśpieszenia ich wyznaczenia
     - Nie zrealizowano ze względu na brak REQ-L0-05


Poziom L1 - Raspberry PI
................................................................................

.. list-table:: Wymagania L1
   :widths: 15 20 35 35
   :header-rows: 1

   * - ID
     - Wymaganie
     - Opis
     - Zgodność

   * - REQ-L1-01
     - Obsługa portu szeregowego
     - Wykrywa dostępne urządzenia UART
     - Wymaganie w pełni zrealizowane

   * - REQ-L1-02
     - UART - Hot Plug
     - Umożliwia się (p)odpinanie urządzeń podczas działania aplikacji
     - Wymaganie częściowo zrealizowano. Sprawdzany jest stan podłączonych mikrokontrolerów.

   * - REQ-L1-03
     - Wysyłanie poleceń do drivera
     - Wysyła polecenia REQ-L0-01 i REQ-EXTRAL0-01 do mikrokontrolerów
     - Wymaganie w pełni zrealizowane

   * - REQ-L1-04
     - Odbiór danych pomiarowych
     - Odbiera dane o chwilowej prędkości kątowej, poborze prądu
     - Wymaganie w pełni zrealizowane

   * - REQ-L1-05
     - Dostęp do telemetrii
     - Udostępnia usługę na wzór HTTP w której prezentuje się telemetrię
     - Wymaganie w pełni zrealizowane z użyciem gRPC.

   * - REQ-L1-06
     - Odbiór poleceń sterujących
     - Odbiera polecenia sterujące jako stream danych w trybie sterowania mocą. Odebrane dane: kierunek i zwrot (wektor jednostkowy), wartość przepustnicy
     - Wymaganie w pełni zrealizowane

   * - REQ-L1-07
     - Model podwozia
     - Implementuje podstawowy model podwozia, umożliwiając sterowanie zgodnie z REQ-L1-06
     - Wymaganie w pełni zrealizowane

   * - REQ-L1-08
     - Odbiór polecenia udostępnienia kamery
     - Uruchamia serwer NetCat streamujący obraz z kamery po UDP
     - Wymaganie zrealizowano częściowo. Możliwe jest zdalne włączenie innego procesu. Pozostaje integracja z hardware'em i Raspberry PI. Wystarczy udostępnić właściwy skrypt.

   * - REQ-EXTRAL1-01
     - Skrypt poleceń
     - Umożliwia się wykonywanie gotowego skryptu wg którego robot przemieszcza się
     - Nie zrealizowano ze względu na brak REQ-L0-05

   * - REQ-EXTRAL1-02
     - Odbiór skryptu poleceń
     - Odbiera z komputera bazy skrypt poleceń (REQ-EXTRAL1-01) i wykonuje go zgodnie z zadanymi parametrami
     - Nie zrealizowano ze względu na brak REQ-L0-05

   * - REQ-EXTRAL1-03
     - Skrypt do pokonywania łuku
     - Udostępnia się opcję pokonywania zakrętu pod postacią łuku.
     - Nie zrealizowano ze względu na brak REQ-L0-05

   * - REQ-EXTRAL1-04
     - Uchwyt do autonomii
     - Projektuje się strukturę obiektów, by umożliwić w przyszłości implementację podstawową autonomię robota bazując na SLAM (wykorzystując m.in. IMU, VO)
     - Dzięki zastosowaniu gRPC możliwa jest niezależna implementacja aplikacji odpowiadającej za przetwarzanie sensorów, SLAM, etc. 


Poziom L2 - aplikacja bazy
................................................................................

Aplikacja L2 nie jest głównym celem projektu. Jej celem jest jedynie zademonstrowanie możliwości oprogramowania pozostałych poziomów. Ze względu na stopień skomplikowania stosu aplikacji, aplikacja L2 może zostać napisana w pythonie z użyciem Qt'a lub bazować na pierwotnej aplikacji Qt-OrionPi

.. list-table:: Wymagania L2
   :widths: 15 20 35 35
   :header-rows: 1

   * - ID
     - Wymaganie
     - Opis
     - Zgodność

   * - REQ-L2-01
     - Odbiór położenia kontrolera
     - Odczytuje położenia analogowych czujników kontrolera oraz przyciski cyfrowe
     - Wymaganie w pełni zrealizowane

   * - REQ-L2-01
     - Wyznaczenie martwej strefy kontrolera
     - Wyznacza się procentową martwą strefę kontrolera, która nie powoduje eventu wysyłania danych do L1
     - Wymaganie w pełni zrealizowane

   * - REQ-L2-03
     - Wysyła polecenia 
     - Wysyła polecenia zgodne z REQ-L1-06 po wstępnym przetworzeniu danych z REQ-L2-01
     - Wymaganie w pełni zrealizowane

   * - REQ-L2-04
     - Telemetria
     - Prezentuje w czasie rzeczywistym odebraną telemetrię w czasie rzeczywistym.
     - Wymaganie w pełni zrealizowane

   * - REQ-L2-05
     - Obraz z kamery
     - Uruchamia aplikację odbierającą obraz z kamery pokładowej, po otrzymaniu informacji uruchomienia serwera, zgodnie z REQ-L1-08
     - Wymaganie zrealizowano częściowo. Możliwe jest zdalne włączenie innego procesu. Pozostaje integracja z hardware'em i Raspberry PI. Wystarczy udostępnić właściwy skrypt.

   * - REQ-EXTRAL2-01
     - Serializacja skryptu poleceń
     - Serializuje i wysyła skrypt poleceń do L1, zgodnie z REQ-EXTRAL1-01 i REQ-EXTRAL1-02
     - Nie zrealizowano ze względu na brak REQ-L0-05

Założenia projektowe
--------------------------------------------------------------------------------

Ogólne
................................................................................

.. list-table:: Ogólne założenia projektowe
   :widths: 15 20 35 35
   :header-rows: 1

   * - ID
     - Założenie
     - Opis
     - Zgodność

   * - ASM-GEN-01
     - Język programowania
     - C++ w wersji min. c++11, preferencyjnie c++14
     - Wymaganie w pełni zrealizowane

   * - ASM-GEN-02
     - AVR: MISRA
     - O ile możliwe, wdrożyć i dostosować reguły zgodne z MISRA np. `<https://lars-lab.jpl.nasa.gov/JPL_Coding_Standard_C.pdf>`_
     - Zrealizowane dla L0

   * - ASM-GEN-03
     - Biblioteki
     - AVR: biblioteka do JSON/nanopb; RPi: gRPC, protobuf, boost, Baza: gRPC, Qt, protobuf
     - Wykorzystano wymienione technologie

   * - ASM-GEN-04
     - Metodologia
     - TDD
     - Wykonano zestaw testów dla L1

   * - ASM-GEN-05
     - Deployment
     - Instalacja bare metal (RPi), co możliwe należy uruchamiać z użyciem Docker'a
     - Prawdopodobnie możliwe. Nie sprawdzono.

   * - ASM-GEN-05
     - System budowania
     - W zależności od wybranej platformy, qmake, cmake, zależne od IDE
     - L0: PlatformIO, L1: CMake/QtCreator, L2: QMake/QtCreator

   * - ASM-GEN-06
     - Platformy
     - AVR: wsad; RPi: Arch Linux, w przypadku Dockera rozważany Hypriot, aplikacja desktopowa: Arch Linux
     - Jak wymieniono

