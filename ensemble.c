#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

void openFile(char fileName[]);
void parameter(char c);
void readData(int data);
void opcIONS();	
void opcIONSWind();
void readOneBlock();
void sizeFile();
void renameFile(char fileName[]);
void closeFile();
void randomEnsemble();

FILE *fileToken, *fileTokenNew;
char parameter1[4] = " ", parameter2[4]= " ";  // Subtracao dos dois resultara no total de dados para leitura
int cont = 0, cont1 = 0, cont2 = 0;
int result, result2, difference;
int OK_PARAMETER = 0;
float Nrand;

int main(int argc, char** argv){
	char c, fileName[15];
	int opc;


	if(argc != 3){
		printf("Error on input data. Expected \"FILENAME OPC\". Check input parameters...");
		exit(0);
	}
	
	// Parameter
	opc = atoi(argv[1]);
	strcpy(fileName, argv[2]);	

	// Rand
	srand (time(NULL));
	Nrand =  rand();
	

	openFile(fileName);

	while((c = getc(fileToken)) != EOF){
		if(cont >= 8 && cont <= 37){ 
			parameter(c); 	// Read Parameter
		}else{
			putc(c, fileTokenNew);
		}
		if(cont == 38){
			readData(opc);
			
			//break;
			//getchar();
	
		}		
					
		cont++;
	}
	
	if (c != EOF){
		putc(c, fileTokenNew);
	}	

	closeFile();
	renameFile(fileName);
	sizeFile();

	return 0;
}

void openFile(char fileName[]){
	
	fileToken = fopen(fileName, "r+");
	fileTokenNew = fopen("testeNOVO.txt", "w+");	


	if(!fileToken){
		printf("ERROR: File Token NOT Found!!! \n");
		exit(0);
	}
	if(!fileTokenNew){
		printf("ERROR: File Token NEW NOT created!!! \n");
		exit(0);
	}
}
void closeFile(){
	fclose(fileToken);
	fclose(fileTokenNew);
}

void parameter(char c){
	putc(c, fileTokenNew); 		// Escrita aqui pois jah fez uma leitura antes de chamar a função
	while(cont < 16){
		c = getc(fileToken);
		if (cont >= 8 && cont < 11){
		        parameter1[cont1] = c;
		        cont1++;
		        result = atoi(parameter1); 
		}else if(cont >= 11 && cont < 15){
			parameter2[cont2] = c;
		        cont2++;
		        result2 = atoi(parameter2); 
		}else{
			difference = result2 - result + 1;  	// Total de dados para leitura
			//printf("VALOR 1: %i\n", result);	//Conferido OK
			//printf("VALOR 2: %i\n", result2);	//Conferido OK
			//printf("DIFERENCA: %i\n", difference);
			if(difference > 0){
				OK_PARAMETER = 1;
				//printf("DIFFERENCE: %i\n", difference);
			}else{
				printf("ERROR: PARAMETER = %i \n\n", difference);
				exit(0);
			}
		        cont1=0;
		        cont2=0;
		}
		putc(c, fileTokenNew);
		cont++;	
	}
	// Read 21 data not important
	while(cont >= 16 && cont <= 37){
		c = getc(fileToken);
		//printf("%c", c);
		putc(c, fileTokenNew);
		cont++;		
	}
}

void readData(int data){
	int i;
	char c;

	switch(data){
		case 1: // Electronic Concentration
			opcIONS();
			//exit(0);
			for(i = 0; i < 22; i++){
				readOneBlock();			
			}
			cont = 0;
			break;
		case 2: // Temperature
			readOneBlock();	
			opcIONS();
			for(i = 0; i < 21; i++){
				readOneBlock();			
			}
			cont = 0;
			//opc2();
			break;
		case 3: // Electronic Concentration IONS
			readOneBlock(); // Eletronic Concentration
			readOneBlock();	// Temperature
			for(i= 0; i < 7; i++){	
				opcIONS();	
				readOneBlock();
				readOneBlock();	
			}
			cont = 0;
			break;
		case 4: // Temperature IONS
			readOneBlock();	// Eletronic Concentration
			readOneBlock();	// Temperature
			for(i = 0; i < 7; i++){
				readOneBlock();
				opcIONS();
				readOneBlock();	
			}
			cont = 0;
			break;		
		case 5: // Wind IONS
			readOneBlock();	// Eletronic Concentration
			readOneBlock();	// Temperature
			for(i = 0; i < 7; i++){
				readOneBlock();
				readOneBlock();
				opcIONSWind();	
			c = getc(fileToken);
			putc(c, fileTokenNew);
			//c = getc(fileToken);
			//putc(c, fileTokenNew);	
			//getchar();
								
			}

			cont = -1;
			break;		
		default:
			break;
		
	}
}

