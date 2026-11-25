#ifdef _WIN32
#include <windows.h>
#endif
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

const double PI = 3.141592653589793;
// FUNCION PARA LIMPIAR PANTALLA
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[H" << flush;
#endif
}

// FUNCION PARA PAUSAR PANTALLA
void pausar() {
    cout << "\nPresione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// DATOS DEL SISTEMA
void ingresarDatos(double& area, double& drenaje, double& duracion, double& tiempo,
                   int& intervalos) {
    limpiarPantalla();
    cout << "\t -----------------------------------\n";
    cout << "\t|    INGRESO DE DATOS DEL SISTEMA   |\n";
    cout << "\t -----------------------------------\n\n";
    cout << "Ingrese el area de la cuenca (m2)      : ";
    cin >> area;
    cout << "Ingrese la capacidad de drenaje (m3/h) : ";
    cin >> drenaje;
    cout << "Ingrese la duracion de la lluvia (h)   : ";
    cin >> duracion;
    cout << "Ingrese el tiempo transcurrido (h)     : ";
    cin >> tiempo;
    cout << "Ingrese el numero de intervalos        : ";
    cin >> intervalos;
}

// TIPO DE LLUVIA
int seleccionarLluvia() {
    int tipo;

    do {
        limpiarPantalla();

        cout << "\t -----------------------------------\n";
        cout << "\t|          TIPO DE LLUVIA           |\n";
        cout << "\t -----------------------------------\n";
        cout << "1. Constante\n";
        cout << "2. Creciente\n";
        cout << "3. Variable\n\n";
        cout << "Elija una opcion (1-3): ";
        cin >> tipo;

        if (tipo < 1 || tipo > 3) {
            cout << "Opcion no valida.\n";
            pausar();
        }

    } while (tipo < 1 || tipo > 3);

    return tipo;
}

// ESTRUCTURA PARA ALMACENAR LOS RESULTADOS DE LA SIMULACION
struct Resultados {
    double acumulado;
    double entrada;
    double salida;
};

// TIPO DE LLUVIA - INTENSIDAD DE LLUVIA(mm/h)
double calcularIntensidad(int tipo, double t, double T) {
    switch (tipo) {
        case 1:
            return 30.0;  // CONSTANTE
        case 2:
            return 10.0 + 40.0 * t / T;  // CRECIENTE
        case 3:
            return 10.0 + 40.0 * sin(PI * t / T);  // VARIABLE
        default:
            return 0.0;
    }
}

// CONVERTIR INTENSIDAD DE LLUVIA A CAUDAL DE ENTRADA (m³/h)
double calcularCaudal(double area, double intensidad) { return area * intensidad / 1000.0; }

// SUMAS DE RIEMANN(METODO DEL PUNTO MEDIO) - VOLUMEN ACUMULADO - AREA DEL RECTANGULO
Resultados calcularAcumulacion(int tipo, double area, double drenaje, double t, double T,
                               int intervalos) {
    /* t: TIEMPO TRANSCURRIDO
    T: DURACION DE LA LLUVIA
    */
    double dt = t / intervalos;  // ANCHO DEL INTERVALO(Δt)
    double acumulado = 0;
    double entrada = 0;
    double salida = 0;

    for (int i = 0; i < intervalos; i++) {
        // EXTREMOS DEL SUBINTERVALO DE TIEMPO (ABSCISAS)
        double t1 = i * dt;
        double t2 = (i + 1) * dt;

        // INTENSIDADES DE LLUVIA EN AMBOS EXTREMOS (mm/h)
        double intensidad1 = calcularIntensidad(tipo, t1, T);
        double intensidad2 = calcularIntensidad(tipo, t2, T);

        // CAUDALES DE ENTRADA (BASES DEL TRAPECIO, m³/h)
        double caudal1 = calcularCaudal(area, intensidad1);
        double caudal2 = calcularCaudal(area, intensidad2);

        // VOLUMEN DE ENTRADA: AREA DEL TRAPECIO (m³)
        // dt representa la altura del trapecio
        double aguaEntrante = (caudal1 + caudal2) * dt / 2.0;

        // VOLUMEN DE SALIDA: CAPACIDAD DE DRENAJE POR INTERVALO (m³)
        double aguaSaliente = drenaje * dt;

        // LIMITAMOS EL DRENAJE AL VOLUMEN DE AGUA DISPONIBLE
        if (aguaSaliente > acumulado + aguaEntrante) {
            aguaSaliente = acumulado + aguaEntrante;
        }

        // BALANCE DE VOLUMENES DEL SUBINTERVALO
        acumulado += aguaEntrante - aguaSaliente;

        // SUMATORIA DE LOS VOLUMENES DE ENTRADA Y SALIDA
        entrada += aguaEntrante;
        salida += aguaSaliente;
    }
    return {acumulado, entrada, salida};
}

// FUNCION PARA EXPORTAR LOS DATOS A UN ARCHIVO CSV
void exportarDatos(int tipo, double area, double drenaje, double tiempo, double T, int intervalos) {
    ofstream archivo("datos.csv");

    archivo << "Tiempo,Intensidad,CaudalEntrada,Drenaje,Volumen\n";

    const int divisionesGrafica = 100;

    for (int i = 0; i <= divisionesGrafica; i++) {
        double t = tiempo * i / divisionesGrafica;

        double intensidad = calcularIntensidad(tipo, t, T);

        double caudal = calcularCaudal(area, intensidad);

        Resultados resultado = calcularAcumulacion(tipo, area, drenaje, t, T, intervalos);

        archivo << t << "," << intensidad << "," << caudal << "," << drenaje << ","
                << resultado.acumulado << "\n";
    }

    archivo.close();
}

// MENU PRINCIPAL
int mostrarMenu() {
    int opcion;

    limpiarPantalla();

    cout << "\t -----------------------------------\n";
    cout << "\t|     SIMULADOR DE INUNDACIONES     |\n";
    cout << "\t -----------------------------------\n";
    cout << "1. Datos del sistema\n";
    cout << "2. Resultados de la simulacion\n";
    cout << "3. Mostrar gráfica\n";
    cout << "4. Nueva simulación\n";
    cout << "5. Salir\n\n";

    cout << "Elija una opcion (1-5): ";
    cin >> opcion;

    return opcion;
}

// MODIFICAR LOS DATOS DEL SISTEMA
void modificarDatos(double& area, double& drenaje, double& duracion, double& tiempo,
                    int& intervalos) {
    int opcion;

    do {
        limpiarPantalla();

        cout << "\t -----------------------------------\n";
        cout << "\t|          MODIFICAR DATOS          |\n";
        cout << "\t -----------------------------------\n";
        cout << "1. Area de la cuenca\n";
        cout << "2. Capacidad de drenaje\n";
        cout << "3. Duracion de la lluvia\n";
        cout << "4. Tiempo transcurrido\n";
        cout << "5. Numero de intervalos\n";
        cout << "6. Volver al menu anterior\n\n";
        cout << "Elija una opcion (1-6): ";
        cin >> opcion;

        limpiarPantalla();

        switch (opcion) {
            case 1:
                cout << "Nueva area (m2): ";
                cin >> area;
                break;

            case 2:
                cout << "Nuevo drenaje (m3/h): ";
                cin >> drenaje;
                break;

            case 3:
                cout << "Nueva duracion (h): ";
                cin >> duracion;
                break;

            case 4:
                cout << "Nuevo tiempo transcurrido (h): ";
                cin >> tiempo;
                break;

            case 5:
                cout << "Nuevo numero de intervalos: ";
                cin >> intervalos;
                break;

            case 6:
                break;

            default:
                cout << "Opcion no valida.\n";
                pausar();
        }

    } while (opcion != 6);
}

// FUNCION PRINCIPAL
int main() {
   #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
   #endif
    int opcion, subopcion, tipo, intervalos;
    double caudal;
    double area, drenaje, tiempo, duración;

    limpiarPantalla();

    ingresarDatos(area, drenaje, duración, tiempo, intervalos);

    limpiarPantalla();

    tipo = seleccionarLluvia();

    caudal = calcularCaudal(area, calcularIntensidad(tipo, tiempo, duración));
    Resultados resultado = calcularAcumulacion(tipo, area, drenaje, tiempo, duración, intervalos);
    exportarDatos(tipo, area, drenaje, tiempo, duración, intervalos);

    do {
        opcion = mostrarMenu();

        limpiarPantalla();

        switch (opcion) {
            case 1:
                cout << "\t --------------------------------------\n";
                cout << "\t|      DATOS DEL SISTEMA SIMULADO      |\n";
                cout << "\t --------------------------------------\n\n";
                cout << "AREA DE LA CUENCA      : " << area << " m²" << endl;
                cout << "DURACION DE LA LLUVIA  : " << duración << " h" << endl;
                cout << "TIEMPO TRANSCURRIDO    : " << tiempo << " h" << endl;
                cout << "CAUDAL DE ENTRADA      : " << caudal << " m³/h" << endl;
                cout << "CAPACIDAD DE DRENAJE   : " << drenaje << " m³/h" << endl << endl;
                break;
            case 2:
                cout << "\t --------------------------------------\n";
                cout << "\t|      RESULTADOS DE LA SIMULACION      |\n";
                cout << "\t --------------------------------------\n\n";
                cout << "VOLUMEN DE ENTRADA   : " << resultado.entrada << " m³" << endl;
                cout << "VOLUMEN DE SALIDA    : " << resultado.salida << " m³" << endl;
                cout << "VOLUMEN ACUMULADO    : " << resultado.acumulado << " m³" << endl << endl;
                break;
            case 3:
                exportarDatos(tipo, area, drenaje, tiempo, duración, intervalos);
                  #ifdef _WIN32
                    cout << " Generando gráfica de resultados..." << endl;
                    system("python grafica.py");
                  #else
                    cout << "Datos exportados a datos.csv." << endl;
                    cout << "La gráfica requiere Python en un entorno local." << endl;
                  #endif
                break;
            case 4:

                do {
                    limpiarPantalla();
                    cout << "\t --------------------------------------\n";
                    cout << "\t|        MODIFICAR SIMULACION         |\n";
                    cout << "\t --------------------------------------\n\n";
                    cout << "1. Cambiar datos del sistema" << endl;
                    cout << "2. Cambiar tipo de lluvia" << endl;
                    cout << "3. Volver al menu principal\n" << endl;
                    cout << "Elija una opcion (1-3): ";
                    cin >> subopcion;

                    switch (subopcion) {
                        case 1:
                            limpiarPantalla();
                            modificarDatos(area, drenaje, duración, tiempo, intervalos);
                            break;
                        case 2:
                            limpiarPantalla();
                            tipo = seleccionarLluvia();
                            break;
                        case 3:
                            break;
                        default:
                            cout << "Opcion no valida.\n";
                           pausar();
                    }

                } while (subopcion != 3);
                caudal = calcularCaudal(area, calcularIntensidad(tipo, tiempo, duración));
                resultado = calcularAcumulacion(tipo, area, drenaje, tiempo, duración, intervalos);

                break;
            case 5:
                limpiarPantalla();
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << "Opción no válida.\n" << endl;
        }
        if (opcion != 4 && opcion != 5) {
           pausar();
        }
    } while (opcion != 5);

    return 0;
}
