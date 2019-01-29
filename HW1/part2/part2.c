typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";

// GIVEN
void rpu(u32 x, u32 base)
{  
    char c;
    if (x){
       c = ctable[x % base];
       rpu(x / base, base);
       putchar(c);
    }
}

void printu(u32 x)
{
   (x==0) ? putchar('0') : rpu(x, 10);
}

// GIVE SOME BACK
void printd(int x)
{
	if (x < 0)
	{
		putchar('-');
		x = x * -1;
		x = (u32) x;
	}
	printu(x);
}

void printx(u32 x)
{
	putchar('0');
	putchar('x');
	(x==0) ? putchar('0') : rpu(x, 16);
}

void printo(u32 x)
{
	putchar('0');
	putchar('o'); // This is the new convention for octal numbers since many other number representations use leadings zeros
	(x==0) ? putchar('0') : rpu(x, 8);
}

int myprintf(char *fmt, ...)
{
	int num = 0;

	char *str;
	int *args = (int *) (&fmt + 1);

	while (*fmt != '\0')
	{
		if (*fmt != '%')
		{
			putchar(*fmt);
		}
		else if (*fmt == '\n')
		{
			putchar('\n');
			putchar('\r');
		}
		else
		{
			// Call appropriate function for format specifier
			fmt++;
			switch(*fmt)
			{
				case 'c':
					putchar(*args);
					break;
				case 's':
					str = (char *) *args;
					while (*str != '\0')
					{
						putchar(*str++);
					}
					break;
				case 'u':
					printu(*args);
					break;
				case 'd':
					printd(*args);
					break;
				case 'o':
					printo(*args);
					break;
				case 'x':
					printx(*args);
					break;
			}
			args++;
			num++;
		}
		fmt++;
	}
	return num; // This is the behavior of the normal printf, it returns the number of arguments replaced
}

// MAIN
int main(void)
{
	myprintf("Hello %d %d %s\n", 3, 4, "HONK");
	myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 'A', "this is a test", 100,    100,   100,  -100);
	myprintf("BONK\n", 3, 4, "HONK");
}
