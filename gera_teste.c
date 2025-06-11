#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define TAM_STRING 16
#define NUM_CAMPOS_DATA 6
#define MAX_SENSORES 4 

int gera_int();
double gera_double();
bool gera_booleano();
void gera_codigo(char *dest);
time_t gerar_timestamp_aleatorio(time_t inicial, time_t final);
bool transforma_data(char *day, char *month, char *year, char *hour, char *minutes, char *second, int datas[NUM_CAMPOS_DATA]);
time_t converter_para_timestamp(int datas[NUM_CAMPOS_DATA]);

int main(int argc, char *argv[]) {
  int date_args = 2 * NUM_CAMPOS_DATA; 

  if (argc < date_args + 1 + 2) { 
    fprintf(stderr, "Argumentos insuficientes. Mínimo necessário: %d argumentos totais.\n", date_args + 3);
    fprintf(stderr, "Uso: %s <dia_ini> <mes_ini> <ano_ini> <hora_ini> <min_ini> <seg_ini> <dia_fim> <mes_fim> <ano_fim> <hora_fim> <min_fim> <seg_fim> <sensor1> <tipo1> [<sensor2> <tipo2> ...]\n", argv[0]);
    return 1;
  }

  int sensor_args = argc - 1 - date_args; 
  printf("Total args: %d, Date args: %d, Sensor args: %d\n", argc, date_args, sensor_args);

  if (sensor_args % 2 != 0) {
    fprintf(stderr, "Número de argumentos de sensores deve ser par (nome e tipo para cada sensor).\n");
    return 1;
  }

  int num_sensores = sensor_args / 2;
  printf("Número de sensores detectados: %d\n", num_sensores);

  if (num_sensores > MAX_SENSORES) {
    fprintf(stderr, "Máximo de %d sensores permitidos. Você informou %d sensores.\n", MAX_SENSORES, num_sensores);
    return 1;
  }

  if (num_sensores == 0) {
    fprintf(stderr, "Pelo menos um sensor deve ser especificado.\n");
    return 1;
  }

  srand(time(NULL));
  int datas_ini[NUM_CAMPOS_DATA], datas_fim[NUM_CAMPOS_DATA];

  printf("Validando data inicial...\n");
  if (!transforma_data(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], datas_ini)) {
    return 1;
  }

  printf("Validando data final...\n");
  if (!transforma_data(argv[7], argv[8], argv[9], argv[10], argv[11], argv[12], datas_fim)) {
    return 1;
  }

  time_t t_ini = converter_para_timestamp(datas_ini);
  if (t_ini == (time_t)-1) {
    fprintf(stderr, "Data/hora inicial inválida. Corrija e tente novamente.\n");
    return 1;
  }

  time_t t_fim = converter_para_timestamp(datas_fim);
  if (t_fim == (time_t)-1) {
    fprintf(stderr, "Data/hora final inválida. Corrija e tente novamente.\n");
    return 1;
  }

  if (t_ini >= t_fim) {
    fprintf(stderr, "A data/hora inicial deve ser anterior à final.\n");
    return 1;
  }

  char *sensores[MAX_SENSORES];
  char *tipos[MAX_SENSORES];

  for(int i = 0; i < num_sensores; i++) {
    int sensor_start_idx = 1 + date_args; 
    sensores[i] = argv[sensor_start_idx + i * 2];     
    tipos[i] = argv[sensor_start_idx + i * 2 + 1];    
    printf("Sensor %d: %s (tipo: %s)\n", i + 1, sensores[i], tipos[i]);

    if (strcmp(tipos[i], "int") != 0 && 
      strcmp(tipos[i], "double") != 0 && 
      strcmp(tipos[i], "bool") != 0 && 
      strcmp(tipos[i], "string") != 0) {
      fprintf(stderr, "Tipo de dado inválido para sensor %s: %s\n", sensores[i], tipos[i]);
      fprintf(stderr, "Tipos válidos: int, double, bool, string\n");
      return 1;
    }
  }

  FILE *fd = fopen("teste.txt", "w");
  if (!fd) {
    fprintf(stderr,"Erro ao abrir o arquivo");
    return 1;
  }

  printf("Gerando dados para %d sensores...\n", num_sensores);

  for (int i = 0; i < 8000; i++) {
    int s=rand()%4;
    time_t t = gerar_timestamp_aleatorio(t_ini, t_fim);
    fprintf(fd, "%ld %s ", t, sensores[s]);
    if (strcmp(tipos[s], "int") == 0) {
      fprintf(fd, "%d\n",gera_int());
    } else if (strcmp(tipos[s], "double") == 0) {
      fprintf(fd, "%.4lf\n",gera_double());
    } else if (strcmp(tipos[s], "bool") == 0) {
      fprintf(fd, "%s\n", gera_booleano() ? "true" : "false");
    } else if (strcmp(tipos[s], "string") == 0) {
      char str[TAM_STRING];
      gera_codigo(str);
      fprintf(fd, "%s\n",str);
    }
  }
  fclose(fd);
  printf("Arquivo teste.txt gerado com sucesso!\n");
  return 0;
}

int gera_int() {
  return rand() % 101;
}

double gera_double() {
  return ((double)rand() / RAND_MAX) * 101.0;
}

bool gera_booleano() {
  return rand() % 2;
}

void gera_codigo(char *dest) {
  const char charset[] = "abcdefghiklmnopqrstuvwxyz";
  for (int i = 0; i < TAM_STRING - 1; i++) {
    dest[i] = charset[rand() % (sizeof(charset) - 1)];
  }
  dest[TAM_STRING - 1] = '\0';
}

time_t gerar_timestamp_aleatorio(time_t inicial, time_t final) {
  return inicial + rand() % (int)(final - inicial + 1);
}



bool transforma_data(char *day, char *month, char *year, char *hour, char *minutes, char *second, int datas[NUM_CAMPOS_DATA]) {
  datas[0] = atoi(day);
  datas[1] = atoi(month);
  datas[2] = atoi(year);
  datas[3] = atoi(hour);
  datas[4] = atoi(minutes);
  datas[5] = atoi(second);

  bool IsValid=
    (datas[1] > 0 && datas[1] <= 12) &&    
    (datas[2] > 1969) &&                   
    (datas[3] >= 0 && datas[3] < 24) &&    
    (datas[4] >= 0 && datas[4] < 60) &&    
    (datas[5] >= 0 && datas[5] < 60); 


  if (!IsValid) {
    fprintf(stderr, "Houve algum erro na formatação da data. Lembre-se que o formato é DD MM AAAA HH MM SS\n");
    fprintf(stderr, "Data informada: %02d/%02d/%04d %02d:%02d:%02d\n", 
            datas[0], datas[1], datas[2], datas[3], datas[4], datas[5]);
  }
  return IsValid;
}

time_t converter_para_timestamp(int datas[NUM_CAMPOS_DATA]) {
  struct tm t = {0}; 
  t.tm_year = datas[2] - 1900;
  t.tm_mon = datas[1] - 1;
  t.tm_mday = datas[0];
  t.tm_hour = datas[3];
  t.tm_min = datas[4];
  t.tm_sec = datas[5];
  t.tm_isdst = -1;
  return mktime(&t);
}
