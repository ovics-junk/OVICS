static volatile unsigned short *screen = (unsigned short *)0xB8000;
static int cursor = 0;

static void putchar(char c)
{
    if (c == '\n') {
        cursor = ((cursor / 80) + 1) * 80;
        return;
    }

    screen[cursor++] = (0x0F << 8) | c;
}

static void print(const char *s)
{
    while (*s) {
        putchar(*s++);
    }
}

static void clear_screen(void)
{
    for (int i = 0; i < 80 * 25; i++) {
        screen[i] = (0x0F << 8) | ' ';
    }

    cursor = 0;
}

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

static int strcmp(const char *a, const char *b)
{
    while (*a && (*a == *b)) {
        a++;
        b++;
    }

    return *(unsigned char *)a - *(unsigned char *)b;
}

static int starts_with(const char *text, const char *prefix)
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

static char keyboard_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

static void run_command(char *command)
{
    if (strcmp(command, "help") == 0) {
        print("Commands:\n");
        print(" help     - Show commands\n");
        print(" version  - Show OVICS version\n");
        print(" clear    - Clear screen\n");
        print(" echo     - Print text\n");
    }
    else if (strcmp(command, "version") == 0) {
        print("OVICS 0.0.2\n");
    }
    else if (strcmp(command, "clear") == 0) {
        clear_screen();
    }
    else if (starts_with(command, "echo ")) {
        print(command + 5);
        print("\n");
    }
    else if (strcmp(command, "echo") == 0) {
        print("\n");
    }
    else if (command[0] != 0) {
        print("Unknown command: ");
        print(command);
        print("\n");
    }
}

void kernel_main(void)
{
    char command[64];
    int command_pos = 0;

    clear_screen();

    print("OVICS 0.0.2\n");
    print("----------------\n");
    print("Open World Interface Computer System\n\n");
    print("OVICS> ");

    while (1) {

        if (inb(0x64) & 1) {

            unsigned char scancode = inb(0x60);

            if (scancode < 128) {

                char c = keyboard_map[scancode];

                if (c == '\n') {

                    putchar('\n');

                    command[command_pos] = 0;
                    run_command(command);

                    command_pos = 0;

                    print("OVICS> ");
                }
                else if (c == '\b') {

                    if (command_pos > 0) {
                        command_pos--;
                    }
                }
                else if (c) {

                    if (command_pos < 63) {
                        command[command_pos++] = c;
                        putchar(c);
                    }
                }
            }
        }
    }
}