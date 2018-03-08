#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


static void sigHandler (int sig, siginfo_t *sigInfo, void *context) {
    printf("\n Signal %d, data %d \n", sig, sigInfo->si_value.sival_int);
}

void* signalHandler(void *args) {
    struct sigaction act;
    int *signo = (int*) args;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = &sigHandler;
    act.sa_flags = SA_SIGINFO;
    
    if (sigaction(SIGRTMIN + *signo, &act, 0) < 0) {
        free(signo);
        perror ("Sigaction");
        return (void*)1;
    }
    while(1);
    free(signo);
    return 0;
}

int main(void) {
	const int SIZE =  16;
	int i;
	pthread_t *threads[SIZE];
    for (i = 0; i < SIZE; ++i) {
        pthread_t *thread = malloc(sizeof(pthread_t));
        int *number = malloc(sizeof(int));
        *number = i;
        threads[i] = thread;
		int tid = pthread_create(thread, 0,  &signalHandler, number);
	}
    while(1);
    for(i = 0; i < SIZE; ++i)
        free(threads[i]);
	return EXIT_SUCCESS;
}

/*
 * Program obsługujący sygnały czasu rzeczywistego. Minimum: wyświetlenie komunikatu o otrzymaniu konkretnego sygnału. Wsystarcza do zaliczenia.
 * Dodatkowe rzeczy: premiowane
 * Sygnały czasu rzeczyiwstego - rozszerzenie tradycyjnych sygnałów. Dodatkowo 2 nowe własności
 * 	- możliwość kolejkowania (nie są gubione jeśli są blokowane, są zachowane w kolejkach, obsługiwane jak pojawi się możliwośc)
 * 	- możliwość przenoszenia danych (4 bajty)
 *	 - sama obsługa może się sprowadzać do wyświetlenia prostego komunikatu
 * 	Wymagania dodatkowe:
 * 	 - odebrać dane przekazane z sygnału
 * 	 - przynajmniej dwa różne sygnały czasu rzeczywistego (z 16 dostępnych w systemie)
 *	 - synchroniczna obsługa sygnału
 *	 	- 3 wątki : macierzysty i dwa potomne do obsługi dwóch różnych sygnałów
 *	 - zgodne ze standardem C/C++
 *	 - na ile to możliwe zgodność ze standardem POSIX (korzystamy z funkcji POSIX)
 *
 *	 Obługa synchronicza/asynchroniczna
 *	 synchroniczna - osobny wątek blokowany w oczekiwaniu na nadejście sygnału
 *	 asynchronicza - podobna do obsługi przerwań
 *	 kill -<nr sygnalu> <nr procesu> - POSIXowa funkcja do wysyłania sygnałów
 *
 *	 Aby wysłać sygnał z danymi należy napisac własną komendę kill, która umożliwi wysłanie dowolnego sygnału z dowolnymi danymi do dowolnego procesu (drugi program)
 *
 *	 Parametry powinny być pobierane z lini poleceń (parametry wejściowe)
 *
 *ustawienie maski konieczne, żeby nie powodowac niepożadnych zachowań procesu
 *		sigwaitinfo(sigset_t zbior sygnalow na jakie watek bedzie czekal, ktore spowoduja jego odblokowanie (negatyw maski); struktura ktora ma zostac wypelniona informacjami o otrzymanym sygnale
 *		jednym z pol z tej struktury jest unia ktora zawiera dane przekazane z sygnalem) - czekaj do nadejscia sygnalu
 *	 pthread_join(*pthread, NULL)
 *	 pthread_sigmask(SIG_BLOCK, zbiór sygnałów do blokowania, zbiór sygnałów do zachowania poprzedniej maski(NULL)) - należy odpowiednio poustawiac maski, żeby wątki odbierały tylko jeden, a blokowały wszystkie inne. Żeby inne blokowaly ten.
 *
 *	4 funkcje operujące na zbiorach sygnałów (sigset_t*)
 *	- sigfillset() - wypelnia zbior wszyskimi sygnalami
 *	- sigemptyset() - usuwa wszystkie sygnaly ze zbioru
 *	- sigaddset() - dodaje pojedynczy sygnal ze zbioru
 *	- sigdelset() - usuwa pojedynczy synal ze zbioru
 *
 *sigqueue(pid, numery sygnalu, dane) - sluzy do wyslania sygnalu do procesu -> odczytujemy 3 liczby calkowite, inicjalizujemy 3 parametry tymi liczbami.
 *41 ... 56 16 POSIX realtime signals (SIGRTMIN to SIGRTMAX)
 * QNX application + Momentics
 */
