#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main ( int argc, char *argv[] )
{
	//Variables
	pid_t pid;
	pid_t pid2;
	int NumberAux = 0;
	int idpapi;
	int aux2;
    	int option;
	int number[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int mypipefd[2]; //tuberia numeros random
	int mypipe[2]; //tuberia para id de procesos
	int pipe3[2]; //tuberia para mandar del proceso 1 al 3
	int pipe4[2]; //tuberia para mandar del proceso 3 al 1 
	int cleaningpipe[2]; //Tuberia para limpiar el proceso 1
	int cleaningpipe2[2];//tuberia para limpiar el proceso 3	
	int pipe5[2]; //Tuberia que conecta el proceso 2 con el 3
	int pipe6[2]; //Mando el id del nieto
	int nieto;
	int Flag;
	int Bandera;//bandera para limpiar el arreglo de numeros pares del proceso 3
	int NuevaBandera = 0;
	int one = 1;
	int cero = 0;
	char idhijo[10];
	char aux[10];
	int buff;
	char original[2];
	char originalLista[40];
	char modificado[2];
	char modificadoLista[40];
	char par[2];
	char parLista[40];
	int numberRead;
	int process3[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int esPar;
	//Create pipes between processes/
	pipe(mypipefd);
	pipe(mypipe);
	pipe(cleaningpipe);
	pipe(cleaningpipe2);
	pipe(pipe3);
	pipe(pipe4);
	pipe(pipe5);
	pipe(pipe6);
	//Crear los procesos hijos
	pid = fork();
	pid2 = fork();
	if(pid ==  0 && pid2 > 0) //Proceso hijo
	{
		for(int contadorP1 = 0; contadorP1 < 20; contadorP1++){
			//Obtener el estado:
			sprintf(idhijo,"%d",getpid());
			write(mypipe[1], &idhijo, sizeof(idhijo));
			//Arreglo de numeros random
			NumberAux = rand()%99;			
			number[contadorP1] = NumberAux;
			write(pipe3[1], &NumberAux, sizeof(NumberAux));
			read(pipe4[0], &esPar, sizeof(cero));
			write(mypipefd[1], &NumberAux, sizeof(NumberAux));
			if(esPar == 1)
			{
				//Eliminar numero porque es par
				number[contadorP1] = 0;
				NumberAux = 0;
			}
			write(mypipefd[1], &NumberAux, sizeof(NumberAux));
			//Limpiar por completo el arreglo de numeros random
			read(cleaningpipe[0], &Flag,sizeof(cero));
                        if(Flag != 0){
                                for(int x = 0; x < 20; x++){
                                        number[x] = 0;
                                }
				printf("Se ha eliminado el arreglo random");
                        	Flag = 0;
                        }
		}
	}
	else if(pid > 0 && pid2 > 0) //Proceso padre
	{
		//Proceso padre/
		idpapi = getpid();
		do{
			write(cleaningpipe[1], &cero,sizeof(cero));
			write(cleaningpipe2[1], &cero,sizeof(cero));
			printf("\nChoose your option\n");
        		printf("1.Mostrar numeros\n2.Vaciar lista \n3. Detener por completo el proceso (kill)\n4. Mostrar estados\n");
        		scanf("%d",&option);
			switch(option) {
            		case 1:
				//IF NUEVA BANDERA ACTIVADA SETEAR ORIGINAL,ORIGINALLISTA, MODIFICADO,MODIFICADOLISTA,PAR,PARLISTA.
				if(NuevaBandera != 0){
					sprintf(original,"%d",0);
                        		sprintf(originalLista,"%d",0);
					sprintf(modificado,"%d",0);
					sprintf(modificadoLista,"%d",0);
					sprintf(par,"%d",0);
					sprintf(parLista,"%d",0);
                 			NuevaBandera = 0;
				}
				printf("Ejecutando proceso padre\n");
				//Imprimir arreglo de num random original
				read(mypipefd[0], &buff,sizeof(buff));
				sprintf(original, "%d", buff);
				strcat(original,",");
				strcat(originalLista, original);
	        		printf("original: %s\n", originalLista);
				//Imprimir arreglo de num random modificado
				read(mypipefd[0], &buff,sizeof(buff));
				sprintf(modificado, "%d",  buff);
				strcat(modificado,",");
				strcat(modificadoLista, modificado);
	        		printf("modificado: %s\n", modificadoLista);
				//Imprimir arreglo de num pares
				read(pipe5[0], &buff,sizeof(buff));
				sprintf(par, "%d",  buff);
				strcat(par,",");
				strcat(parLista, par);
	        		printf("pares: %s\n", parLista);
                		break;
            		case 2:
				NuevaBandera = 1;
				write(cleaningpipe[1], &one,sizeof(one));
				write(cleaningpipe2[1], &one,sizeof(one));
                		break;
            		case 3:
				kill(idpapi,SIGKILL);
				printf("El proceso se ha eliminado.\n");
                		break;
            		case 4:
                		printf("PID Padre:\n");
                		printf("%d\n",idpapi);
				printf("PID Hijo:\n");
				sleep(2.7);
				close(mypipe[1]);
				read(mypipe[0], aux,10);
                		printf("%s\n", aux);
				printf("PID Nieto:\n");
				read(pipe6[0], &aux2,sizeof(aux2));
				sprintf(aux,"%d", aux2);
				printf("%s\n", aux);
                		break;
            		default:
                		printf("Invalid option\n");
                		break;
        		}
		}while(1==1);
	}
	else if (pid == 0 && pid2 == 0) //Proceso nieto
	{
		nieto = getpid();
		write(pipe6[1],&nieto, sizeof(nieto));
		for(int z = 1; z < 20; z++){
			sleep(4);
			read(pipe3[0], &numberRead, sizeof(numberRead));//Recibir el random
			if (numberRead%2 ==0){
				process3[z] = numberRead;
				write(pipe4[1], &one, sizeof(one));
				write(pipe5[1], &numberRead, sizeof(numberRead));
			}
			else
			{
				write(pipe4[1], &cero, sizeof(cero));
				write(pipe5[1], &cero, sizeof(cero));
			}
			//Limpiar por completo el arreglo de numeros pares
			read(cleaningpipe2[0], &Bandera,sizeof(cero));
                        if(Bandera != 0){
                                for(int y = 0; y < 20; y++){
                                        process3[y] = 0;
                                }
				printf("Se ha eliminado el arreglo pares.\n");
                        	Flag = 0;
                        }
		}
	}
	return 0;
}
