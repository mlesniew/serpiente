#include <allegro.h>
#include <time.h>

// Rozdzielczosc
#define screenX 640
#define screenY 480

// Wielkosc macierzy
#define matrixX 64
#define matrixY 48

// Wielkosc bloku
#define blockX 10
#define blockY 10

// Najwyzsza predkosc
#define MaxSpeed 10

// 1000 / czestotliwosc odswiezania
#define RestTime 40

// Poczatkowa dlugosc weza
#define InitLen 3

// Czas oczekiwania na demo
#define demoLoops 190

// Czy po wyjsciu z jednej strony z planszy, waz ma sie pojawic po drugiej?
int teleportar = 1;

// Pelny ekran
int fullscreen = 0;

// Wstepna predkosc
int Speed = MaxSpeed / 2;
int color;

BITMAP *bmp;
BITMAP *sprites;
PALETTE pal;

// ===== CZESC WEZA =====

struct LaParte
{
    int X, Y;
    int sprno;
    LaParte *sucesivo;
    LaParte() { sucesivo = NULL; X = 0; Y = 0; sprno = 0; }
    LaParte(LaParte *parte) { sucesivo = parte; X = 0; Y = 0; sprno = 0; }
    void dibujar()
    {
        if (sucesivo != NULL) sucesivo->dibujar();
        blit(sprites, bmp, sprno * blockX, 0, X * blockX, Y * blockY, blockX, blockY);
    }

    bool buscar(int bX, int bY)
    {
        if ((bX == X) && (bY == Y)) return true;
        else if (sucesivo == NULL) return false;
        else return sucesivo->buscar(bX, bY);
    }

    ~LaParte() { if (sucesivo != NULL) delete sucesivo; }
};

// ===== WAZ =====

struct LaSerpiente
{
    int X, Y;
    LaParte *cabeza;

    void borrarUltimo()
    {
        if (cabeza != NULL) {
            if (cabeza->sucesivo == NULL)
            {
                delete cabeza;
                cabeza = NULL;
            }
            LaParte *p = cabeza;
            while (p->sucesivo->sucesivo != NULL)
            {
                p = p->sucesivo;
            }
            delete p->sucesivo;
            p->sucesivo = NULL;
            switch (p->sprno) {
                case 0: case 4: case 8: case 17: case 18:
                p->sprno = 4; break;
                case 1: case 5: case 9: case 14: case 15:
                p->sprno = 5; break;
                case 2: case 6: case 10: case 16: case 19:
                p->sprno = 6; break;
                case 3: case 7: case 11: case 12: case 13:
                p->sprno = 7; break;
            }
        }
    }

    void anadir(int dx, int dy)
    {
        LaParte *p = cabeza;

        dy *= -1;

        int os = -1;
        if (p != NULL) os = p->sprno;

        cabeza = new LaParte(p);
        cabeza->X = X;
        cabeza->Y = Y;

        // Wybor sprite'a glowy
        if (dx == -1)
        {
            // Glowa w lewo
            cabeza->sprno = 3;
            switch (os) {
            case 0: cabeza->sucesivo->sprno = 12; break;
            case 2: cabeza->sucesivo->sprno = 13; break;
            case 3: cabeza->sucesivo->sprno = 11; break;
            }
        }
        else if (dx == 1)
        {
            // Glowa w prawo
            cabeza->sprno = 1;
            switch (os) {
            case 0: cabeza->sucesivo->sprno = 15; break;
            case 1: cabeza->sucesivo->sprno = 9; break;
            case 2: cabeza->sucesivo->sprno = 14; break;
            }
        }
        else if (dy == 1)
        {
            // Glowa w gore
            cabeza->sprno = 0;
            switch (os) {
            case 0: cabeza->sucesivo->sprno = 8; break;
            case 1: cabeza->sucesivo->sprno = 17; break;
            case 3: cabeza->sucesivo->sprno = 18; break;
            }
        }
        else {
            // Glowa w dol
            cabeza->sprno = 2;
            switch (os) {
            case 1: cabeza->sucesivo->sprno = 16; break;
            case 2: cabeza->sucesivo->sprno = 10; break;
            case 3: cabeza->sucesivo->sprno = 19; break;
            }
        }

    }

