static volatile unsigned short *screen =
    (unsigned short *)0xB8000;

static int cursor = 0;

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)


/* -------------------------------------------------
   VGA SCREEN
   ------------------------------------------------- */

static void scroll_screen(void)
{
    /* Move every line up by one */
    for (int i = 0;
         i < SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
         i++) {

        screen[i] = screen[i + SCREEN_WIDTH];
    }

    /* Clear last line */
    for (int i = SCREEN_WIDTH * (SCREEN_HEIGHT - 1);
         i < SCREEN_SIZE;
         i++) {

        screen[i] = (0x0F << 8) | ' ';
    }

    cursor -= SCREEN_WIDTH;

    if (cursor < 0) {
        cursor = 0;
    }
}


static void check_scroll(void)
{
    while (cursor >= SCREEN_SIZE) {
        scroll_screen();
    }
}


/* Forward declaration */
static void update_cursor(void);


static void putchar(char c)
{
    if (c == '\n') {

        cursor =
            ((cursor / SCREEN_WIDTH) + 1)
            * SCREEN_WIDTH;

        check_scroll();
        update_cursor();

        return;
    }

    screen[cursor++] =
        (0x0F << 8) | c;

    check_scroll();
    update_cursor();
}


static void print(const char *s)
{
    while (*s) {
        putchar(*s++);
    }
}


static void clear_screen(void)
{
    for (int i = 0;
         i < SCREEN_SIZE;
         i++) {

        screen[i] =
            (0x0F << 8) | ' ';
    }

    cursor = 0;
    update_cursor();
}


/* -------------------------------------------------
   PORT I/O
   ------------------------------------------------- */

static unsigned char inb(unsigned short port)
{
    unsigned char value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}


static void outb(
    unsigned short port,
    unsigned char value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value),
          "Nd"(port)
    );
}


/* -------------------------------------------------
   VGA HARDWARE CURSOR
   ------------------------------------------------- */

static void update_cursor(void)
{
    unsigned short pos =
        (unsigned short)cursor;

    /* Low byte */
    outb(0x3D4, 0x0F);
    outb(
        0x3D5,
        (unsigned char)(pos & 0xFF)
    );

    /* High byte */
    outb(0x3D4, 0x0E);
    outb(
        0x3D5,
        (unsigned char)((pos >> 8) & 0xFF)
    );
}


/* -------------------------------------------------
   STRING FUNCTIONS
   ------------------------------------------------- */

static int strcmp(
    const char *a,
    const char *b)
{
    while (*a && (*a == *b)) {
        a++;
        b++;
    }

    return
        *(unsigned char *)a -
        *(unsigned char *)b;
}


static int starts_with(
    const char *text,
    const char *prefix)
{
    while (*prefix) {

        if (*text != *prefix) {
            return 0;
        }

        text++;
        prefix++;
    }

    return 1;
}


/* -------------------------------------------------
   KEYBOARD
   ------------------------------------------------- */

static char keyboard_map[128] = {

    0, 27,

    '1','2','3','4','5',
    '6','7','8','9','0',
    '-','=', '\b',

    '\t',

    'q','w','e','r','t',
    'y','u','i','o','p',
    '[',']','\n',0,

    'a','s','d','f','g',
    'h','j','k','l',
    ';','\'','`',

    0,'\\',

    'z','x','c','v','b',
    'n','m',
    ',','.','/',

    0,'*',0,' '
};


/* -------------------------------------------------
   COMMANDS
   ------------------------------------------------- */

static void run_command(char *command)
{
    if (strcmp(command, "help") == 0) {

        print("Commands:\n");
        print(" help     - Show commands\n");
        print(" version  - Show OVICS version\n");
        print(" clear    - Clear screen\n");
        print(" echo     - Print text\n");
        print(" about    - About OVICS JUNK\n");
        print(" explore  - Explore junk tech\n");
        print(" project  - Show project status\n");
    }

    else if (
        strcmp(command, "version") == 0) {

        print("OVICS 0.0.2\n");
    }

    else if (
        strcmp(command, "clear") == 0) {

        clear_screen();
    }

    else if (
        starts_with(command, "echo ")) {

        print(command + 5);
        print("\n");
    }

    else if (
        strcmp(command, "echo") == 0) {

        print("\n");
    }

    else if (
        strcmp(command, "about") == 0) {

        print("\n");
        print("OVICS JUNK\n");
        print("------------------------------\n");
        print("Exploring old computers,\n");
        print("junk devices and forgotten tech.\n");
        print("\n");
        print("Giving old hardware another chance.\n");
        print("\n");
    }

    else if (
        strcmp(command, "explore") == 0) {

        print("\n");
        print("EXPLORE\n");
        print("------------------------------\n");
        print("JUNK PC / RETRO PC / Linux\n");
        print("CUSTOM ROM / OLD DEVICE\n");
        print("\n");
    }

    else if (
        strcmp(command, "project") == 0) {

        print("\n");
        print("PROJECT STATUS\n");
        print("------------------------------\n");
        print("Self-made OS : OVICS\n");
        print("YouTube      : @OVICSJUNK\n");
        print("Status       : ONLINE\n");
        print("\n");
    }

    else if (command[0] != 0) {

        print("Unknown command: ");
        print(command);
        print("\n");
    }
}


/* -------------------------------------------------
   KERNEL
   ------------------------------------------------- */

void kernel_main(void)
{
    char command[64];

    int command_pos = 0;

    clear_screen();

    print("OVICS 0.0.2\n");
    print("----------------\n");
    print(
        "Open World Interface Computer System\n\n"
    );

    print("OVICS> ");

    while (1) {

        /* Keyboard data available? */
        if (inb(0x64) & 1) {

            unsigned char scancode =
                inb(0x60);

            /*
             * Ignore key-release scancodes.
             */
            if (scancode < 128) {

                char c =
                    keyboard_map[scancode];

                /* ENTER */
                if (c == '\n') {

                    putchar('\n');

                    command[command_pos] = 0;

                    run_command(command);

                    command_pos = 0;

                    print("OVICS> ");
                }

                /* BACKSPACE */
                else if (c == '\b') {

                    if (command_pos > 0) {

                        command_pos--;

                        if (cursor > 0) {

                            cursor--;

                            screen[cursor] =
                                (0x0F << 8) | ' ';

                            update_cursor();
                        }
                    }
                }

                /* NORMAL CHARACTER */
                else if (c) {

                    if (command_pos < 63) {

                        command[command_pos++] =
                            c;

                        putchar(c);
                    }
                }
            }
        }
    }
}