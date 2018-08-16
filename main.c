#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

//COLORES
#define RESET    "\x1B[0m"
#define ROJO     "\x1B[31m"
#define VERDE    "\x1B[32m"
#define AMARILLO "\x1B[33m"
#define AZUL     "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CELESTE  "\x1B[36m"

//STRUCTS
struct Particion
{
    char part_status;    //Indica si esta esta vacia, montada o desmontada
    char part_type;      //Indica si es primaria, extendida o logica
    char part_fit;       //Indica que ajuste usar
    int part_start;     //Indica el byte del disco donde inicia
    int part_size;      //Indica el tamano de la particion
    char part_name[16];  //Indica el nombre de la particion
};

struct MBR
{
    int mbr_tamano;
    time_t*	mbr_fecha_creacion;
    int mbr_disk_signature;
    struct Particion mbr_partition_1;
    struct Particion mbr_partition_2;
    struct Particion mbr_partition_3;
    struct Particion mbr_partition_4;
};

struct EBR
{
    char 	part_status;
    char 	part_fit;
    int   part_start;
    int 	part_size;
    int 	part_next;
    int   part_previous;
    char	part_name[16];
};

//Variables
char buffer[1];
int Multiplicador;
int convertido;
char Montador[100][100][100];
char Normal[500];
char Linea_Comparable[500];
char Comando[500]; //Guarda el comando ejecutandose
char Abecedario[28]; //Todo el abecedario en un array

//Flags
int c_true;
int TAG; //Una bandera x que sirve para todo y nada a la ves
int TAG_Script = 0;
int aux_int;
int Particiones_N = 0;
int Particiones_Ini = 0;
int Particiones_Fin = 0;
int EBR_Ini = 0;

//Cont
int iWhile;
int salvador = 0;

//Declaración de Metodos Sin Parametros
void Ingresar_Comando();
void Analizar_Comando(char *linea, char *palabra);

//Declaracion de Metodos Con Parametros
void Crear_Disco(char size[10], char name[20], char ruta_Disco[100]);
void Eliminar_Disco(char path[100]);
void Crear_Particion(char name[20], char size[10], char unit[2], char fit[2], char type[1], char path[100]);
void Crear_Particion_Logica(FILE *f, int inicio, int limite, char fit[2], char name[20], int size);
void Eliminar_Particiones(char delet[4], char name[20], char path[100]);
void Add_Espacio(int size, char ruta_Disco[100], char name[20], char unit[2]);

int main()
{
    system("clear");
    printf(AMARILLO "************************************************************\n" RESET);
    printf(AMARILLO "*              SISTEMA DE ARCHIVO 201213283                *\n" RESET);
    printf(AMARILLO "************************************************************\n" RESET);
    time_t t;
    srand((unsigned) time(&t));
    strcpy(Abecedario,"abcdefghijklmnopqrstuvwxyz");
    printf(VERDE ">>PARA SALIR DE LA APLICACIÓN, ESCRIBA LA PALABRA  \"" CELESTE "EXIT" VERDE "\"\n" RESET);
    Ingresar_Comando();
    printf(MAGENTA "Espero ganar MIA esta vez :S\n" RESET);
    return 0;
}

//###################################################################################################################################
//#																				           RECONOZCO EL COMANDO 																												  #
//###################################################################################################################################
void Ingresar_Comando()
{
    printf(VERDE "INGRESE COMANDO>>>>>  \n" RESET);
    char temp[500];
    TAG = 0;
    scanf(" %[^\n]s", Normal);
    if (Normal[strlen(Normal)-1] == '\\')
    {
        scanf(" %[^\n]s", temp);
        strncpy(Linea_Comparable, Normal, strlen(Normal)-1);
        strcat(Linea_Comparable, temp);
        strcpy(Normal, Linea_Comparable);
        char *Lista = strtok(Linea_Comparable, " ");
        strcpy(Comando, Normal);
        Analizar_Comando(Normal, Lista);
    }
    else
    {
        strcpy(Comando, Normal);
        strcpy(Linea_Comparable, Normal);
        char *Lista = strtok(Normal, " ");
        Analizar_Comando(Linea_Comparable, Lista);
    }
}

//###################################################################################################################################
//#																			        ELIMINAR Y CREAR DIRECTORIOS 																												#
//###################################################################################################################################
void Crear_Directorios_Reales(char path[100])
{
    if(path[0] == '/')
    {
        char terminal_cmd[300];
        strcpy (terminal_cmd,  "mkdir -p '");
        strncat(terminal_cmd, path, strlen(path)-1);
        strcat(terminal_cmd, "'");
        system(terminal_cmd);
    }
}

void Crear_Directorios_Con_Nombre(char path[100])
{
    const char x = '/';
    char *fin;
    if(path[0] == '/')
    {
        fin = strrchr(path, x);
        char terminal_cmd[300];
        strcpy (terminal_cmd,  "mkdir -p '");
        strncat(terminal_cmd, path, strlen(path)-strlen(fin));
        strcat(terminal_cmd, "'");
        system(terminal_cmd);
    }
}

void Eliminar_Ultima_Carpeta(char path[100])
{
    if(path[0]=='/')
    {
        char terminal_cmd[300];
        strcpy (terminal_cmd, "rm -R '");
        strncat(terminal_cmd, path, strlen(path)-1);
        strcat(terminal_cmd, "'");
        system(terminal_cmd);
    }
}

//###################################################################################################################################
//#																					             CREAR DISCO																																#
//###################################################################################################################################
void Crear_Disco(char size[10], char name[20], char ruta_Disco[100])
{
    if(Multiplicador>=1)
    {
        struct MBR mbr;
        //Inicializo los datos del disco
        convertido = (int) strtol(size, (char **)NULL, 10);
        mbr.mbr_tamano = convertido * 1024 * Multiplicador;
        time(mbr.mbr_fecha_creacion);
        mbr.mbr_disk_signature = (rand() % 26);
        mbr.mbr_partition_1.part_status = 'n';
        mbr.mbr_partition_2.part_status = 'n';
        mbr.mbr_partition_3.part_status = 'n';
        mbr.mbr_partition_4.part_status = 'n';

        //Inicio de la escritura del disco
        char ruta_temp[120];
        strcpy(ruta_temp, ruta_Disco);
        //strcat(ruta_temp, "/");
        //strcat(ruta_temp, name);
        FILE *f_disco = fopen(ruta_temp, "wb+");
        for(int iFor=0; iFor < mbr.mbr_tamano; iFor++)
        {
            fwrite(buffer, sizeof(buffer), 1, f_disco);
        }//Fin del for que escribe el disco

        rewind(f_disco);
        fwrite(&mbr, sizeof(mbr), 1, f_disco);
        fclose(f_disco);

        printf("\t>>" VERDE "Disco creado exitosamente.\n" RESET);
    }
    else
    {
        printf("\t>" ROJO "El tamano minimo de un disco es de 1K.\n" RESET);
    }
}

//###################################################################################################################################
//#																					             ELIMINAR DISCO																															#
//###################################################################################################################################
void Eliminar_Disco(char path[100])
{
    char SoN[1];
    printf("¿Deseas eliminar el disco? [S/N]\n" RESET);
    scanf("%s", SoN);
    if(strcasecmp(SoN,"S")==0)
    {
        if(fopen (path, "r") == NULL)
        {
            printf("\t>" ROJO "El disco indicado no existe. Intentelo nuevamente...\n" RESET);
        }
        else
        {
            if(remove(path) == 0)
            {
                printf("\t>>" VERDE "Disco eliminado.\n" RESET);
            }
            else
            {
                printf("\t>" ROJO "Disco en uso, cierre o desmonte el disco y luego intentelo nuevamente.\n" RESET);
            }
        }
    }
}