    bool buscar(int bX, int bY)
    {
        if (cabeza == NULL) return false;
            else return cabeza->buscar(bX, bY);
    }

    bool mover(int dx, int dy, int l)
    {
        X += dx;
        Y += dy;

        if (teleportar != 0) {
            if (X < 0) X = matrixX - 1; if (Y < 0) Y = matrixY - 1;
            if (X >= matrixX) X = 0; if (Y >= matrixY) Y = 0;
        }

        if ((dx != 0) || (dy != 0))
        {
            anadir(dx, dy);
            if (l <= 0) borrarUltimo();
        }

        // Czy waz ugryzl sie w dupe?
        if (cabeza == NULL) return false;   // Brak glowy - nie mogl sie ugryzc
        else if (cabeza->sucesivo != NULL)  // Tylko glowa bez ogona - tez nie mogl
            if (cabeza->sucesivo->buscar(X, Y)) // Glowa + ogon =
                return true;                    //      ugryziona dupa?

        // Czy waz ugryzl sciane?
        if (teleportar == 0) {
        // Przy teleportowaniu nie moze
            if (X < 0) return true;
            if (Y < 0) return true;
            if (X >= matrixX) return true;
            if (Y >= matrixY) return true;
        }

        return false;
    }

    void dibujar()
    {
        if (cabeza != NULL) cabeza->dibujar();
    }

    LaSerpiente(int longitud)
    {
        cabeza = NULL;
        X = (matrixX - longitud) / 2;
        Y = matrixY / 2;
        for (int l = 0; l <= longitud; l++)
        {
            anadir(-1, 0);
            X--;
        }
        X++;
        borrarUltimo();
    }

    ~LaSerpiente()
    {
        if (cabeza != NULL) delete cabeza;
    }
};


void winfull();
void init();
void deinit();

int demo()
{
    int dx = -1, dy = 0;
    int p = 0;
    int l = 0;

    LaSerpiente serpiente(InitLen);

    int appleX, appleY;

    do {
        appleX = 1 + rand() % (matrixX - 2);
        appleY = 1 + rand() % (matrixY - 2);
    } while (serpiente.buscar(appleX, appleY));

    bool GameOver = false;

    while (!GameOver) {
        l = 0;

        // Wyczyszczenie ekranu
        clear_to_color(bmp, sprites->line[0][0]);

        // Napis
        blit(sprites, bmp, 0, blockY, (screenX - blockX * 21) / 2, blockY, 21 * blockX, blockY);
        textprintf_centre_ex(bmp, font, screenX / 2, blockY * 3, color, -1, "..:: demostracion ::..");

        // Narysowanie weza
        serpiente.dibujar();

        // Narysowanie jablka
        blit(sprites, bmp, 20 * blockX, 0, appleX * blockX, appleY * blockY, blockX, blockY);

        // Kontrola weza - automatyczna

        if (dx == -1)
        {
            if (appleX == serpiente.X)      // Jestesmy na wysokosci jablka
            { dx = 0; dy = (appleY < serpiente.Y)? -1: 1; }
            if (appleX > serpiente.X)       // Jablko jest na prawo
                if (teleportar == 0)        // Trzeba zawrocic
                    { dx = 0; dy = (rand() % 2 == 0)? 1: -1; }
        } else if (dx == 1) {
            if (appleX == serpiente.X)      // Jestesmy na wysokosci jablka
            { dx = 0; dy = (appleY < serpiente.Y)? -1: 1; }
            if (appleX < serpiente.X)       // Jablko jest na lewo
                if (teleportar == 0) // Trzeba zawrocic
                    { dx = 0; dy = (rand() % 2 == 0)? 1: -1; }
        } else if (dy == -1) {
            if (appleY == serpiente.Y)      // Jestesmy na wysokosci jablka
            { dy = 0; dx = (appleX < serpiente.X)? -1: 1; }
            if (appleY > serpiente.Y)       // Jablko jest na prawo
                if (teleportar == 0)        // Trzeba zawrocic
                    { dy = 0; dx = (rand() % 2 == 0)? 1: -1; }
        } else {
            if (appleY == serpiente.Y)      // Jestesmy na wysokosci jablka
            { dy = 0; dx = (appleX < serpiente.X)? -1: 1; }
            if (appleY < serpiente.Y)       // Jablko jest na prawo
                if (teleportar == 0)        // Trzeba zawrocic
                    { dy = 0; dx = (rand() % 2 == 0)? 1: -1; }
        }

        // Poddanie sie
        if (key[KEY_ESC]) GameOver = true;

        // Zmiana na pelny ekran / okno?
        if ((key_shifts & KB_ALT_FLAG) && (key[KEY_ENTER])) winfull();

        // Przesuniecie weza
        if ((serpiente.X + dx == appleX) && (serpiente.Y + dy == appleY))
        {
            p += MaxSpeed - Speed + 1;
            l = 1;
            do {
                appleX = 1 + rand() % (matrixX - 2);
                appleY = 1 + rand() % (matrixY - 2);
            } while (serpiente.buscar(appleX, appleY));
        }
        if (serpiente.mover(dx, dy, l)) GameOver = true;

        // Skopiowanie na ekran
        blit(bmp, screen, 0, 0, 0, 0, screenX, screenY);

        // Opoznienie
        rest(RestTime);
    }

    // Informacja o przegranej
    textprintf_centre_ex(bmp, font, screenX / 2, screenY/2 - 50, color, -1, "fin de demostracion");

    // Zamruganie
    for (int i = 0; i < 3; i++)
    {
        clear_to_color(screen, sprites->line[0][0]);
        rest(250);
        blit(bmp, screen, 0, 0, 0, 0, screenX, screenY);
        rest(250);
    }

    // Zwrocenie punktow
    return p;
}

