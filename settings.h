// Pokud je pozadovana komunikace s PC, tak umazat //
#define LOGS
//#define LOGS_readed_vals

// Pinout  
#define               TRANS_1         9      // NEMENIT - Vysílač páru 1
#define               TRANS_2         10     // NEMENIT - Vysílač páru 2
#define               REC_1           A2     // Přijímač páru 1
#define               REC_2           A3     // Přijímač páru 2
#define               output          LED_BUILTIN   // Výstup, který je v log0 po celou dobu snímání ruky, maximální délka je MAX_TIME
#define               outputPulse     8      // Výstup, který je v log0 jen určitý čas (PULSE_TIME) a je jedno, jak dlouho je tam ruka
#define               calibration     A0     // Kalibrační potenciometer
#define               error           2      // Error dioda, ukazuje, že je ruka snímána moc dlouho (ERROR_TIME), výstupy jsou již vypnuté, program se zde zasekne, nutný restart.

// Defines
#define               FREQ            20     //Hz    Modulacni frekvence, max 100Hz (asi)
#define               FREQ_DEAD_ZONE  25     //Hz    Maxmimalni odchylka namerene frekvence od vysilane
#define               omega           2 * PI * FREQ //rad/s  NEMENIT
const int             loopTime      = 1800;  //us    Perioda vzorkovani, min 1800
#define               lengthOfQ       100    //      Pocet vzorku, max 250, jinak pretece pamet
const unsigned long   MAX_TIME      = 5;     //s     Maximalni doba trvani vystupu 'output'
const unsigned long   PULSE_TIME    = 1;     //s     Doba trvani vystupu 'outputPulse'
const unsigned long   ERROR_TIME    = 30;   //s     Maximalni povolena doba snimani ruky, vystup 'error'
#define               enable_ERROR    true   //      false - bez erroru     true - s errorem
#define               OUTPUT_NEG      false  //      false - pri zaznamenani LOG1
