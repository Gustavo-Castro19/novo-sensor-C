#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUM_DATAS (6)
#define MAX_ID_LEN   (32)
#define MAX_VALUE_LEN (32)
#define MAX_LINE (3000)
#define MAX_SENS (10000)


typedef struct{
time_t timestamp;
char id[MAX_ID_LEN];
char value[MAX_VALUE_LEN];
}sensor_t;

int get_sensors(char* file,sensor_t *catching,int size);

sensor_t busca_binaria(time_t target, sensor_t *times, int size);

bool transforma_data(char *day, char *month, char *year, char *hour, char *minutes, char *second, int datas[NUM_DATAS]);

time_t converter_para_timestamp(int datas[NUM_DATAS]);

int main(int argc,char *argv[]){

  if(argc!=8){
    fprintf(stderr,"ERRO: parametros passados incorretamente\n USO: %s <NOME_ARQUIVO> data e hora formato <DD MM YY HH MM SS>",argv[0]);
    return 1;
  }

  int data[NUM_DATAS];
  if(!transforma_data(argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],data)) return 1;

  time_t targ=converter_para_timestamp(data);
  
  if (targ == (time_t)-1) {
    fprintf(stderr, "Data/hora final inválida. Corrija e tente novamente.\n");
    return 1;
  }

  static sensor_t sensors[MAX_SENS];
  sensor_t find;
  int counter=get_sensors(argv[1],sensors,MAX_SENS);
  if(counter<0) return 1;
  find=busca_binaria(targ,sensors,counter);
  char tempo_str[64];
  struct tm *tm_info = localtime(&find.timestamp);
  strftime(tempo_str, sizeof(tempo_str), "%d/%m/%Y %H:%M:%S", tm_info);
  printf("a leitura mais proxima de %s foi\n",tempo_str);
  printf("%ld %s %s\n",find.timestamp,find.id,find.value);
  return 0;
}
bool transforma_data(char *day, char *month, char *year, char *hour, char *minutes, char *second, int datas[NUM_DATAS]) {
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

time_t converter_para_timestamp(int datas[NUM_DATAS]) {
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


sensor_t busca_binaria(time_t target, sensor_t *times, int size) {
  int low = 0, high = size - 1;
  int pivot;

  while (low <= high) {
    pivot = (low + high) / 2;
    if (times[pivot].timestamp == target) {
      return times[pivot];
    } else if (times[pivot].timestamp < target) {
      low = pivot + 1;
    } else {
      high = pivot - 1;
    }
  }

  if (low >= size) return times[high];
  if (high < 0) return times[low];

  if ((time_t)labs(times[low].timestamp - target) < (time_t)labs(times[high].timestamp - target)) {
    return times[low];
  } else {
    return times[high];
  }
}

int get_sensors(char* file, sensor_t *catching, int size) {
    FILE* fd = fopen(file, "r");
    if (!fd) {
        perror("nao foi possivel abrir o arquivo fornecido");
        return -1;
    }

    int count = 0;
    while(count<size){
    int res=fscanf(fd, "%ld %31s %31s", &catching[count].timestamp, catching[count].id, catching[count].value);
    if (res==3) count++;
    else if(res ==EOF){
      break;
    }
    else{
      fprintf(stderr, "houve um erro na leitura do arquivo, verifique se o arquivo esta no formato correto <TIMESTAMP> <ID_SENSOR <VALOR>\n");
      fprintf(stderr, "erro ao ler o arquivo programa encerrado\n");
      fclose(fd);
      return -1;
    }
  }
    fclose(fd);
    return count;
}