int jugar()
{
    int dx = 0, dy = 0;
    int p = 0;
    int l = 0;
    int c = 0;

    LaSerpiente serpiente(InitLen);

    int appleX, appleY;

    do {
        appleX = 1 + rand() % (matrixX - 2);
        appleY = 1 + rand() % (matrixY - 2);
    } while (serpiente.buscar(appleX, appleY));

    bool GameOver = false;

    while (!GameOver) {
        l = 0;

        // Wyczyszczenie ekranu
        clear_to_color(bmp, sprites->line[0][0]);

        // Narysowanie weza
        serpiente.dibujar();

        // Narysowanie jablka
        blit(sprites, bmp, 20 * blockX, 0, appleX * blockX, appleY * blockY, blockX, blockY);

        // Kontrola weza
        if (key[KEY_UP] && (dy != 1)) { dx = 0; dy = -1; }
        else if (key[KEY_DOWN] && (dy != -1)) { dx = 0; dy = 1; }
        else if (key[KEY_LEFT] && (dx != 1)) { dx = -1; dy = 0; }
        else if (key[KEY_RIGHT] && (dx != -1)) { dx = 1; dy = 0; }

        // Pausa
        if (key[KEY_P]) { dx = 0; dy = 0; }

        // Poddanie sie
        if (key[KEY_ESC]) GameOver = true;

        // Zmiana na pelny ekran / okno?
        if ((key_shifts & KB_ALT_FLAG) && (key[KEY_ENTER])) winfull();

        // Licznik czasu
        c = ++c % Speed;

        // Przesuniecie weza
        if (c == 0)
        {
            if ((serpiente.X + dx == appleX) && (serpiente.Y + dy == appleY))
            {
                p += MaxSpeed - Speed + 1;
                l = 1;
                do {
                    appleX = 1 + rand() % (matrixX - 2);
                    appleY = 1 + rand() % (matrixY - 2);
                } while (serpiente.buscar(appleX, appleY));
            }
            if (serpiente.mover(dx, dy, l)) GameOver = true;
        }

        // Pokazanie punktow
        textprintf_centre_ex(bmp, font, screenX / 2, 0, color, -1, "Puntos: %d00", p);

        // Skopiowanie na ekran
        blit(bmp, screen, 0, 0, 0, 0, screenX, screenY);

        // Opoznienie
        rest(RestTime);
    }

    // Informacja o przegranej
    textprintf_centre_ex(bmp, font, screenX / 2, screenY/2 - 50, color, -1, "fin de juego");

    // Zamruganie
    for (int i = 0; i < 3; i++)
    {
        clear_to_color(screen, sprites->line[0][0]);
        rest(250);
        blit(bmp, screen, 0, 0, 0, 0, screenX, screenY);
        rest(250);
    }

    // Zwrocenie punktow
    return p;
}

