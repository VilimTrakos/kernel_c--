#include <iostream>
#include <cstring>
#include <cstdlib>

#include <csignal>
// SIGINT ctrl-C - igrac A
// SIGQUIT ctrl-Ž - igrac B
// SIGTSTP ctrl-Z - izlaz
using namespace std;

int N; // predstavlja broj žigica na stolu, na poèetku igre- prima se kao argv, mora biit veæi od M!
bool igracA = true;
int M = 3; // konstantan param, predstavlja max broj žigica koje je moguæe uzeti, mora biti veæi od 2 i cijeli broj

void unos(int sig)
{
    int uzima;
    if (sig == SIGTSTP)
    {
        cout << "SIGTSTP" << endl;
        exit(0);
    }

    if (sig == SIGINT) // igrac A
    {
        if (igracA)
        {
            cout << "Na redu je igrac A:" << endl;
            cin >> uzima;
            if (uzima > M)
            {
                cout << "Maksimalno se može uzeti " << M << " zigica" << endl;
                return;
            }
            else
            {
                N = N - uzima;
                cout << "Na stolu ostaje " << N << " zigica" << endl;
                igracA = false;
                if (N <= 0)
                {
                    cout << "Igrac B pobjeduje!" << endl;
                    exit(0);
                }
                return;
            }
        }
        else
        {
            cout << "Igrac A nije na redu!" << endl;
            return;
        }
    }
    else if (sig == SIGQUIT) // igrac B
    {
        if (!igracA)
        {
            cout << "Na redu je igrac B:" << endl;
            cin >> uzima;
            if (uzima > M)
            {
                cout << "Maksimalno se može uzeti " << M << " zigica" << endl;
                return;
            }
            else
            {
                N = N - uzima;
                cout << "Na stolu ostaje " << N << " zigica" << endl;
                igracA = true;
                if (N <= 0)
                {
                    cout << "Igrac A pobjeduje!" << endl;
                    exit(0);
                }
                return;
            }
        }
        else
        {
            cout << "Igrac B nije na redu!" << endl;
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Program nema ispravan broj elemenata unesenih preko naredbenog redka!" << endl;
        return 0;
    }
    // segmentation za pocetak - if argc == 0  return 0
    N = atoi(argv[1]);

    if (N < M)
    {
        cout << "Broj zigica na pocetku igre je pre malen, mora biti veci od M(" << M << ")" << endl;
        return 0;
    }
    else
    {
        cout << "Broj zigica na pocetku igre: " << N << endl;
    }
    sigset(SIGINT, unos);  // za igraèa A
    sigset(SIGQUIT, unos); // za igraèa B
    sigset(SIGTSTP, unos); // za izlaz iz programa

    while (N > 0)
    {
        pause();
    }

    return 0;
}
