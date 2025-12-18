/* cajero_file.c
Compilar con: gcc -std=c99 -Wall -Wextra -o cajero cajero_file.c
*/

#include <stdio.h>
#include <string.h>

#define NOMBRE_LEN 50
#define ARCHIVO_USUARIO "usuario.txt"

struct Usuario {
	char nombre[NOMBRE_LEN];
	int pin;
	float saldo;
};

/* Prototipos */
int login(const struct Usuario *u);
void menu(struct Usuario *u);
void verSaldo(const struct Usuario *u);
void sacar(struct Usuario *u);
void depositar(struct Usuario *u);
void cambiarPin(struct Usuario *u);
int guardarDatos(const struct Usuario *u);
int cargarDatos(struct Usuario *u);
void limpiarEntrada(void);

/* main */
int main(void) {
	struct Usuario u;
	
	if (!cargarDatos(&u)) {
		/* Crear usuario por defecto y guardar */
		strncpy(u.nombre, "JUAN DE GLEE", NOMBRE_LEN - 1);
		u.nombre[NOMBRE_LEN - 1] = '\0';
		u.pin = 1234;
		u.saldo = 10000.0f;
		guardarDatos(&u);
		printf("Usuario por defecto creado.\n");
	}
	
	if (login(&u)) {
		printf("\nBienvenido %s\n", u.nombre);
		menu(&u);
	} else {
		printf("\nTarjeta bloqueada.\n");
	}
	
	return 0;
}

/* Limpia sobrantes del stdin tras scanf */
void limpiarEntrada(void) {
	int c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Guardar en archivo. Devuelve 1 si ok, 0 si error */
int guardarDatos(const struct Usuario *u) {
	FILE *f = fopen(ARCHIVO_USUARIO, "w");
	if (!f) {
		perror("guardarDatos: fopen");
		return 0;
	}
	
	/* Formato: nombre (línea), pin, saldo */
	fprintf(f, "%s\n%d\n%.2f\n", u->nombre, u->pin, u->saldo);
	fclose(f);
	return 1;
}

/* Cargar desde archivo. Devuelve 1 si ok, 0 si no existe/error */
int cargarDatos(struct Usuario *u) {
	FILE *f = fopen(ARCHIVO_USUARIO, "r");
	if (!f) return 0;
	
	/* Leer nombre completo (línea) y luego pin y saldo */
	if (fgets(u->nombre, NOMBRE_LEN, f) == NULL) {
		fclose(f);
		return 0;
	}
	/* quitar posible salto de línea */
	u->nombre[strcspn(u->nombre, "\r\n")] = '\0';
	
	if (fscanf(f, "%d", &u->pin) != 1) {
		fclose(f);
		return 0;
	}
	if (fscanf(f, "%f", &u->saldo) != 1) {
		fclose(f);
		return 0;
	}
	
	fclose(f);
	return 1;
}

/* Login: 3 intentos (0-2). Devuelve 1 si ok, 0 si falla */
int login(const struct Usuario *u) {
	int p;
	int intentos = 0;
	while (intentos < 3) {
		printf("Ingrese PIN: ");
		if (scanf("%d", &p) != 1) {
			printf("Entrada invalida.\n");
			limpiarEntrada();
			intentos++;
			continue;
		}
		limpiarEntrada();
		if (p == u->pin) return 1;
		printf("PIN incorrecto.\n");
		intentos++;
	}
	return 0;
}

/* Menu principal */
void menu(struct Usuario *u) {
	int op = 0;
	do {
		printf("\n--- MENU ---\n");
		printf("1. Ver saldo\n");
		printf("2. Sacar dinero\n");
		printf("3. Depositar dinero\n");
		printf("4. Salir\n");
		printf("5. Cambiar PIN\n");
		printf("Opcion: ");
		if (scanf("%d", &op) != 1) {
			printf("Opcion no valida.\n");
			limpiarEntrada();
			op = 0;
			continue;
		}
		limpiarEntrada();
		
		switch (op) {
		case 1:
			verSaldo(u);
			break;
		case 2:
			sacar(u);
			guardarDatos(u);
			break;
		case 3:
			depositar(u);
			guardarDatos(u);
			break;
		case 4:
			printf("Hasta luego.\n");
			break;
		case 5:
			cambiarPin(u);
			guardarDatos(u);
			break;
		default:
			printf("Opcion no valida.\n");
		}
	} while (op != 4);
}

void verSaldo(const struct Usuario *u) {
	printf("Saldo actual: $%.2f\n", u->saldo);
}

void sacar(struct Usuario *u) {
	float m;
	printf("Monto a retirar: $");
	if (scanf("%f", &m) != 1) {
		printf("Entrada invalida.\n");
		limpiarEntrada();
		return;
	}
	limpiarEntrada();
	
	if (m <= 0.0f) {
		printf("Monto invalido.\n");
		return;
	}
	
	if (m > u->saldo) {
		printf("Saldo insuficiente. Operacion cancelada.\n");
		return;
	}
	
	u->saldo -= m;
	printf("Retiro realizado. Nuevo saldo: $%.2f\n", u->saldo);
}

void depositar(struct Usuario *u) {
	float m;
	printf("Monto a depositar: $");
	if (scanf("%f", &m) != 1) {
		printf("Entrada invalida.\n");
		limpiarEntrada();
		return;
	}
	limpiarEntrada();
	
	if (m <= 0.0f) {
		printf("Monto invalido.\n");
		return;
	}
	
	u->saldo += m;
	printf("Deposito hecho. Nuevo saldo: $%.2f\n", u->saldo);
}

void cambiarPin(struct Usuario *u) {
	int pinActual, nuevoPin, confirmarPin;
	
	printf("Ingrese el PIN actual: ");
	if (scanf("%d", &pinActual) != 1) {
		printf("Entrada invalida.\n");
		limpiarEntrada();
		return;
	}
	limpiarEntrada();
	
	if (pinActual != u->pin) {
		printf("PIN incorrecto. No es posible cambiarlo.\n");
		return;
	}
	
	printf("Ingrese el nuevo PIN (4 digitos recomendado): ");
	if (scanf("%d", &nuevoPin) != 1) {
		printf("Entrada invalida.\n");
		limpiarEntrada();
		return;
	}
	limpiarEntrada();
	
	printf("Confirme el nuevo PIN: ");
	if (scanf("%d", &confirmarPin) != 1) {
		printf("Entrada invalida.\n");
		limpiarEntrada();
		return;
	}
	limpiarEntrada();
	
	if (nuevoPin != confirmarPin) {
		printf("Los PIN no coinciden. Intente de nuevo.\n");
		return;
	}
	
	u->pin = nuevoPin;
	printf("PIN cambiado con exito.\n");
}