//###################################################################################################################################
//#																					           CREAR PARTICION																															#
//###################################################################################################################################
void Crear_Particion(char name[20], char size[10], char unit[2], char fit[2], char type[1], char path[100])
{
    struct MBR mbr;
    FILE *f;
    if((fopen (path, "rb+")) != NULL)
    {
        f = fopen (path, "rb+");
        fread (&mbr, sizeof(mbr), 1, f);

        //contar particiones
        TAG = 0;
        aux_int = 0;
        Particiones_N = 0;
        Particiones_Ini = 0;
        Particiones_Fin = 0;
        if(mbr.mbr_partition_1.part_status != 'n')
        {
            Particiones_N++;
            if(mbr.mbr_partition_1.part_type == 'e')
            {
                aux_int++;
            }
        }
        if(mbr.mbr_partition_2.part_status != 'n')
        {
            Particiones_N++;
            if(mbr.mbr_partition_2.part_type == 'e')
            {
                aux_int++;
            }
        }
        if(mbr.mbr_partition_3.part_status != 'n')
        {
            Particiones_N++;
            if(mbr.mbr_partition_3.part_type == 'e')
            {
                aux_int++;
            }
        }
        if(mbr.mbr_partition_4.part_status != 'n')
        {
            Particiones_N++;
            if(mbr.mbr_partition_4.part_type == 'e')
            {
                aux_int++;
            }
        }

        if(strcasecmp(unit, "k") == 0)
        {
            Multiplicador = 1024;
        }
        else if(strcasecmp(unit, "m") == 0)
        {
            Multiplicador = 1024 * 1024;
        }
        else if(strcasecmp(unit, "") == 0)
        {
            Multiplicador = 1024;
        }
        else if(strcasecmp(unit, "b") == 0)
        {
            Multiplicador = 1;
        }
        else
        {
            Multiplicador = 0;
        }

        char charFit;
        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0) || (fit==NULL))
        {
            charFit = 'w';
        }
        else if((strcasecmp(fit,"bf") == 0))
        {
            charFit = 'b';
        }
        else if((strcasecmp(fit,"ff") == 0))
        {
            charFit = 'f';
        }
        else
        {
            charFit = 'n';
        }

        if(strcasecmp(type,"l") != 0)
        {

            if(Multiplicador != 0)
            {
                if(Particiones_N == 4)
                {
                    printf("\t>" ROJO "Solo se pueden crear 4 particiones.\n" RESET);
                }
                else if(Particiones_N == 0)
                {
                    if(mbr.mbr_tamano >= strtol(size, (char**)NULL, 10)*Multiplicador)
                    {
                        mbr.mbr_partition_1.part_status = 'd';
                        mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                        mbr.mbr_partition_1.part_start = sizeof(mbr);
                        strcpy(mbr.mbr_partition_1.part_name, name);

                        if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                        {
                            mbr.mbr_partition_1.part_fit = 'w';
                        }
                        else if ((strcasecmp(fit,"bf") == 0))
                        {
                            mbr.mbr_partition_1.part_fit = 'b';
                        }
                        else
                        {
                            mbr.mbr_partition_1.part_fit = 'f';
                        }

                        if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                        {
                            mbr.mbr_partition_1.part_type = 'p';
                        }
                        else if(strcasecmp(type,"e") == 0)
                        {
                            if(aux_int == 0)
                            {
                                mbr.mbr_partition_1.part_type = 'e';

                                struct EBR ebr;
                                ebr.part_next= -1;
                                ebr.part_start = sizeof(mbr) + sizeof(ebr);
                                ebr.part_status = 'n';
                                fseek(f,sizeof(mbr),SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                            }
                            else
                            {
                                TAG = 1;
                                printf("\t>" ROJO "¡Solo puede haber una particion extendida!\n" RESET);
                            }
                        }

                        if(TAG == 0)
                        {
                            fseek(f, 0, SEEK_SET);
                            fwrite(&mbr, sizeof(mbr), 1, f);
                            printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                        }
                        else
                        {
                            printf("\t>" ROJO "No se pudieron guardar los cambios\n" RESET);
                        }
                    }
                    else
                    {
                        printf("\t>" ROJO "No hay espacio suficiente en el disco.\n" RESET);
                    }
                }
                else if(Particiones_N == 1)
                {

                    if(mbr.mbr_partition_1.part_status != 'n')
                    {
                        if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                        {
                            Particiones_Ini = mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                        else if(mbr.mbr_partition_1.part_start > sizeof(mbr))
                        {
                            Particiones_Ini = sizeof(mbr);
                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                            if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                            {
                                Particiones_Ini = mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size;
                                Particiones_Fin = mbr.mbr_tamano;
                            }
                        }
                        else
                        {
                            printf("\t>" ROJO "Se leyo mal el archivo.\n" RESET);
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_2.part_status = 'd';
                            mbr.mbr_partition_2.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_2.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_2.part_name, name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_2.part_fit = 'w';
                            }
                            else if ((strcasecmp(fit,"bf") == 0))
                            {
                                mbr.mbr_partition_2.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_2.part_fit = 'f';
                            }
                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_2.part_type = 'p';
                            }
                            else if(strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_2.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_2.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                    else if(mbr.mbr_partition_2.part_status != 'n')
                    {
                        if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                        {
                            Particiones_Ini = mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                        else if(mbr.mbr_partition_2.part_start > sizeof(mbr))
                        {
                            Particiones_Ini = sizeof(mbr);
                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                            if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                            {
                                Particiones_Ini = mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size;
                                Particiones_Fin = mbr.mbr_tamano;
                            }
                        }
                        else
                        {
                            printf("\t>" ROJO "Se leyo mal el archivo.\n" RESET);
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_1.part_status = 'a';
                            mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_1.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_1.part_name, name);

                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_1.part_fit = 'w';
                            }
                            else if((strcasecmp(fit,"bf") == 0))
                            {
                                mbr.mbr_partition_1.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_1.part_fit = 'f';
                            }

                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_1.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_1.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios\n" RESET);
                            }
                        }
                    }
                    else if(mbr.mbr_partition_3.part_status != 'n')
                    {
                        if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                        {
                            Particiones_Ini = mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                        else if(mbr.mbr_partition_3.part_start > sizeof(mbr))
                        {
                            Particiones_Ini = sizeof(mbr);
                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                            if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                            {
                                Particiones_Ini = mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size;
                                Particiones_Fin = mbr.mbr_tamano;
                            }
                        }
                        else
                        {
                            printf("\t>" ROJO "Se leyo mal el archivo.\n" RESET);
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_1.part_status = 'a';
                            mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_1.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_1.part_name, name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_1.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_1.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_1.part_fit = 'f';
                            }
                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_1.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_1.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                    else if(mbr.mbr_partition_4.part_status != 'n')
                    {
                        if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                        {
                            Particiones_Ini = mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size;
                            Particiones_Fin = mbr.mbr_tamano;
                        }
                        else if(mbr.mbr_partition_4.part_start > sizeof(mbr))
                        {
                            Particiones_Ini = sizeof(mbr);
                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                            if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                            {
                                Particiones_Ini = mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size;
                                Particiones_Fin = mbr.mbr_tamano;
                            }
                        }
                        else
                        {
                            printf("\t>" ROJO "Se leyo mal el archivo.\n" RESET);
                        }
                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_1.part_status = 'a';
                            mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_1.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_1.part_name, name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_1.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_1.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_1.part_fit = 'f';
                            }
                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_1.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_1.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                    else
                    {
                        printf("\t>" ROJO "Se leyo mal el archivo.\n" RESET);
                    }
                }
                else if(Particiones_N == 2)
                {
                    if(mbr.mbr_partition_1.part_status == 'n')
                    {
                        if((mbr.mbr_partition_2.part_status != 'n')&&(mbr.mbr_partition_3.part_status != 'n'))
                        {
                            if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start))
                            {
                                if((mbr.mbr_partition_2.part_start == sizeof(mbr)))
                                {
                                    if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_3.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_3.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size == mbr.mbr_partition_2.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_3.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_3.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }
                        else if((mbr.mbr_partition_2.part_status != 'n')&&(mbr.mbr_partition_4.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_2.part_start < mbr.mbr_partition_4.part_start)
                            {
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_4.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_4.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_2.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)==mbr.mbr_partition_4.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }
                        else if((mbr.mbr_partition_3.part_status != 'n')&&(mbr.mbr_partition_4.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start)
                            {
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size == mbr.mbr_partition_4.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)==mbr.mbr_partition_4.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_3.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_3.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)==mbr.mbr_partition_4.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }//ya termino

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_1.part_status = 'a';
                            mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_1.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_1.part_name, name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_1.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_1.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_1.part_fit = 'f';
                            }

                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_1.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_1.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }


                    }
                    else if(mbr.mbr_partition_2.part_status == 'n')
                    {
                        if((mbr.mbr_partition_1.part_status != 'n')&&(mbr.mbr_partition_4.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_1.part_start < mbr.mbr_partition_4.part_start)
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_4.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_4.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_1.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_4.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }
                        else if((mbr.mbr_partition_1.part_status != 'n')&&(mbr.mbr_partition_1.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_1.part_start < mbr.mbr_partition_1.part_start)
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_1.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_1.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_1.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_1.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }
                        else if((mbr.mbr_partition_4.part_status != 'n')&&(mbr.mbr_partition_1.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start)
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_1.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_4.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }


                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_2.part_status = 'a';
                            mbr.mbr_partition_2.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_2.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_2.part_name,name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_2.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_2.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_2.part_fit = 'f';
                            }

                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_2.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_2.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_2.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                    else if(mbr.mbr_partition_3.part_status == 'n')
                    {
                        if((mbr.mbr_partition_4.part_status != 'n')&&(mbr.mbr_partition_1.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start)
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_1.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_4.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_1.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }
                        else if((mbr.mbr_partition_4.part_status != 'n')&&(mbr.mbr_partition_2.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_4.part_start < mbr.mbr_partition_2.part_start)
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size == mbr.mbr_partition_2.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_2.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_4.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_4.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)==mbr.mbr_partition_2.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }
                        else if((mbr.mbr_partition_1.part_status != 'n')&&(mbr.mbr_partition_2.part_status != 'n'))
                        {
                            if(mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start)
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size == mbr.mbr_partition_2.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_2.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_1.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size == mbr.mbr_partition_1.part_start)
                                    {
                                        if(((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))>=strtol(size, (char**)NULL, 10)*Multiplicador)&&(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)==mbr.mbr_partition_2.part_start)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                        }
                                    }
                                }
                                else if(mbr.mbr_partition_2.part_start-sizeof(mbr)>= strtol(size, (char**)NULL, 10)*Multiplicador)
                                {
                                    Particiones_Ini = sizeof(mbr);
                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size)>= strtol(size, (char**)NULL, 10)*Multiplicador))
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador )
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_tamano;
                                    }
                                    else
                                    {
                                        printf("\t>" ROJO "No hay suficiente espacio para crear la particion.\n" RESET);
                                    }
                                }
                            }
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("error, no hay espacio suficiente, %d,%d",Particiones_Fin, Particiones_Ini);
                        }
                        else
                        {
                            mbr.mbr_partition_3.part_status = 'd';
                            mbr.mbr_partition_3.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_3.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_3.part_name,name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_3.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_3.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_3.part_fit = 'f';
                            }
                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_3.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_3.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_3.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }


                    }
                    else
                    {
                        printf(">Algo salio mal :(\n" RESET);
                    }
                }
                else if(Particiones_N == 3)
                {
                    if(mbr.mbr_partition_1.part_status == 'n')
                    {
                        if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start))
                        {
                            if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_4.part_start))
                            {
                                if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start))
                                {
                                    if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }

                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start))
                            {
                                if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_4.part_start))
                                {
                                    if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }

                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_1.part_status = 'a';
                            mbr.mbr_partition_1.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_1.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_1.part_name, name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_1.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_1.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_1.part_fit = 'f';
                            }
                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_1.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_1.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_1.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                    else if(mbr.mbr_partition_2.part_status == 'n')
                    {
                        if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_4.part_start))
                        {
                            if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_1.part_start))
                            {
                                if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start))
                                {
                                    if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start))
                            {
                                if((mbr.mbr_partition_3.part_start < mbr.mbr_partition_1.part_start))
                                {
                                    if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_2.part_status = 'a';
                            mbr.mbr_partition_2.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_2.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_2.part_name,name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_2.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_2.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_2.part_fit = 'f';
                            }
                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_2.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_2.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_2.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                    else if(mbr.mbr_partition_3.part_status == 'n')
                    {
                        if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_1.part_start))
                        {
                            if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_2.part_start))
                            {
                                if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start))
                                {
                                    if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_4.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_4.part_start;
                                    }
                                    else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start))
                            {
                                if((mbr.mbr_partition_4.part_start < mbr.mbr_partition_2.part_start))
                                {
                                    if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_4.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_4.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if(mbr.mbr_partition_4.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_4.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_4.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_3.part_status = 'a';
                            mbr.mbr_partition_3.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_3.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_3.part_name,name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_3.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_3.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_3.part_fit = 'f';
                            }

                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_3.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_3.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_3.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                    else if(mbr.mbr_partition_4.part_status == 'n')
                    {
                        if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_2.part_start))
                        {
                            if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_3.part_start))
                            {
                                if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start))
                                {
                                    if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_1.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_2.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_2.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_1.part_start;
                                    }
                                    else if(mbr.mbr_partition_2.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if((mbr.mbr_partition_2.part_start < mbr.mbr_partition_3.part_start))
                            {
                                if((mbr.mbr_partition_1.part_start < mbr.mbr_partition_3.part_start))
                                {
                                    if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_3.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start == (mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_partition_3.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if(mbr.mbr_partition_2.part_start == sizeof(mbr))
                                    {
                                        if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_2.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = sizeof(mbr);
                                            Particiones_Fin = mbr.mbr_partition_2.part_start;
                                        }
                                        else if(mbr.mbr_partition_3.part_start==(sizeof(mbr)+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                            {
                                                if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                            else
                                            {
                                                if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                    Particiones_Fin = mbr.mbr_partition_1.part_start;
                                                }
                                                else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                                {
                                                    Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                    Particiones_Fin = mbr.mbr_tamano;
                                                }
                                                else
                                                {
                                                    printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                                }
                                            }
                                        }
                                        else if((mbr.mbr_partition_3.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_3.part_start;
                                        }
                                        else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(mbr.mbr_partition_3.part_start == sizeof(mbr))
                                {
                                    if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                                else
                                {
                                    if((mbr.mbr_partition_3.part_start-sizeof(mbr)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                    {
                                        Particiones_Ini = sizeof(mbr);
                                        Particiones_Fin = mbr.mbr_partition_3.part_start;
                                    }
                                    else if(mbr.mbr_partition_2.part_start==(sizeof(mbr)+mbr.mbr_partition_3.part_size))
                                    {
                                        if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                        {
                                            if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                        else
                                        {
                                            if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                                Particiones_Fin = mbr.mbr_partition_1.part_start;
                                            }
                                            else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                            {
                                                Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                                Particiones_Fin = mbr.mbr_tamano;
                                            }
                                            else
                                            {
                                                printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                            }
                                        }
                                    }
                                    else if((mbr.mbr_partition_2.part_start-(mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)  // fragmentacion entre 2 y 3
                                    {
                                        Particiones_Ini = mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size;
                                        Particiones_Fin = mbr.mbr_partition_2.part_start;
                                    }
                                    else if(mbr.mbr_partition_1.part_start == (mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size))
                                    {
                                        if(mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                    else
                                    {
                                        if((mbr.mbr_partition_1.part_start-(mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size;
                                            Particiones_Fin = mbr.mbr_partition_1.part_start;
                                        }
                                        else if((mbr.mbr_tamano-(mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size)) >= strtol(size, (char**)NULL, 10)*Multiplicador)
                                        {
                                            Particiones_Ini = mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size;
                                            Particiones_Fin = mbr.mbr_tamano;
                                        }
                                        else
                                        {
                                            printf("\t>" ROJO "No hay espacio suficiente para crear la particion deseada.\n" RESET);
                                        }
                                    }
                                }
                            }
                        }

                        if((Particiones_Fin-Particiones_Ini) < strtol(size, (char**)NULL, 10)*Multiplicador)
                        {
                            printf("\t>" ROJO "No hay espacio suficiente.\n" RESET);
                        }
                        else
                        {
                            mbr.mbr_partition_4.part_status = 'a';
                            mbr.mbr_partition_4.part_size = strtol(size, (char**)NULL, 10)*Multiplicador;
                            mbr.mbr_partition_4.part_start = Particiones_Ini;
                            strcpy(mbr.mbr_partition_4.part_name,name);
                            if((strcasecmp(fit,"") == 0)||(strcasecmp(fit,"wf") == 0))
                            {
                                mbr.mbr_partition_4.part_fit = 'w';
                            }
                            else if(strcasecmp(fit,"bf") == 0)
                            {
                                mbr.mbr_partition_4.part_fit = 'b';
                            }
                            else
                            {
                                mbr.mbr_partition_4.part_fit = 'f';
                            }

                            if((strcasecmp(type,"") == 0)||(strcasecmp(type,"p") == 0))
                            {
                                mbr.mbr_partition_4.part_type = 'p';
                            }
                            else if (strcasecmp(type,"e") == 0)
                            {
                                if(aux_int == 0)
                                {
                                    mbr.mbr_partition_4.part_type = 'e';

                                    struct EBR ebr;
                                    ebr.part_next= -1;
                                    ebr.part_start = mbr.mbr_partition_4.part_start+sizeof(ebr);
                                    ebr.part_status = 'n';
                                    fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                                    fwrite(&ebr, sizeof(ebr), 1, f);
                                    printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                                }
                                else
                                {
                                    TAG = 1;
                                    printf("\t>" ROJO "Solo puede haber una particion extendida.\n" RESET);
                                }
                            }

                            if(TAG == 0)
                            {
                                fseek(f, 0, SEEK_SET);
                                fwrite(&mbr, sizeof(mbr), 1, f);
                                printf("\t>>" VERDE "Particion creada exitosamente!\n" RESET);
                            }
                            else
                            {
                                printf("\t>" ROJO "No se pudieron guardar los cambios.\n" RESET);
                            }
                        }
                    }
                }
            }
            else
            {
                printf("\t>Unit invalido, tipos disponibles: P, E y L.\n" RESET);
            }
        }
        else
        {

            if(aux_int == 0)
            {
                printf("\t>" ROJO "No existe una particion extendida en el disco.\n" RESET);
            }
            else
            {
                if(mbr.mbr_partition_1.part_type == 'e')
                {

                    Crear_Particion_Logica(f, mbr.mbr_partition_1.part_start,mbr.mbr_partition_1.part_start+mbr.mbr_partition_1.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

                }
                else if(mbr.mbr_partition_2.part_type == 'e')
                {

                    Crear_Particion_Logica(f, mbr.mbr_partition_2.part_start,mbr.mbr_partition_2.part_start+mbr.mbr_partition_2.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

                }
                else if(mbr.mbr_partition_3.part_type == 'e')
                {

                    Crear_Particion_Logica(f, mbr.mbr_partition_3.part_start,mbr.mbr_partition_3.part_start+mbr.mbr_partition_3.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

                }
                else if(mbr.mbr_partition_4.part_type == 'e')
                {

                    Crear_Particion_Logica(f, mbr.mbr_partition_4.part_start,mbr.mbr_partition_4.part_start+mbr.mbr_partition_4.part_size, charFit, name, strtol(size, (char**)NULL, 10) * Multiplicador);

                }
                else
                {
                    printf("Algo esta mal :(\n" RESET);
                }
            }
        }
        fclose(f);
    }
    else
    {
        printf("\t>" ROJO "¡El disco no existe!\n" RESET);
    }
}

//###################################################################################################################################
//#																					         CREAR PARTICION LOGICA																													#
//###################################################################################################################################
void Crear_Particion_Logica(FILE *f, int inicio, int limite, char fit[2], char name[20], int size)
{
    struct EBR ebr;
    struct EBR ebrNuevo;
    fseek(f, inicio, SEEK_SET);
    fread(&ebr, sizeof(ebr), 1, f);

    if(ebr.part_next == -1)
    {
        if(inicio + sizeof(ebr) + size <= limite)
        {
            ebr.part_fit = fit;
            strcpy(ebr.part_name, name);
            ebr.part_next = inicio + sizeof(ebr) + size;
            ebr.part_size = size;
            ebr.part_start = inicio;
            ebr.part_status = 'd';

            ebrNuevo.part_next= -1;
            ebrNuevo.part_start = ebr.part_next + sizeof(ebr);
            ebrNuevo.part_previous= inicio;
            ebrNuevo.part_status = 'n';
            fseek(f, inicio, SEEK_SET);
            fwrite(&ebr, sizeof(ebr), 1, f);
            fseek(f, ebr.part_next, SEEK_SET);
            fwrite(&ebrNuevo, sizeof(ebr), 1, f);
            printf(VERDE ">> Particion logica creada exitosamente\n" RESET);
        }
        else
        {
            printf("\t>" ROJO "No hay espacio suficiente en la particion extendida.\n" RESET);
        }

    }
    else
    {
        if(ebr.part_next == ebr.part_size+ebr.part_start)
        {
            Crear_Particion_Logica(f, ebr.part_next, limite, fit, name, size);
        }
        else
        {
            if(ebr.part_next -(ebr.part_size+ebr.part_start) >= size)
            {
                ebrNuevo.part_next= ebr.part_next;
                ebrNuevo.part_start = ebr.part_start + sizeof(ebr) + ebr.part_size + sizeof(ebr);
                ebrNuevo.part_previous= inicio;
                ebrNuevo.part_status = 'd';

                ebr.part_next = ebr.part_start + sizeof(ebr) + ebr.part_size;
                fseek(f, inicio, SEEK_SET);
                fwrite(&ebr, sizeof(ebr), 1, f);
                fseek(f, ebr.part_next, SEEK_SET);
                fwrite(&ebrNuevo, sizeof(ebr), 1, f);
                printf(VERDE ">> Particion logica creada exitosamente\n" RESET);
            }
            else
            {
                Crear_Particion_Logica(f, ebr.part_next, limite, fit, name, size);
            }
        }
    }
}

//###################################################################################################################################
//#																					         ELIMINAR PARTICIONES 																													#
//###################################################################################################################################
void Eliminar_Particiones(char delet[4], char name[20], char path[250])
{
    char SoN[1];
    printf("¿Desea borrar la particion? [S/N]\n" RESET);
    scanf("%s", SoN);
    if(strcasecmp(SoN,"S")==0)
    {
        struct MBR mbr;
        FILE *f = fopen (path, "rb+");
        fseek(f,0,SEEK_SET);
        fread (&mbr, sizeof(mbr), 1, f);

        //Comparo con la particion 1
        if(strcasecmp(mbr.mbr_partition_1.part_name, name) == 0)
        {
            if(strcasecmp(delet,"fast") == 0)
            {
                mbr.mbr_partition_1.part_status = 'n';
                mbr.mbr_partition_1.part_start = 0;
                mbr.mbr_partition_1.part_size = 0;
                mbr.mbr_partition_1.part_type = 'n';
            }
            else if(strcasecmp(delet,"full") == 0)
            {
                fseek(f, mbr.mbr_partition_1.part_start, SEEK_SET);
                for(int iFor  = 0; iFor  < mbr.mbr_partition_1.part_size;  iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_1.part_status = 'n';
                mbr.mbr_partition_1.part_start = 0;
                mbr.mbr_partition_1.part_size = 0;
                mbr.mbr_partition_1.part_type = 'n';
            }
            printf("Particion %s eliminada.\n" RESET, name);
            //Comparo con la particion 2
        }
        else if(strcasecmp(mbr.mbr_partition_2.part_name,  name) == 0)
        {
            if(strcasecmp(delet,"fast") == 0)
            {
                mbr.mbr_partition_2.part_status = 'n';
                mbr.mbr_partition_2.part_start = 0;
                mbr.mbr_partition_2.part_size = 0;
                mbr.mbr_partition_2.part_type = 'n';
            }
            else if(strcasecmp(delet,"full") == 0)
            {
                fseek(f, mbr.mbr_partition_2.part_start, SEEK_SET);
                for(int iFor = 0; iFor < mbr.mbr_partition_2.part_size; iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_2.part_status = 'n';
                mbr.mbr_partition_2.part_start = 0;
                mbr.mbr_partition_2.part_size = 0;
                mbr.mbr_partition_2.part_type = 'n';
            }
            printf("Particion %s eliminada.\n" RESET, name);
            //Comparo con la particion 3
        }
        else if(strcasecmp(mbr.mbr_partition_3.part_name, name) == 0)
        {
            if(strcasecmp(delet,"fast") == 0)
            {
                mbr.mbr_partition_3.part_status = 'n';
                mbr.mbr_partition_3.part_start = 0;
                mbr.mbr_partition_3.part_size = 0;
                mbr.mbr_partition_3.part_type = '0';
            }
            else if(strcasecmp(delet,"full") == 0)
            {
                fseek(f, mbr.mbr_partition_3.part_start, SEEK_SET);
                for(int iFor = 0; iFor < mbr.mbr_partition_3.part_size; iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_3.part_status = 'n';
                mbr.mbr_partition_3.part_start = 0;
                mbr.mbr_partition_3.part_size = 0;
                mbr.mbr_partition_3.part_type = 'n';
            }
            printf("Particion %s eliminada.\n" RESET, name);
            //Comparo con la particion 4
        }
        else if(strcasecmp(mbr.mbr_partition_4.part_name, name) == 0)
        {
            if(strcasecmp(delet,"fast") == 0)
            {
                mbr.mbr_partition_4.part_status = 'n';
                mbr.mbr_partition_4.part_start = 0;
                mbr.mbr_partition_4.part_size = 0;
                mbr.mbr_partition_4.part_type = 'n';
            }
            else if(strcasecmp(delet,"full") == 0)
            {
                fseek(f, mbr.mbr_partition_4.part_start, SEEK_SET);
                for(int iFor = 0; iFor < mbr.mbr_partition_4.part_size; iFor++)
                    fwrite (buffer, sizeof(buffer), 1, f);
                mbr.mbr_partition_4.part_status = 'n';
                mbr.mbr_partition_4.part_start = 0;
                mbr.mbr_partition_4.part_size = 0;
                mbr.mbr_partition_4.part_type = 'n';
            }
            printf("Particion %s eliminada.\n" RESET, name);
        }
        else
        {
            struct EBR ebr;
            struct EBR ebr_prev;
            struct EBR ebr_next;
            ebr_prev.part_next = -1;

            if(mbr.mbr_partition_1.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_1.part_start;
            }
            else if(mbr.mbr_partition_2.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_2.part_start;
            }
            else if(mbr.mbr_partition_3.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_3.part_start;
            }
            else if(mbr.mbr_partition_4.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = mbr.mbr_partition_4.part_start;
            }
            else
            {
                printf("\t>" ROJO "No se encontro la particion logica.\n" RESET);
            }
            TAG = 0;

            while(ebr.part_status != 'n')
            {
                TAG = 1;
                if(strcasecmp(ebr.part_name, name) == 0)
                {
                    if(ebr_prev.part_next != -1)
                    {

                        if(strcasecmp(delet,"fast") != 0)
                        {
                            ebr.part_start = 0;
                            ebr.part_status = 'n';
                            fseek(f, ebr.part_next, SEEK_SET);
                            fread (&ebr_next, sizeof(ebr_next), 1, f);
                            if(ebr_next.part_next == -1)
                            {
                                ebr.part_next = -1;
                                fseek(f, ebr_prev.part_next, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                                printf("\t>>" VERDE "Particion logica %s eliminada.\n" RESET, name);
                            }
                            else
                            {
                                ebr_prev.part_next = ebr.part_next;
                                ebr_next.part_previous = ebr.part_previous;
                                fseek(f, ebr.part_previous, SEEK_SET);
                                fwrite(&ebr_prev, sizeof(ebr_prev), 1, f);
                                fseek(f, ebr.part_next,SEEK_SET);
                                fwrite(&ebr_next, sizeof(ebr_next), 1, f);
                                printf("\t>>" VERDE "Particion %s eliminada.\n" RESET, name);
                            }
                            //Fin del borrado rapido
                        }
                        else if(strcasecmp(delet,"full") != 0)
                        {
                            ebr.part_start = 0;
                            ebr.part_status = 'n';
                            fseek(f, ebr.part_next, SEEK_SET);
                            fread (&ebr_next, sizeof(ebr_next), 1, f);
                            if(ebr_next.part_next == -1)
                            {
                                ebr.part_next = -1;
                                fseek(f, ebr_prev.part_next, SEEK_SET);
                                fwrite(&ebr, sizeof(ebr), 1, f);
                                fseek(f, ebr.part_start, SEEK_SET);
                                for(int iFor = 0; iFor < ebr.part_size; iFor++)
                                {
                                    fwrite (buffer, sizeof(buffer), 1, f);
                                }
                                printf("\t>>" VERDE "Particion %s eliminada.\n" RESET, name);
                            }
                            else
                            {
                                int posicionBorrado = ebr_prev.part_next;
                                ebr_prev.part_next = ebr.part_next;
                                ebr_next.part_previous = ebr.part_previous;
                                fseek(f, ebr.part_previous, SEEK_SET);
                                fwrite(&ebr_prev, sizeof(ebr_prev),1, f);
                                fseek(f, ebr.part_next, SEEK_SET);
                                fwrite(&ebr_next, sizeof(ebr_next),1, f);
                                fseek(f, posicionBorrado, SEEK_SET);
                                for(int iFor = 0; iFor < ebr.part_size+sizeof(ebr); iFor++)
                                {
                                    fwrite (buffer, sizeof(buffer), 1, f);
                                }
                                printf("\t>>" VERDE "Particion %s eliminada.\n" RESET, name);
                            }
                        }//Fin del borrado completo
                    } //Fin del if que compara el nombre con la particion actual
                }
                else
                {
                    ebr_prev = ebr;
                    fseek(f, ebr.part_next, SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1, f);
                }//Fin del cambio de particion
            }//Fin del ciclo de busqueda
        }//Fin del borrado de una particion
        fseek(f, 0, SEEK_SET);
        fwrite(&mbr, sizeof(mbr), 1, f);
        fclose(f);
    }//Fin del borrado
}

//###################################################################################################################################
//#																		      AGREGAR ESPACIO A LAS PARTICIONES 																										  #
//###################################################################################################################################
void Add_Espacio(int size, char ruta_Disco[100], char name[20], char unit[1])
{
    struct MBR mbr;
    TAG = 0;
    FILE *f;

    if((strcasecmp(unit, "k") == 0) || (strcasecmp(unit, "k\\n" RESET) == 0))
    {
        Multiplicador = 1024;
    }
    else if((strcasecmp(unit, "m") == 0) || (strcasecmp(unit, "m\\n" RESET) == 0))
    {
        Multiplicador = 1024 * 1024;
    }
    else if((strcasecmp(unit, "") == 0) || (strcasecmp(unit, "\\n" RESET) == 0))
    {
        Multiplicador = 1024 * 1024;
    }
    else if((strcasecmp(unit, "b") == 0) || (strcasecmp(unit, "b\\n" RESET) == 0))
    {
        Multiplicador = 1;
    }
    else
    {
        Multiplicador = 1;
    }

    if((fopen (ruta_Disco, "rb+")) != NULL)
    {
        f = fopen (ruta_Disco, "rb+");
        fread (&mbr, sizeof(mbr), 1, f);

        if(mbr.mbr_partition_1.part_status != 'n')
        {
            if(strcasecmp(mbr.mbr_partition_1.part_name, name) == 0)
            {
                if((mbr.mbr_partition_1.part_size + (size * Multiplicador)) >= 2048)
                {
                    if(size < 0)
                    {
                        mbr.mbr_partition_1.part_size = mbr.mbr_partition_1.part_size + (size * Multiplicador);
                        printf("\t>>" VERDE "Particion %s reducida a %d.\n" RESET, mbr.mbr_partition_1.part_name, mbr.mbr_partition_1.part_size);
                        TAG = 10;
                    }
                    else
                    {
                        if(mbr.mbr_partition_2.part_status != 'n')
                        {
                            if((mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size + (size*Multiplicador))< mbr.mbr_partition_2.part_start)
                            {
                                mbr.mbr_partition_1.part_size = mbr.mbr_partition_1.part_size + (size * Multiplicador);
                                printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_1.part_name, mbr.mbr_partition_1.part_size);
                                TAG = 10;
                            }
                        }
                        else if(mbr.mbr_partition_3.part_status != 'n')
                        {
                            if((mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size + (size*Multiplicador))< mbr.mbr_partition_3.part_start)
                            {
                                mbr.mbr_partition_1.part_size = mbr.mbr_partition_1.part_size + (size * Multiplicador);
                                printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_1.part_name, mbr.mbr_partition_1.part_size);
                                TAG = 10;
                            }
                        }
                        else if(mbr.mbr_partition_4.part_status != 'n')
                        {
                            if((mbr.mbr_partition_1.part_start + mbr.mbr_partition_1.part_size + (size*Multiplicador))< mbr.mbr_partition_4.part_start)
                            {
                                mbr.mbr_partition_1.part_size = mbr.mbr_partition_1.part_size + (size * Multiplicador);
                                printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_1.part_name, mbr.mbr_partition_1.part_size);
                                TAG = 10;
                            }
                        }
                        else
                        {
                            mbr.mbr_partition_1.part_size = mbr.mbr_partition_1.part_size + (size * Multiplicador);
                            printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_1.part_name, mbr.mbr_partition_1.part_size);
                            TAG = 10;
                        }
                    }
                }
                else
                {
                    printf("\t>" ROJO "El tamano minimo de una particion es de 2Mb.\n" RESET);
                    TAG = 10;
                }
            }
        }
        else if(mbr.mbr_partition_2.part_status != 'n' && TAG != 10)
        {
            if(strcasecmp(mbr.mbr_partition_2.part_name, name) == 0)
            {
                if((mbr.mbr_partition_2.part_size + (size * Multiplicador)) >= 2048)
                {
                    if(size < 0)
                    {
                        mbr.mbr_partition_2.part_size = mbr.mbr_partition_2.part_size + (size * Multiplicador);
                        printf("\t>>" VERDE "Particion %s disminuida a %d.\n" RESET, mbr.mbr_partition_2.part_name, mbr.mbr_partition_2.part_size);
                        TAG = 10;
                    }
                    else
                    {
                        if(mbr.mbr_partition_3.part_status != 'n')
                        {
                            if((mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size + (size*Multiplicador))< mbr.mbr_partition_3.part_start)
                            {
                                mbr.mbr_partition_2.part_size = mbr.mbr_partition_2.part_size + (size * Multiplicador);
                                printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_2.part_name, mbr.mbr_partition_2.part_size);
                                TAG = 10;
                            }
                        }
                        else if(mbr.mbr_partition_4.part_status != 'n')
                        {
                            if((mbr.mbr_partition_2.part_start + mbr.mbr_partition_2.part_size + (size*Multiplicador))< mbr.mbr_partition_4.part_start)
                            {
                                mbr.mbr_partition_2.part_size = mbr.mbr_partition_2.part_size + (size * Multiplicador);
                                printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_2.part_name, mbr.mbr_partition_2.part_size);
                                TAG = 10;
                            }
                        }
                        else
                        {
                            mbr.mbr_partition_2.part_size = mbr.mbr_partition_2.part_size + (size * Multiplicador);
                            printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_2.part_name, mbr.mbr_partition_2.part_size);
                            TAG = 10;
                        }
                    }
                }
                else
                {
                    printf("\t>" ROJO "El tamano minimo de una particion es de 2Mb.\n" RESET);
                    TAG = 10;
                }
            }
        }
        else if(mbr.mbr_partition_3.part_status != 'n' && TAG != 10)
        {
            if(strcasecmp(mbr.mbr_partition_3.part_name, name) == 0)
            {
                if((mbr.mbr_partition_3.part_size + (size * Multiplicador)) >= 2048)
                {
                    if(size < 0)
                    {
                        mbr.mbr_partition_3.part_size = mbr.mbr_partition_3.part_size + (size * Multiplicador);
                        printf("\t>>" VERDE "Particion %s disminuida a %d.\n" RESET, mbr.mbr_partition_3.part_name, mbr.mbr_partition_3.part_size);
                        TAG = 10;
                    }
                    else
                    {
                        if(mbr.mbr_partition_4.part_status != 'n')
                        {
                            if((mbr.mbr_partition_3.part_start + mbr.mbr_partition_3.part_size + (size*Multiplicador))< mbr.mbr_partition_4.part_start)
                            {
                                mbr.mbr_partition_3.part_size += mbr.mbr_partition_3.part_size + (size * Multiplicador);
                                printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_3.part_name, mbr.mbr_partition_3.part_size);
                                TAG = 10;
                            }
                        }
                        else
                        {
                            mbr.mbr_partition_3.part_size = mbr.mbr_partition_3.part_size + (size * Multiplicador);
                            printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_3.part_name, mbr.mbr_partition_3.part_size);
                            TAG = 10;
                        }
                    }
                }
                else
                {
                    printf("\t>" ROJO "El tamano minimo de una particion es de 2Mb.\n" RESET);
                    TAG = 10;
                }
            }
        }
        else if(mbr.mbr_partition_4.part_status != 'n' && TAG != 10)
        {
            if(strcasecmp(mbr.mbr_partition_4.part_name, name) == 0)
            {
                if((mbr.mbr_partition_3.part_size + (size * Multiplicador)) >= 2048)
                {
                    if(size < 0)
                    {
                        mbr.mbr_partition_4.part_size = mbr.mbr_partition_4.part_size + (size * Multiplicador);
                        printf("\t>>" VERDE "Particion %s disminuida a %d.\n" RESET, mbr.mbr_partition_4.part_name, mbr.mbr_partition_4.part_size);
                        TAG = 10;
                    }
                    else
                    {
                        if(mbr.mbr_tamano > (mbr.mbr_partition_4.part_start + mbr.mbr_partition_4.part_size + (size * Multiplicador)))
                        {
                            mbr.mbr_partition_4.part_size = mbr.mbr_partition_4.part_size + (size * Multiplicador);
                            printf("\t>>" VERDE "Particion %s incrementada a %d.\n" RESET, mbr.mbr_partition_4.part_name, mbr.mbr_partition_4.part_size);
                            TAG = 10;
                        }
                        else
                        {
                            printf("\tNo hay espacio suficiente para incrementear la particion.\n" RESET);
                            TAG = 10;
                        }
                    }
                }
                else
                {
                    printf("\t>" ROJO "El tamano minimo de una particion es de 2Mb.\n" RESET);
                    TAG = 10;
                }
            }
        }
        if(TAG == 0)
        {
            printf("\t>La particion %s no existe.\n" RESET, name);
        }
        fclose(f);
        //printf(">>>>>>>>>>>>>>>>>>>>>>Size %d\n" RESET, mbr.mbr_partition_1.part_size);
        FILE *f_disco = fopen(ruta_Disco, "wb+");
        rewind(f_disco);
        fwrite(&mbr, sizeof(mbr), 1, f_disco);
        fclose(f_disco);
    }
    else
    {
        printf("\t>" ROJO "¡El archivo no existe!\n" RESET);
    }
}

//###################################################################################################################################
//#																		            MONTAR LAS PARTICIONES       																										  #
//###################################################################################################################################
char* montador(char name[20], char path[100])
{
    char *id = malloc(sizeof(char) * 128);
    int nRuta = 0,  nPart = 1;
    while(nRuta<100)
    {
        if((strcasecmp(Montador[nRuta][0],"")==0) || (strcasecmp(Montador[nRuta][0], path)==0))
        {
            strcpy(Montador[nRuta][0], path);
            while ((strcasecmp(Montador[nRuta][nPart], "") != 0)&&(strcasecmp(Montador[nRuta][nPart], name) != 0))
            {
                nPart++;
            }
            strcpy(Montador[nRuta][nPart], name);
            sprintf(id, "vd%c%d", Abecedario[nRuta], nPart);
            return id;
        }
        else
        {
            nRuta++;
        }
    }
    return "";
}

void Desplegar()
{
    //strcpy(montadas, "");
    char idname[4];
    idname[0] = 'v';
    idname[1] = 'd';
    int j = 0;
    while(j!=-1)
    {
        if((strcasecmp(Montador[j],"")!=0))
        {
            int i = 1;
            while(i!=-1)
            {
                if((strcasecmp(Montador[0][i],"")!=0))
                {
                    idname[2] = Abecedario[j];
                    idname[3] = '0' + i;
                    idname[4] = '\0';
                    Desplegar2(idname, Montador[0][i]);
                    i++;
                }
                else
                {
                    i = -1;
                }
            }
            j++;
        }
        else
        {
            j=-1;
        }
    }
}

void Desplegar2(char id[4], char particion_montada[50])
{
    char Letra_Disco = id[2], n_S[3];
    char nameP[20], ruta_Disco[250];
    sprintf(n_S, "%c%c", id[3], id[4]);
    aux_int = 0;
    while(Letra_Disco != Abecedario[aux_int])
    {
        aux_int++;
    }
    convertido = (int) strtol(n_S, (char **)NULL, 10);
    strcpy(ruta_Disco, Montador[aux_int][0]);
    printf("id->%s path->\"%s\" name->%s\n" RESET,id, ruta_Disco, particion_montada);
}

void Montar(char name[20], char path[100])
{
    char id[10];
    struct MBR mbr;
    FILE *f_disco;
    if((fopen(path, "rb+")) == NULL)
    {
        printf("\t>" ROJO "No se encontro el archivo en la ruta especificada.\n" RESET);
    }
    else
    {
        f_disco = fopen(path, "rb+");
        fseek(f_disco,0,SEEK_SET);
        fread (&mbr, sizeof(mbr), 1,f_disco);

        if(strcasecmp(mbr.mbr_partition_1.part_name, name) == 0)
        {
            mbr.mbr_partition_1.part_status = 'a';
            strcpy(id, montador(name, path));
            printf("\t>>" VERDE "La particion %s se monto con el id: %s\n" RESET, name, id);
        }
        else if(strcasecmp(mbr.mbr_partition_2.part_name, name) == 0)
        {
            mbr.mbr_partition_2.part_status = 'a';
            strcpy(id, montador(name, path));
            printf("\t>>" VERDE "La particion %s se monto con el id: %s\n" RESET, name, id);
        }
        else if(strcasecmp(mbr.mbr_partition_3.part_name, name) == 0)
        {
            mbr.mbr_partition_3.part_status = 'a';
            strcpy(id, montador(name, path));
            printf("\t>>" VERDE "La particion %s se monto con el id: %s\n" RESET, name, id);
        }
        else if(strcasecmp(mbr.mbr_partition_4.part_name, name) == 0)
        {
            mbr.mbr_partition_4.part_status = 'a';
            strcpy(id, montador(name, path));
            printf("\t>>" VERDE "La particion %s se monto con el id: %s\n" RESET, name, id);
        }
        else
        {
            struct EBR ebr;
            TAG = 0;

            if(mbr.mbr_partition_1.part_type == 'e')
            {
                fseek(f_disco, mbr.mbr_partition_1.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }
            else if(mbr.mbr_partition_2.part_type == 'e')
            {
                fseek(f_disco, mbr.mbr_partition_2.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }
            else if(mbr.mbr_partition_3.part_type == 'e')
            {
                fseek(f_disco, mbr.mbr_partition_3.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }
            else if(mbr.mbr_partition_4.part_type == 'e')
            {
                fseek(f_disco, mbr.mbr_partition_4.part_start, SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f_disco);
            }
            else
            {
                printf("\t>" ROJO "Solo se pueden montar particiones existentes, intentelo de nuevo\n" RESET);
                TAG =1;
            }
            while((ebr.part_status != 'n')&&(TAG == 0))
            {
                if(strcasecmp(ebr.part_name, name) == 0)
                {
                    ebr.part_status = 'a';
                    fseek(f_disco, ebr.part_start-sizeof(ebr), SEEK_SET);
                    fwrite(&ebr, sizeof(ebr), 1, f_disco);
                    strcpy(id, montador( name, path));
                    printf("\t>>" VERDE "La particion %s se monto con el id: %s\n" RESET, name, id);
                    TAG = 1;
                }
                else
                {
                    fseek(f_disco,ebr.part_next,SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1,f_disco);
                }
            }
        }
        fseek(f_disco, 0, SEEK_SET);
        fwrite(&mbr, sizeof(mbr), 1, f_disco);
        fclose(f_disco);
    }
}

//###################################################################################################################################
//#																		           DESMONTAR LAS PARTICIONES     																										  #
//###################################################################################################################################
void Desmontar(char id[4])
{
    struct MBR mbr;
    char name[20], path[100];
    int Algo = 0;
    char Letra_Disco = id[2];
    char nP[1];
    FILE *f;
    sprintf(nP, "%c%c", id[3], id[4]);
    aux_int = 0;
    while(Letra_Disco != Abecedario[aux_int])
    {
        aux_int++;
    }

    strcpy(name, Montador[aux_int][(int) strtol(nP, (char **)NULL, 10)]);
    strcpy(path, Montador[aux_int][0]);
    strcpy(Montador[aux_int][(int) strtol(nP, (char **)NULL, 10)], "");
    TAG = 0;
    while(Algo < 100)
    {
        if(strcasecmp(Montador[aux_int][Algo], "") != 0)
        {
            TAG = 1;
        }
        Algo++;
    }
    if(TAG == 0)
    {
        strcpy(Montador[aux_int][0], "");
    }

    if((fopen (path, "rb+")) == NULL)
    {
        printf("\t>" ROJO "No se encontro el archivo en la ruta indicada\n" RESET);
    }
    else
    {
        f = fopen (path, "rb+");
        fseek(f,0,SEEK_SET);
        fread (&mbr, sizeof(mbr), 1,f);
        if(strcasecmp(mbr.mbr_partition_1.part_name,name) == 0)
        {
            mbr.mbr_partition_1.part_status = 'd';
            printf("\t>>" VERDE "Particion %s desmontada.\n" RESET,name);
        }
        else if(strcasecmp(mbr.mbr_partition_2.part_name,name) == 0)
        {
            mbr.mbr_partition_2.part_status = 'd';
            printf("\t>>" VERDE "Particion %s desmontada.\n" RESET,name);
        }
        else if(strcasecmp(mbr.mbr_partition_3.part_name,name) == 0)
        {
            mbr.mbr_partition_3.part_status = 'd';
            printf("\t>>" VERDE "Particion %s desmontada.\n" RESET,name);
        }
        else if(strcasecmp(mbr.mbr_partition_4.part_name,name) == 0)
        {
            mbr.mbr_partition_4.part_status = 'd';
            printf("\t>>" VERDE "Particion %s desmontada.\n" RESET,name);
        }
        else
        {
            struct EBR ebr;
            TAG = 0;
            //con esto ubico en donde esta el primer ebr a leer
            if(mbr.mbr_partition_1.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
            }
            else if(mbr.mbr_partition_2.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
            }
            else if(mbr.mbr_partition_3.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
            }
            else if(mbr.mbr_partition_4.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
            }
            else
            {
                printf("\t>" ROJO "La particion que intenta desmontar no esta montada\n" RESET);
                TAG =1;
            }
            while((ebr.part_status != '0')&&(TAG == 0))
            {
                if(strcasecmp(ebr.part_name,name) == 0)
                {
                    ebr.part_status = 'd';
                    fseek(f,ebr.part_start-sizeof(ebr),SEEK_SET);
                    fwrite(&ebr,sizeof(ebr),1,f);
                    TAG = 1;
                    printf("\t>>" VERDE "Particion %s desmontada.\n" RESET,name);
                }
                else
                {
                    fseek(f,ebr.part_next,SEEK_SET);
                    fread (&ebr, sizeof(ebr), 1,f);
                }
            }
        }
        fseek(f, 0, SEEK_SET);
        fwrite(&mbr, sizeof(mbr), 1, f);
        fclose(f);
    }
}

//###################################################################################################################################
//#																		         METODOS DE LOS REPORTES A CREAR         																						  #
//###################################################################################################################################
void Rep(char ruta_Destino[100],  char id[4], char name[10], char path[100])
{
    char Letra_Disco = id[2], n_S[3], p[2];
    char nameP[20], ruta_Disco[250];
    sprintf(n_S, "%c%c", id[3], id[4]);
    aux_int = 0;
    while(Letra_Disco != Abecedario[aux_int])
    {
        aux_int++;
    }
    convertido = (int) strtol(n_S, (char **)NULL, 10);
    strcpy(ruta_Disco, Montador[aux_int][0]);
    strcpy(nameP, Montador[aux_int][convertido]);
    strcpy(p, Montador[aux_int][convertido]);

    if(strcasecmp(Montador[aux_int][convertido],"") != 0)
    {
        if(strcasecmp(name,"mbr") == 0)
        {
            Rep_MBR(ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"disk") == 0)
        {
            Rep_Disco(ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"block") == 0)
        {
            //Rep_Bloques(nameP, ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"bm_block") == 0)
        {
            //Rep_Bitmap('b', nameP, ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"bm_inode") == 0)
        {
            //Rep_Bitmap('i', nameP, ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"inode") == 0)
        {
            //Rep_Inodos(nameP, ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"journaling") == 0)
        {
            //Rep_Journal(nameP, ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"tree") == 0)
        {
            //Rep_Tree(nameP, ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"sb") == 0)
        {
            //Rep_SuperBloque(nameP, ruta_Disco, ruta_Destino);
        }
        else if(strcasecmp(name,"file") == 0)
        {
            //Rep_File(nameP, ruta_Disco, ruta_Destino, path);
        }
        else
        {
            printf("\t>" ROJO "Nombre de reporte incorrecto, intentelo de nuevo\n" RESET);
        }
    }
    else
    {
        printf("\t>" ROJO "No hay discos o particiones para crear el reporte solicitado\nAsegurese de tener los archivos creados necesarios para realizar el reporte que solicita\n" RESET);
    }
}

//###################################################################################################################################
//#																		          REPORTE DEL EBR (EXTENDED BOOT RECORD)      																					#
//###################################################################################################################################
void Rep_EBR(FILE *fp, FILE *f, struct EBR ebr)
{
    if((ebr.part_status != 'n')&&(ebr.part_status!='\000'))
    {
        fprintf ( fp, "%s [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n", ebr.part_name);
        fprintf ( fp, "<tr><td><b>%s</b></td><td>Valor</td></tr>\n",ebr.part_name);
        fprintf ( fp, "<tr><td><b>part_status</b></td><td><b>%c</b></td></tr>\n",ebr.part_status);
        fprintf ( fp, "<tr><td><b>part_fit</b></td><td><b>%c</b></td></tr>\n",ebr.part_fit);
        fprintf ( fp, "<tr><td><b>part_start</b></td><td><b>%d</b></td></tr>\n",ebr.part_start);
        fprintf ( fp, "<tr><td><b>part_size</b></td><td><b>%d</b></td></tr>\n",ebr.part_size);
        fprintf ( fp, "<tr><td><b>part_next</b></td><td><b>%d</b></td></tr>\n",ebr.part_next);
        fprintf ( fp, "<tr><td><b>part_name</b></td><td><b>%s</b></td></tr>\n",ebr.part_name);
        fprintf ( fp, "</table>>];\n");
        if((ebr.part_next != -1) &&(ebr.part_next!=0))
        {
            fseek(f,ebr.part_next,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            Rep_EBR(fp, f, ebr);
        }
    }
}

void Rep_EBR_Disco(FILE *fp, FILE *f, struct EBR ebr, struct MBR mbr)
{

    float tam = mbr.mbr_tamano;
    if(ebr.part_status != 'n')
    {
        fprintf ( fp, "extendida%s [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n",ebr.part_name);
        fprintf ( fp, "<tr><td><b>EBR</b></td><td><b>logica(%s) \n %f \% </b></td></tr>\n",ebr.part_name,(ebr.part_size/tam)*100);
        fprintf ( fp, "</table>>];\n");
        if(ebr.part_next != -1)
        {
            fseek(f,ebr.part_next,SEEK_SET);
            fread (&ebr, sizeof(ebr), 1,f);
            Rep_EBR_Disco(fp, f, ebr,mbr);
        }
    }
}

//###################################################################################################################################
//#																		          REPORTE DEL MBR (MASTER BOOT RECORD)      																					#
//###################################################################################################################################
void Rep_MBR(char path[100], char ruta_Destino[150])
{
    struct MBR mbr;
    struct EBR ebr;
    FILE *f;
    if((fopen (path, "rb+")) != NULL)
    {

        f = fopen (path, "rb+");
        FILE *fp = fopen ("/home/mario_morales/Documentos/MIA/dot/MBR.dot", "w" );
        rewind(f);
        fread(&mbr, sizeof(mbr), 1, f);
        fprintf ( fp, "digraph \"mbr\" {\n");
        fprintf ( fp, "node [shape=plaintext]\n");
        fprintf ( fp, "nodoMbr [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
        fprintf ( fp, "<tr><td><b> %s </b></td></tr>\n", path);
        fprintf ( fp, "<tr><td><b>mbr_tamano(bytes)</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_tamano);
        fprintf ( fp, "<tr><td><b>mbr_fecha_creacion</b></td><td><b> %s </b></td></tr>\n",ctime(&mbr.mbr_fecha_creacion));
        fprintf ( fp, "<tr><td><b>mbr_disk_signature</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_disk_signature);
        TAG = 0;

        if(mbr.mbr_partition_1.part_status != 'n')
        {
            fprintf ( fp, "<tr><td><b>part_status_1</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_1.part_status);
            fprintf ( fp, "<tr><td><b>part_type_1</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_1.part_type);
            fprintf ( fp, "<tr><td><b>part_fit_1</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_1.part_fit);
            fprintf ( fp, "<tr><td><b>part_start_1</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_1.part_start);
            fprintf ( fp, "<tr><td><b>part_size_1</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_1.part_size);
            fprintf ( fp, "<tr><td><b>part_name_1</b></td><td><b> %s </b></td></tr>\n",mbr.mbr_partition_1.part_name);
            if(mbr.mbr_partition_1.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                TAG = 10;
            }
        }
        if(mbr.mbr_partition_2.part_status != 'n')
        {
            fprintf ( fp, "<tr><td><b>part_status_2</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_2.part_status);
            fprintf ( fp, "<tr><td><b>part_type_2</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_2.part_type);
            fprintf ( fp, "<tr><td><b>part_fit_2</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_2.part_fit);
            fprintf ( fp, "<tr><td><b>part_start_2</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_2.part_start);
            fprintf ( fp, "<tr><td><b>part_size_2</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_2.part_size);
            fprintf ( fp, "<tr><td><b>part_name_2</b></td><td><b> %s </b></td></tr>\n",mbr.mbr_partition_2.part_name);
            if(mbr.mbr_partition_2.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                TAG = 10;
            }
        }
        if(mbr.mbr_partition_3.part_status != 'n')
        {
            fprintf ( fp, "<tr><td><b>part_status_3</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_3.part_status);
            fprintf ( fp, "<tr><td><b>part_type_3</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_3.part_type);
            fprintf ( fp, "<tr><td><b>part_fit_3</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_3.part_fit);
            fprintf ( fp, "<tr><td><b>part_start_3</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_3.part_start);
            fprintf ( fp, "<tr><td><b>part_size_3</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_3.part_size);
            fprintf ( fp, "<tr><td><b>part_name_3</b></td><td><b> %s </b></td></tr>\n",mbr.mbr_partition_3.part_name);
            if(mbr.mbr_partition_3.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                TAG = 10;
            }
        }
        if(mbr.mbr_partition_4.part_status != 'n')
        {
            fprintf ( fp, "<tr><td><b>part_status_4</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_4.part_status);
            fprintf ( fp, "<tr><td><b>part_type_4</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_4.part_type);
            fprintf ( fp, "<tr><td><b>part_fit_4</b></td><td><b> %c </b></td></tr>\n",mbr.mbr_partition_4.part_fit);
            fprintf ( fp, "<tr><td><b>part_start_4</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_4.part_start);
            fprintf ( fp, "<tr><td><b>part_size_4</b></td><td><b> %d </b></td></tr>\n",mbr.mbr_partition_4.part_size);
            fprintf ( fp, "<tr><td><b>part_name_4</b></td><td><b> %s </b></td></tr>\n",mbr.mbr_partition_4.part_name);
            if(mbr.mbr_partition_4.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                TAG = 10;
            }
        }
        fprintf ( fp, "</table>>];\n");

        if(TAG == 10)
        {
            EBR_Ini = ebr.part_next;
            Rep_EBR(fp,f,ebr);
        }

        fprintf ( fp, "}\n");
        fclose(f);
        fclose(fp);
        char *rutacompleta = malloc(500);
        memset(rutacompleta,0, sizeof(rutacompleta));
        strcat(rutacompleta, "dot -Tjpg /home/mario_morales/Documentos/MIA/dot/MBR.dot -o ");
        strcat(rutacompleta, ruta_Destino+1);
        strcat(rutacompleta, "\0");
        system(rutacompleta);
        sleep(2);
        printf("\t>>" VERDE "Reporte MBR creado exitosamente!\n" RESET);
    }
    else
    {
        printf("\t>" ROJO "El archivo no existe!\n" RESET);
    }
}

//###################################################################################################################################
//#																		                 REPORTE DEL DISCO                  																					#
//###################################################################################################################################
void Rep_Disco_Interno(FILE *fp, FILE *f, struct MBR mbr, int comparador)
{
    TAG = 0;
    struct EBR ebr;
    float tamanio = mbr.mbr_tamano;

    if(mbr.mbr_partition_1.part_status != 'n')
    {
        if(mbr.mbr_partition_1.part_start == comparador)
        {
            comparador += mbr.mbr_partition_1.part_size;
            TAG = 10;
            if(mbr.mbr_partition_1.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_1.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = ebr.part_next;
                if(ebr.part_status == 'n')
                {
                    fprintf ( fp, "extendida [label=\"Extendida(vacia) \n %f \%\"];",(mbr.mbr_partition_1.part_size/tamanio)*100.00);
                }
                else
                {
                    fprintf ( fp, "subgraph sub { \n");
                    Rep_EBR_Disco(fp,f,ebr,mbr);
                    fprintf ( fp, "}");
                }
            }
            else
            {
                fprintf ( fp, "primaria%s [label=\"primaria(%s) \n %f \% \"];", mbr.mbr_partition_1.part_name, mbr.mbr_partition_1.part_name,(mbr.mbr_partition_1.part_size/tamanio)*100.00);
            }
        }
    }
    if((mbr.mbr_partition_2.part_status != 'n')&&(TAG != 10))
    {
        if(mbr.mbr_partition_2.part_start == comparador)
        {
            comparador += mbr.mbr_partition_2.part_size;
            TAG = 10;
            if(mbr.mbr_partition_2.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_2.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = ebr.part_next;
                if(ebr.part_status == 'n')
                {
                    fprintf ( fp, "extendida [label=\"Extendida(vacia) \n %f \%\"];",(mbr.mbr_partition_2.part_size/tamanio)*100.00);
                }
                else
                {
                    fprintf ( fp, "subgraph sub { \n");
                    Rep_EBR_Disco(fp,f,ebr,mbr);
                    fprintf ( fp, "}");
                }

            }
            else
            {
                fprintf ( fp, "primaria%s [label=\"primaria(%s) \n %f \% \"];", mbr.mbr_partition_2.part_name, mbr.mbr_partition_2.part_name,(mbr.mbr_partition_2.part_size/tamanio)*100.00);
            }
        }
    }
    if((mbr.mbr_partition_3.part_status != 'n')&&(TAG != 10))
    {
        if(mbr.mbr_partition_3.part_start == comparador)
        {
            comparador += mbr.mbr_partition_3.part_size;
            TAG = 10;
            if(mbr.mbr_partition_3.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_3.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = ebr.part_next;
                if(ebr.part_status == 'n')
                {
                    fprintf ( fp, "extendida [label=\"Extendida(vacia) \n %f \% \"];",(mbr.mbr_partition_3.part_size/tamanio)*100.00);
                }
                else
                {
                    fprintf ( fp, "subgraph sub1 { \n");
                    Rep_EBR_Disco(fp,f,ebr,mbr);
                    fprintf ( fp, "}");
                }
            }
            else
            {
                fprintf ( fp, "primaria%s [label=\"primaria(%s) \n %f \% \"];", mbr.mbr_partition_3.part_name, mbr.mbr_partition_3.part_name,(mbr.mbr_partition_3.part_size/tamanio)*100.00);
            }
        }
    }
    if((mbr.mbr_partition_4.part_status != 'n')&&(TAG != 10))
    {
        if(mbr.mbr_partition_4.part_start == comparador)
        {
            comparador += mbr.mbr_partition_4.part_size;
            TAG = 10;
            if(mbr.mbr_partition_4.part_type == 'e')
            {
                fseek(f,mbr.mbr_partition_4.part_start,SEEK_SET);
                fread (&ebr, sizeof(ebr), 1,f);
                EBR_Ini = ebr.part_next;
                if(ebr.part_status == 'n')
                {
                    fprintf ( fp, "extendida [label=\"Extendida(vacia) \n %f \%\"];",(mbr.mbr_partition_4.part_size/tamanio)*100.00);
                }
                else
                {
                    fprintf ( fp, "subgraph sub1 { \n");
                    Rep_EBR_Disco(fp,f,ebr,mbr);
                    fprintf ( fp, "}");
                }
            }
            else
            {
                fprintf ( fp, "primaria%s [label=\"primaria(%s) \n %f \% \"];", mbr.mbr_partition_4.part_name, mbr.mbr_partition_4.part_name,(mbr.mbr_partition_4.part_size/tamanio)*100.00);
            }
        }
    }

    int respaldo = 984315;
    if(TAG == 0)
    {
        fprintf ( fp, "libre [label=\"libre\"];");
        respaldo = comparador;

        if(comparador < mbr.mbr_partition_1.part_start)
        {
            respaldo = mbr.mbr_partition_1.part_start;
           fprintf ( fp, "libre1 [label=\"libre \\n %f \% \"];",(mbr.mbr_partition_1.part_size/tamanio)*100.00);
            TAG = 11;
        }
        if((comparador < mbr.mbr_partition_2.part_start)&&((respaldo > mbr.mbr_partition_2.part_start)||(TAG == 0)))
        {
            respaldo = mbr.mbr_partition_2.part_start;

           fprintf ( fp, "libre2 [label=\"libre \\n %f \% \"];",(mbr.mbr_partition_2.part_size/tamanio)*100.00);
            TAG = 11;
        }
        if((comparador < mbr.mbr_partition_3.part_start)&&((respaldo > mbr.mbr_partition_3.part_start)||(TAG == 0)))
        {
            respaldo = mbr.mbr_partition_3.part_start;

            fprintf ( fp, "libre3 [label=\"libre \\n %f \% \"];",(mbr.mbr_partition_3.part_size/tamanio)*100.00);
            TAG = 11;
        }
        if((comparador < mbr.mbr_partition_4.part_start)&&((respaldo > mbr.mbr_partition_4.part_start)||(TAG == 0)))
        {
            respaldo = mbr.mbr_partition_4.part_start;
        fprintf ( fp, "libre4 [label=\"libre \\n %f \% \"];",(mbr.mbr_partition_4.part_size/tamanio)*100.00);
            TAG = 11;
        }

        //printf("Porcentaje es: %f ",(respaldo/tamanio)*100.00);

    }

    if(TAG == 10)
    {
        Rep_Disco_Interno(fp,f,mbr,comparador);
    }
    else if(comparador < respaldo)
    {
        Rep_Disco_Interno(fp,f,mbr,respaldo);
    }
}

void Rep_Disco(char path[100], char pSalida[150])
{
    struct MBR mbr;
    struct EBR ebr;
    FILE *f;
    if((fopen (path, "rb+")) != NULL)
    {
        f = fopen (path, "rb+");
        FILE *fp = fopen ("/home/mario_morales/Documentos/MIA/dot/Disco.dot", "w" );
        fread (&mbr, sizeof(mbr), 1,f);
        fprintf ( fp, "digraph disk { \n subgraph sub { \n node [shape=rectangle style=filled color=black fillcolor=white];\n");
        fprintf ( fp, "mbr [label=\"MBR\"];");
        TAG = 0;

        Rep_Disco_Interno(fp,f,mbr,sizeof(mbr));

        fprintf (fp, "}\n }\n");

        fclose(f);
        fclose(fp);

        char *rutacompleta = malloc(300);
        memset(rutacompleta,0, sizeof(rutacompleta));
        strcat(rutacompleta, "dot -Tjpg /home/mario_morales/Documentos/MIA/dot/Disco.dot -o ");
        strcat(rutacompleta, pSalida + 1);
        strcat(rutacompleta, "\0");
        system(rutacompleta);
        sleep(2);
        printf("\t>>" VERDE "Reporte de disco creado exitosamente!\n" RESET);
    }
    else
    {
        printf("\t>" ROJO "El archivo no existe!\n" RESET);
    }
}

//###################################################################################################################################
//#																					          ANALIZAR COMANDO																															#
//###################################################################################################################################
void Analizar_Comando(char *linea, char *palabra)
{
    char lid[20][5];
    char lfile[10][100];
    char aux_String[100], ruta_Disco[100], ruta_Destino[100], path[100], ruta_A[100];
    char name[20];
    char size[10], usr[10], pwd[10], grp[10];
    char id[5];
    char delet[4];
    char fs[3], ugo[3], perm[3];
    char unit[2], type[2], fit[2], order[2];
    int  add;

    strcpy(delet, "");
    strcpy(unit, "");
    strcpy(type, "");
    strcpy(fit, "");
    strcpy(order, "");
    strcpy(ruta_Destino, "");
    strcpy(ruta_Disco, "");
    add = 0;
    c_true = 0;

    //CREACION DE DISCOS
    if(strcasecmp(palabra, "mkdisk") == 0)
    {
        strcpy(ruta_Disco, "");
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "->");
        while(temp != NULL)
        {
            if(strcasecmp(temp, "size")==0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(size, temp);
            }
            else if (strcasecmp(temp,"path")==0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp+1);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);

                if (aux[0]=='\"')
                {
                    while(strcmp(&aux2[strlen(aux2) -1], "\"") != 0)
                    {
                        temp = strtok(NULL, " ");
                        strcat(aux, " ");
                        strcat(aux, temp);
                        strcpy(aux2, aux);
                    }
                    strcat(aux2, "\0");
                    char* otroaux = malloc(100);
                    strncpy(otroaux, aux2 + 1, strlen(aux2) - 2);
                    otroaux[strlen(aux2)-2] = '\0';
                    strcpy(ruta_Disco, otroaux);
                }
                else
                {
                    strcpy(ruta_Disco, aux2);
                }

            }
            else if(strcasecmp(temp,"unit") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(unit, temp);
            }
            else if(strcasecmp(temp, "\n") == 0)
            {

            }
            else if(strcasecmp(temp, "\r\n") == 0)
            {

            }
            else
            {
                printf("\t>" ROJO "\"%s\" no es un modificador valido para la instruccion Mkdisk.\n" RESET,temp);
                c_true = 1;


            }//Fin del if que verifica que modificador es
            temp = strtok(NULL, "->");
            if(temp!=NULL)
            {
                if(temp[0] == ' ')
                {
                    temp++;
                }
            }
        }//Fin del while que obtiene las propiedades

        int convertido = (int)strtol(size, (char **)NULL, 10);
        if(convertido >= 0)
        {
            if((strcasecmp(size,"")!=0)&&(strcasecmp(ruta_Disco,"")!=0)&& (c_true == 0)&&((strcasecmp(unit,"")!=0)||(strcasecmp(unit,"")==0)))
            {
                //Crea los directorios si no existen
                Crear_Directorios_Reales(ruta_Disco);
                Eliminar_Ultima_Carpeta(ruta_Disco);

                if((strcasecmp(unit,"m")==0) ||(strcasecmp(unit,"")==0))
                {
                    Multiplicador = 1024;
                }
                else
                {
                    Multiplicador = 1;
                }


                //CREACION DEL DISCO
                printf("\t>>" VERDE "Creando disco... \n" RESET);

                int tamanio = (int) strtol(size, (char **)NULL, 10);
                if((tamanio%8 == 0)||(tamanio%8 != 0))
                {
                    Crear_Disco(size, name, ruta_Disco);
                }
                else
                {
                    printf("\t>" ROJO "El tamaño no es un multiplo de 8.\n\t>Por favor intentelo nuevamente e ingrese un tamaño que sea múltiplo de 8\n" RESET);
                }
            }
            else
            {
                printf("\t>" ROJO "Faltan modificadores obligatorios.\n\t>Por favor intentelo nuevamente e ingrese el tamaño y la ruta por lo menos.\n" RESET);
            }
        }
        else
        {
            printf("\t>" ROJO "No se pueden crear discos con size negativo\n", RESET);
        }
    }
    else if (strcasecmp(palabra, "rmdisk") == 0)
    {
        strcpy(ruta_Disco, "");
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "->");
        if (strcasecmp(temp,"path")==0)
        {
            temp = strtok(NULL, " ");
            char* aux = malloc(strlen("plop") + 100);
            strcpy(aux, temp+1);
            char aux2[150];
            memset(aux2, 0, sizeof(aux2));
            strcpy(aux2, aux);

            if (aux[0]=='\"')
            {
                while(strcmp(&aux2[strlen(aux2) -1], "\"") != 0)
                {
                    temp = strtok(NULL, " ");
                    strcat(aux, " ");
                    strcat(aux, temp);
                    strcpy(aux2, aux);
                }

                strcat(aux2, "\0");
                char* otroaux = malloc(strlen("plop") + 100);
                strncpy(otroaux, aux2 + 1, strlen(aux2) - 2);
                strcpy(ruta_Disco, otroaux);
                printf("Ruta_Disco: %s\n", ruta_Disco);
                Eliminar_Disco(ruta_Disco);
            }
            else
            {
                strcpy(ruta_Disco, aux2);
                printf("Ruta_Disco: %s\n", ruta_Disco);
                Eliminar_Disco(ruta_Disco);
            }


        }
        else if(strcasecmp(temp, "\n") == 0)
        {

        }
        else if(strcasecmp(temp, "\r\n") == 0)
        {

        }
        else
        {
            printf("\t>" ROJO "\"%s\" no es un modificador valido para la instruccion rmdisk.\n" RESET,temp);
        }
    }
    else if(strcasecmp(palabra, "fdisk") == 0)
    {
        strcpy(ruta_Disco, "");
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "->");

        while(temp != NULL)
        {
            if(strcasecmp(temp,"size") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(size, temp);
            }
            else if(strcasecmp(temp,"unit") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(unit, temp);
            }
            else if(strcasecmp(temp,"path") == 0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp+1);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);
                if (aux[0]=='\"')
                {
                    while(strcmp(&aux2[strlen(aux2) -1], "\"") != 0)
                    {
                        temp = strtok(NULL, " ");
                        strcat(aux, " ");
                        strcat(aux, temp);
                        strcpy(aux2, aux);
                    }

                    char* otroaux = malloc(150);
                    memset(&otroaux[0], 0, sizeof(otroaux));
                    strncpy(otroaux, aux2 + 1, strlen(aux2) - 2);
                    otroaux[strlen(aux2)-2] = '\0';
                    strcpy(ruta_Disco, otroaux);
                }
                else
                {
                    strcpy(ruta_Disco, aux2);
                }

            }
            else if(strcasecmp(temp,"type") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(type, temp);
            }
            else if(strcasecmp(temp,"fit") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(fit, temp);
            }
            else if(strcasecmp(temp,"delete") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(delet, temp);
            }
            else if(strcasecmp(temp,"name") == 0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp+1);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);

                strcat(aux2, "\0");
                char* otroaux = malloc(strlen("plop") + 100);
                strncpy(otroaux, aux2, strlen(aux2));
                otroaux[strlen(aux2)] = '\0';
                strcpy(name, otroaux);
            }
            else if(strcasecmp(temp,"add") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                add = (int) strtol(temp, (char **)NULL, 10);
            }
            else if(strcasecmp(temp, "\n") == 0)
            {

            }
            else if(strcasecmp(temp, "\r\n") == 0)
            {

            }
            else
            {
                printf("\t>" ROJO "\"%s\" no es un modificador valido para la instruccion fdisk.\n" RESET,temp);
            }
            temp = strtok(NULL, "->");
            if(temp!=NULL)
            {
                if(temp[0] == ' ')
                {
                    temp++;
                }
            }
        }

        if(((strcasecmp(size,"")!=0)&&(strcasecmp(delet,"")==0))&&(strcasecmp(ruta_Disco,"")!=0)&&(strcasecmp(name,"")!=0)&&(add == 0))
        {
            //Validacion de que el fit este correcto
            if((strcasecmp(fit,"bf") == 0)||(strcasecmp(fit,"ff") == 0)||(strcasecmp(fit,"wf") == 0)||(strcasecmp(fit,"") == 0))
            {
                //Validacion de que el type este correcto
                if((strcasecmp(type,"p") == 0)||(strcasecmp(type,"e") == 0)||(strcasecmp(type,"l") == 0)||(strcasecmp(type,"") == 0))
                {
                    if(((size >= 2) && (strcasecmp(unit, "m") == 0)) || ((size >= (2*1024)) && ((strcasecmp(unit, "k") == 0) || (strcasecmp(unit, "") == 0))) || ((size >= 2*1024*1024) && (strcasecmp(unit, "b") == 0)))
                    {
                        Crear_Particion(name, size, unit, fit, type, ruta_Disco);
                    }
                    else
                    {
                        printf("\t>" ROJO "El tamano minimo para una particion es de 2 Mb.\n" RESET);
                    }
                }
                else
                {
                    printf("\t>" ROJO "Caracter no reconocido.\n\t>Los types validos son: P, E y L.\n" RESET);
                }
            }
            else
            {
                printf("\t>" ROJO "Caracter no reconocido.\n\t>Los tipos de fit disponibles son: BF, FF y WF.\n" RESET);
            }
        }
        else if((strcasecmp(delet,"")!=0)&&(strcasecmp(ruta_Disco,"")!=0)&&(strcasecmp(name,"")!=0) && (add == 0))
        {
            Eliminar_Particiones(delet, name, ruta_Disco);

        }
        else if(((strcasecmp(delet,"")==0))&&(strcasecmp(ruta_Disco,"")!=0)&&(add != 0)&&(strcasecmp(name,"")!=0))
        {
            Add_Espacio(add, ruta_Disco, name, unit);
        }
        else
        {
            printf("\t>" ROJO "No se han escrito todos los comandos necesarios, intentelo de nuevo...\n" RESET);
            printf("\t>" AMARILLO "Recuerde que no puede usar delete y add en un mismo comando...\n" RESET);
        }
    }
    else if (strcasecmp(palabra, "mount") == 0)
    {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "->");
        while (temp != NULL)
        {
            if (strcasecmp(temp,"name")==0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);
                strcat(aux2, "\0");
                char sincom[150];
                memset(&sincom[0], 0, sizeof(sincom));
                char* otroaux = malloc(100);
                strncpy(sincom, aux2+1, strlen(aux2)-1);
                strncpy(otroaux, sincom, strlen(sincom)+1);
                strcpy(name, otroaux);
            }
            else if (strcasecmp(temp,"path")==0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp+1);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);
                strcat(aux2, "\0");
                char sincom[150];
                memset(&sincom[0], 0, sizeof(sincom));
                char* otroaux = malloc(100);
                if(strcmp(&aux2[strlen(aux2) -1], "\""))
                {
                    strncpy(sincom, aux2+1, strlen(aux2)-3);
                    strncpy(otroaux, sincom, strlen(sincom)+6);
                }
                else
                {
                    strncpy(sincom, aux2+1, strlen(aux2)-2);
                    strncpy(otroaux, sincom, strlen(sincom)+6);
                }
                strcpy(ruta_Disco, otroaux);
            }
            else if(strcasecmp(temp, "\n") == 0)
            {

            }
            else if(strcasecmp(temp, "\r\n") == 0)
            {

            }
            else
            {
                printf("\t>" ROJO "Comando no valido, para montar una particion debe ingresar el nombre y la ruta.\n" RESET);
            }
            temp = strtok(NULL, "->");
            if(temp!=NULL)
            {
                if(temp[0] == ' ')
                {
                    temp++;
                }
            }
        }

        if((strcasecmp(name,"") != 0) && (strcasecmp(ruta_Disco,"") != 0))
        {
            Montar(name, ruta_Disco);
        }
        else
        {
            printf("\t>" ROJO "Para montar ingrese el nombre de la particion y la ruta del disco.\n" RESET);
            Desplegar();
        }
    }
    else if(strcasecmp(palabra, "unmount") == 0)
    {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "-> ");
        int contador = 1;
        while(temp != NULL)
        {
            char *auxiliar= malloc(strlen("plop") + 10);
            strcpy(auxiliar, "id");
            //char num[2];
            //sprintf(num, "%d", contador);
            //strcat(auxiliar, num);
            if (strcasecmp(temp, auxiliar)==0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(lid[contador], temp);
                contador++;
            }
            else if(strcasecmp(temp, "\n") == 0)
            {

            }
            else if(strcasecmp(temp, "\r\n") == 0)
            {

            }
            else
            {
                printf("\t>" ROJO "Comando invalido o numero de id no consecutivo, para desmontar debe ingresar el id asignado a la particion.\n" RESET);
            }
            temp = strtok(NULL, "-> ");
        }
        if(strcasecmp(lid[1], "") != 0)
        {
            for(int i = 1; i < contador; i++)
            {
                Desmontar(lid[i]);
                //printf("%s\n", lid[i]);
            }
            //printf("Id: %s",id);
        }
        else
        {
            printf("\t>" ROJO "Para desmontar se requiere el id de la particion.\n" RESET);
        }
    }
    else if(strcasecmp(palabra, "rep") == 0)
    {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "->");

        while(temp != NULL)
        {
            if(strcasecmp(temp,"name") == 0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp+1);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);

                char* otroaux = malloc(150);
                memset(&otroaux[0], 0, sizeof(otroaux));
                strncpy(otroaux, aux2, strlen(aux2));
                otroaux[strlen(aux2)] = '\0';
                strcpy(name, otroaux);
            }
            else if(strcasecmp(temp,"path") == 0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);

                while(strcmp(&aux2[strlen(aux2) -1], "\"") != 0)
                {
                    temp = strtok(NULL, " ");
                    strcat(aux, " ");
                    strcat(aux, temp);
                    strcpy(aux2, aux);
                }
                strcat(aux2, "\0");
                char* otroaux = malloc(strlen("plop") + 100);
                strncpy(otroaux, aux2 + 1, strlen(aux2) - 2);
                otroaux[strlen(aux2)-2] = '\0';
                strcpy(ruta_Destino, otroaux);
            }
            else if(strcasecmp(temp,"id") == 0)
            {
                temp = strtok(NULL, " ");
                if (temp[0] == '-')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                if (temp[0] == '>')
                {
                    memmove(temp, temp+1, strlen(temp));
                }
                strcpy(id, temp);
            }
            else if(strcasecmp(temp,"ruta") == 0)
            {
                temp = strtok(NULL, " ");
                char* aux = malloc(strlen("plop") + 100);
                strcpy(aux, temp+1);
                char aux2[150];
                memset(aux2, 0, sizeof(aux2));
                strcpy(aux2, aux);

                while(strcmp(&aux2[strlen(aux2) -1], "\"") != 0)
                {
                    temp = strtok(NULL, " ");
                    strcat(aux, " ");
                    strcat(aux, temp);
                    strcpy(aux2, aux);
                }
                strcat(aux2, "\0");
                char* otroaux = malloc(strlen("plop") + 100);
                strncpy(otroaux, aux2 + 1, strlen(aux2) - 2);
                otroaux[strlen(aux2)-2] = '\0';
                strcpy(path, otroaux);
            }
            else if(strcasecmp(temp, "\n") == 0)
            {

            }
            else if(strcasecmp(temp, "\r\n") == 0)
            {

            }
            else
            {
                printf("\t>" ROJO "Comando invalido %s, para desplegar un reporte se necesita el nombre, la ruta y el id de particion.\n" RESET, temp);
            }
            temp = strtok(NULL, "->");
            if(temp!=NULL)
            {
                if(temp[0] == ' ')
                {
                    temp++;
                }
            }
        }
        if((strcasecmp(ruta_Destino, "") != 0) && (strcasecmp(id, "") != 0) && (strcasecmp(name, "") != 0))
        {
            Crear_Directorios_Con_Nombre(ruta_Destino+1);
            Rep(ruta_Destino, id, name, path);
        }
    }
    else if(strcasecmp(palabra, "exec") == 0)
    {
        char *temp = strtok(linea, " ");
        temp = strtok(NULL, "->");
        if (strcasecmp(temp,"path")==0)
        {
            temp = strtok(NULL, " ");
            char* aux = malloc(strlen("plop") + 100);
            strcpy(aux, temp+1);
            char aux2[150];
            memset(aux2, 0, sizeof(aux2));
            strcpy(aux2, aux);

            if (aux[0]=='\"')
            {
                while(strcmp(&aux2[strlen(aux2) -1], "\"") != 0)
                {
                    temp = strtok(NULL, " ");
                    strcat(aux, " ");
                    strcat(aux, temp);
                    strcpy(aux2, aux);
                }
                strcat(aux2, "\0");
                char* otroaux = malloc(strlen("plop") + 100);
                strncpy(otroaux, aux2 + 1, strlen(aux2) - 2);
                otroaux[strlen(aux2)-2] = '\0';
                strcpy(ruta_Disco, otroaux);
            }
            else
            {
                strcpy(ruta_Disco, aux2);
            }

            FILE *f = fopen (ruta_Disco, "r");

            if(f != NULL)
            {
                while(feof(f)==0)
                {
                    strcpy(Normal,"");
                    fgets(Normal,500,f);
                    iWhile = 0;
                    strcpy(aux_String, Normal);
                    Normal[strlen(Normal)-1] = '\0';
                    if(aux_String[0] != '#')
                    {
                        if((strcasecmp(Normal,"") != 0) && (strlen(Normal) > 2))
                        {
                            strcpy(Comando, Normal);
                            printf(CELESTE "Script: %s.\n" RESET, Normal);
                            strcpy(Linea_Comparable, Normal);
                            char *lista = strtok(Normal, " ");
                            TAG_Script = 1;
                            Analizar_Comando(Linea_Comparable, lista);
                        }
                    }
                    else
                    {
                        printf(AZUL ">> %s\n" RESET, aux_String);
                    }
                }
            }
            else
            {
                printf("\t>" ROJO "No se encontro el archivo\n" RESET);
            }
        }
        else if(strcasecmp(temp, "\n") == 0)
        {

        }
        else
        {
            printf("\t>" ROJO "Para ejecutar un script unicamente debe ingresar la ruta del archivo con el comando $path.\n" RESET);
        }
    }
    else if(strcasecmp(palabra, "exit") == 0)
    {
        TAG_Script = -7;
    }
    else if(strcasecmp(palabra, "clear") == 0)
    {
        system("clear");
    }
    else
    {
        printf("\t>" ROJO "Comando invalido. Por favor intentelo nuevamente...\n" RESET);
    }

    //Valido si seguir leyendo comandos o realizar el script
    if(TAG_Script == 0)
    {
        Ingresar_Comando();
    }
    else if(TAG_Script == -7)
    {

    }
    else
    {
        TAG_Script = 0;
    }

}
