#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SENS_TYPE (4) 
#define MAX_SENS_READS (8000)
#define MAX_LINE (1000)

typedef data{
  time_t timestamp;
  char *sens_id;
  char *value;
}sensor_t;

sensor_t *get_sensors[](FILE *fp,sensor_t catching,int size);
bool sens_exist(char *parser,char *list_sens[],int size);
void order_timestamp(sensor_t *sensor,int size);
int main(int argc, char *argv[]) {

  if(argc!=2 ){
    fprintf(stderr,"erro: parametros passados incorretamente\n usagem: %s <NOME_DO_ARQUIVO>",argv[1]);
    return 1;
  }

  FILE *in_file=fopen(argv[1],"r");
  sensor_t sensors_list[MAX_SENS_READS];

  if (!in_file) {
    fprintf(stderr,"ERRO: nao foi possivel ler o texto do arquivo")
  }

  char *list_sens[MAX_SENS_TYPE];
  list_sens=get_sensors(in_file,sensors_list);


  return 0;
}

sensor_t get_sensors[](FILE *fp,sensor_t *catching,int size){
  char *sensors[MAX_SENS_TYPE];

  static char line[MAX_LINE];
  char *parser;
  int count=0;

  while(fgets(line,sizeof(line),fp)){
    if(count>MAX_SENS_READS) break;

    parser=strtok(line,"");
    if(!parser) continue;
    catching[count].timestamp=(time_t)atol(parser);

    parser=strtok(NULL,"");
    if(!parser) continue;
    strcpy(catching[count].sens_id,parser);

    parser=strtok(NULL,"\n");
    if(!parser) continue;
    strcpy(catching[count].value,parser);

    count++;
  }
  return sensors;

}

void order_timestamp(sensor_t *sensor,int size){
  sensor_t temp;
  for(int i=0; i<size-1; i++ ){
    for (int j=0;j<size-i-1;j++) {
      if(sensor[j].timestamp>sensor[j+1].timestamp){
        temp=sensor[j+1];
        sensor[j+1]=sensor[j];
        sensor[j]=temp;
      }
    }
  }
}
