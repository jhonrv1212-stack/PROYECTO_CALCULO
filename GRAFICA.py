
import csv
import matplotlib.pyplot as plt

tiempos = []
intensidades = []
caudales = []
drenajes = []
volumenes = []

with open("datos.csv", "r") as archivo:
    lector = csv.DictReader(archivo)

    for fila in lector:
        tiempos.append(float(fila["Tiempo"]))
        intensidades.append(float(fila["Intensidad"]))
        caudales.append(float(fila["CaudalEntrada"]))
        drenajes.append(float(fila["Drenaje"]))
        volumenes.append(float(fila["Volumen"]))

fig, graficas = plt.subplots(3, 1, figsize=(10, 9), sharex=True)

# GRAFICA 1: INTENSIDAD DE LLUVIA
graficas[0].plot(tiempos, intensidades, color="blue", linewidth=2)
graficas[0].set_title("Intensidad de lluvia")
graficas[0].set_ylabel("Intensidad (mm/h)")
graficas[0].grid(True)

# GRAFICA 2: CAUDAL DE ENTRADA Y DRENAJE
graficas[1].plot(tiempos, caudales, color="green",
                 linewidth=2, label="Caudal de entrada")
graficas[1].plot(tiempos, drenajes, color="red",
                 linestyle="--", linewidth=2,
                 label="Capacidad de drenaje")
graficas[1].set_title("Entrada de agua y capacidad de drenaje")
graficas[1].set_ylabel("Caudal (m³/h)")
graficas[1].legend()
graficas[1].grid(True)

# GRAFICA 3: VOLUMEN ACUMULADO
graficas[2].plot(tiempos, volumenes, color="purple", linewidth=2)
graficas[2].set_title("Evolucion del volumen acumulado")
graficas[2].set_xlabel("Tiempo transcurrido (horas)")
graficas[2].set_ylabel("Volumen (m³)")
graficas[2].grid(True)

fig.suptitle("SIMULADOR DE INUNDACIONES URBANAS", fontsize=14)
plt.tight_layout()
plt.show()