int main() {
    init();

    int top = 0, last = 0;
    int loops = 0;

    color = makecol8(255, 0, 0);

    while (!key[KEY_ESC])
    {
        clear_to_color(bmp, sprites->line[0][0]);

        textprintf_centre_ex(bmp, font, screenX / 2, screenY / 2 - 70, color, -1, "..:: Michal Lesniewski ::..");

        blit(sprites, bmp, 0, blockY, (screenX - blockX * 21) / 2, screenY / 2 - 50, 21 * blockX, blockY);

        textprintf_centre_ex(bmp, font, screenX / 2, screenY / 2 - 10, color, -1, "Ultimo resultado: %d00", last);
        textprintf_centre_ex(bmp, font, screenX / 2, screenY / 2 - 20, color, -1, "Mejor resultado: %d00", top);

        textprintf_centre_ex(bmp, font, screenX / 2, screenY / 2 + 10, color, -1, "Velocidad: %d / %d (arriva/abajo)", MaxSpeed - Speed + 1, MaxSpeed);
        if (teleportar == 0)
            textprintf_centre_ex(bmp, font, screenX / 2, screenY / 2 + 20, color, -1, "Teleportar: NO (T)");
        else
            textprintf_centre_ex(bmp, font, screenX / 2, screenY / 2 + 20, color, -1, "Teleportar: SI (T)");

        textprintf_centre_ex(bmp, font, screenX / 2, screenY / 2 + 40, color, -1, "Juego - SPACE, Demostracion - D, Salida - ESC");

        if (key[KEY_UP] && (Speed > 1))
        { Speed--; loops = 0; }
        if (key[KEY_DOWN] && (Speed < MaxSpeed))
        { Speed++; loops = 0; }
        if (key[KEY_T])
        { teleportar = ++teleportar % 2; loops = 0; }
        if (key[KEY_D] || (loops >= demoLoops))
        { demo(); loops = 0; }

        // Zmiana na pelny ekran / okno?
        if ((key_shifts & KB_ALT_FLAG) && (key[KEY_ENTER])) { winfull(); loops = 0; }

        if (key[KEY_SPACE])
        {
            last = jugar();
            if (last > top) top = last;
            loops = 0;
        }
        loops++;
        blit(bmp, screen, 0, 0, 0, 0, screenX, screenY);
        rest(RestTime*2);
    }

    deinit();
    return 0;
}
END_OF_MAIN()

void winfull()
{
    fullscreen = ++fullscreen % 2;

    int res;

    if (fullscreen == 0)
        res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, screenX, screenY, 0, 0);
    else
        res = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, screenX, screenY, 0, 0);

    if (res != 0) {
        allegro_message(allegro_error);
        deinit();
        exit(-1);
    }

    set_palette(pal);
    rest(500);
}

void init() {
    int res;
    allegro_init();
    set_color_depth(8);
    res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, screenX, screenY, 0, 0);
    if (res != 0) {
        allegro_message(allegro_error);
        exit(-1);
    }
    set_window_title("Serpiente");

    install_timer();
    install_keyboard();
    install_mouse();
    bmp = create_bitmap(screenX, screenY);
    set_color_conversion(COLORCONV_24_TO_8);
    sprites = load_bmp("serpiente.bmp", (RGB*)&pal);
    set_palette(pal);
    srand(time(NULL));
}

void deinit() {
    clear_keybuf();
    destroy_bitmap(bmp);
    destroy_bitmap(sprites);
}