void opcIONS(){	// Eletronic Concentration IONS
	int i = 0, contData = 0, totalData = 1, j = 0;
	char c, dataC[12];
	double dataOld = 0.0, dataNew = 0.0;
	strcpy(dataC, "");
	
	//readOneBlock();
	//readOneBlock();
		
		i = 0;
		while(i != difference){
			Nrand =  rand();
			while ((c = getc(fileToken)) == 32){ 	// Lê espaco
				putc(c, fileTokenNew);	
				//printf("%c", c);	
			}
			contData = 0;
			dataC[contData] = c;
			putc(c, fileTokenNew);
			//printf("%c", c);
			while ((c = getc(fileToken)) != 32){
				contData++;
				dataC[contData] = c;
				//printf("%c", c);
				putc(c, fileTokenNew);	
			}
			putc(c, fileTokenNew);		// Escreve ultimo caracter lido
			//exit(0);
					 
			// Transform Char to Double
			dataOld = atof(dataC);

			// Noise
			randomEnsemble();
			dataNew = dataOld * Nrand;
			//dataNew = dataNew * 1.2;			

			// Transform Double for Exponential 
			int exponent = (int)log10(fabs(dataNew));
			float mantissa = dataNew / pow(10, exponent);

			if(totalData % 5 != 0 && totalData != difference){	// Caso for o último item da linha, é preciso dar um enter 		
				// Volta ponteiro no arquivo e escreve novo dado
				fseek(fileTokenNew, -13, SEEK_CUR);
				fprintf(fileTokenNew, "%f" ,mantissa);
				putc('E', fileTokenNew);
				if(dataC[9] == '-'){
					putc('-', fileTokenNew);
					exponent = exponent * (-1);	
				}else{
					putc('+', fileTokenNew);
				}
				if(exponent < 10)
					fprintf(fileTokenNew, "%i" , 0);
					fprintf(fileTokenNew, "%i", exponent);
			}else{
				fseek(fileTokenNew, -14, SEEK_CUR);
				fprintf(fileTokenNew, "%f" ,mantissa);
				putc('E', fileTokenNew);
				if(dataC[9] == '-'){
					putc('-', fileTokenNew);
					exponent = exponent * (-1);	
				}else{
					putc('+', fileTokenNew);
				}
				if(exponent < 10){
					fprintf(fileTokenNew, "%i" , 0);
				}
				fprintf(fileTokenNew, "%i\n", exponent);
			}
			totalData++;
			strcpy(dataC, "");		// Zera String
			
			//c = getc(fileToken);
			putc(c, fileTokenNew);	
			i++;
		}
			
	totalData = 1;	
}
void opcIONSWind(){
	int i, j, algoritmo, totalData = 1;
	char c, dataC[11];
	double dataOld = 0.0, dataNew = 0.0;	

	while (totalData <= difference){
		Nrand =  rand();
		algoritmo = 0; // Total de casas lida
		c = getc(fileToken);
		while(c == 32 || ((c < 47) || (c > 70))){ //Leh os espacos
			putc(c, fileTokenNew);
			c = getc(fileToken);
		}
		if(c == '-'){
			putc(c, fileTokenNew);
			c = getc(fileToken);
		}
		dataC[algoritmo] = c;
		putc(c, fileTokenNew);
		for(j = 0; j < 11; j++){
			algoritmo++;
			c = getc(fileToken);
			dataC[algoritmo] = c;
			putc(c, fileTokenNew);
		}
		/*for(j = 0; j < 12; j++){
			printf("DATAC[%i] = %c INT: %i \n", j, dataC[j], dataC[j]);
		}

		getchar();*/
		
		// Transform Char to Double
		dataOld = atof(dataC);

		// Noise
		//randomEnsemble();
		dataNew = dataOld * Nrand;
		//dataNew = dataNew * 1.2;

		// Transform Double for Exponential 
		int exponent = (int)log10(fabs(dataNew));
		float mantissa = dataNew / pow(10, exponent);

		/*printf("MANTISSA: %f, Exponent: %i\n", mantissa, exponent);
		getchar();*/
		if(dataOld != 0){
			//printf("AQUI. TOTALDATA: %i\n", totalData);
			//getchar();
			if(totalData % 5 != 0 && totalData != difference){
				//printf("AQUI DENTRO. TOTALDATA: %i\n", totalData);
				// Volta ponteiro no arquivo e escreve novo dado
				fseek(fileTokenNew, -12, SEEK_CUR);
				//getchar();
				fprintf(fileTokenNew, "%f" ,mantissa);
				putc('E', fileTokenNew);
				if(dataC[9] == '-'){
					putc('-', fileTokenNew);
					exponent = exponent * (-1);	
				}else{
					putc('+', fileTokenNew);
				}				
				if(exponent < 10)
					fprintf(fileTokenNew, "%i" , 0);
				fprintf(fileTokenNew, "%i", exponent);
			}else{
				// Volta ponteiro no arquivo e escreve novo dado
				fseek(fileTokenNew, -12, SEEK_CUR);
				fprintf(fileTokenNew, "%f" ,mantissa);
				putc('E', fileTokenNew);
				if(dataC[9] == '-'){
					putc('-', fileTokenNew);
					exponent = exponent * (-1);	
				}else{
					putc('+', fileTokenNew);
				}
				if(exponent < 10)
					fprintf(fileTokenNew, "%i" , 0);
				fprintf(fileTokenNew, "%i", exponent);
				c = getc(fileToken); // Le o enter
				putc(c, fileTokenNew);
			}
	
		}else{
			//fprintf(fileTokenNew, "%i", 0);
			//fseek(fileTokenNew, -1, SEEK_CUR);
		}
		//putc(c, fileTokenNew);
		//printf("DATAC: %s. DATACFLOAT: %f. NUMERO %f. EXPOENTE: %i\n", dataC, dataOld, mantissa, exponent);

	
		if(c == EOF){
			printf("TotalData= %i Difference: %i\n", totalData, difference);
			return;
		}
		totalData++;
		strcpy(dataC, "");		// Zera String
		//putc(c, fileTokenNew);
		i++;
	}

}

void readOneBlock(){
	int i;
	char c;

	for (i = 0; i < difference ; i++){
	
		while ((c = getc(fileToken)) == 32){ 	// Lê espaco
			//printf("%c", c);
			putc(c, fileTokenNew);		
		}
		putc(c, fileTokenNew);
		//printf("%c", c);
		while ((c = getc(fileToken)) != 32 && c != EOF){ 	// Lê Dado
			putc(c, fileTokenNew);
			//printf("%c", c);	
			
		}
		if (c != EOF){
			putc(c, fileTokenNew);
		}	
		//printf("%c", c);
	}
	//getchar();

	/*
	for(i = 0; i < ((difference * 14) + (difference % 5)); i++){
	//for(i = 0; i < ((difference * 14) + ((difference * 14) % 5)); i++){
		c = getc(fileToken);
		putc(c, fileTokenNew);
		printf("%c", c);
	}
	exit(0);
	c = getc(fileToken);
	putc(c, fileTokenNew);*/
}

void sizeFile(){
	long sizeOld, sizeNew;

	sizeOld = ftell(fileToken);
	sizeNew = ftell(fileTokenNew);

	if(sizeOld == sizeNew){
		printf("SIZE FILES OK\n");
		return;	
	}else{
		printf("ERROR: SIZE FILES\n");
		exit(0);
	}
}
void renameFile(char fileName[]){
	char file[15];

	strcpy(file, fileName);

	rename(fileName, "OLD");
	rename("testeNOVO.txt", file);
}
void randomEnsemble(){
	float aux;

	aux = Nrand;
	aux = (aux / RAND_MAX) * 0.4;
	aux = aux - 0.2;		// Para ter número menor de 1
	aux = aux + 1;
	Nrand = aux;
}
