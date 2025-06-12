#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_SENS_TYPE (4)
#define MAX_SENS_READS (8000)
#define MAX_LINE (1000)
#define MAX_ID_LEN   (32)
#define MAX_VALUE_LEN (32)

typedef struct {
  time_t timestamp;
  char sens_id[MAX_ID_LEN];
  char value[MAX_VALUE_LEN];
} sensor_t;

int get_sensors(FILE *fp, sensor_t *catching, char *archive_names[], int size);
void order_timestamp(sensor_t *sensor, int size);
void catch_pos_file(char *pars, char *pos_arc[], int *arc_count);
void record_archives(char *filenames[], sensor_t *sensor_reads, int size);

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "erro: parametros passados incorretamente\n usagem: %s <NOME_DO_ARQUIVO>\n", argv[0]);
    return 1;
  }

  FILE *fd = fopen(argv[1], "r");
  sensor_t sensors_list[MAX_SENS_READS];
  char *sensor_names[MAX_SENS_TYPE]; 

  if (!fd) {
    fprintf(stderr, "ERRO: nao foi possivel ler o texto do arquivo\n");
    return 1;
  }

  int sensor_count = 0;

  sensor_count = get_sensors(fd, sensors_list, sensor_names, MAX_SENS_READS);

  order_timestamp(sensors_list, sensor_count);
  record_archives(sensor_names, sensors_list, sensor_count);

  //printf("Processados %d registros de sensores.\n", sensor_count);
  for(int i=0;i<MAX_SENS_TYPE;i++){
    free(sensor_names[i]);
  }
  fclose(fd);
  return 0;
}

int get_sensors(FILE *fp, sensor_t *catching, char *archive_names[], int size) {
  static char line[MAX_LINE];
  char *parser;
  int count = 0, arc_count = 0;

  for (int i = 0; i < MAX_SENS_TYPE; i++) {
      archive_names[i] = NULL;
  }

  while (fgets(line, sizeof(line), fp)) {
    if (count >= size) break;

    parser = strtok(line, " ");
    if (!parser) continue;
    catching[count].timestamp = (time_t)atol(parser);

    parser = strtok(NULL, " ");
    if (!parser) continue;
    strncpy(catching[count].sens_id, parser, MAX_ID_LEN - 1);
    catching[count].sens_id[MAX_ID_LEN - 1] = '\0';
    if (strlen(parser)>MAX_ID_LEN) {
      fprintf(stderr, "erro: alguns valores de ID excedem o limite do sistema havera truncamento no arquivo final\n");
      continue;
    }
    catch_pos_file(parser, archive_names, &arc_count);

    parser = strtok(NULL, "\n");
    if (!parser) continue;
    strncpy(catching[count].value,parser,MAX_VALUE_LEN-1);
    catching[count].value[MAX_VALUE_LEN-1]='\0';
    if (strlen(parser)>MAX_VALUE_LEN) {
      fprintf(stderr, "erro: o valor de alguma linha excede o limite do sistema havera truncamento no arquivo final\n");
      continue;
    }
    
    count++;
  }
  
  return count;
}

void order_timestamp(sensor_t *sensor, int size) {
  sensor_t temp;
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (sensor[j].timestamp > sensor[j + 1].timestamp) {
        temp = sensor[j + 1];
        sensor[j + 1] = sensor[j];
        sensor[j] = temp;
      }
    }
  }
}

void catch_pos_file(char *pars, char *pos_arc[], int *arc_count) {
  if (*arc_count == MAX_SENS_TYPE) return;
  for (int i = 0; i < *arc_count; i++) {
    if (strcmp(pos_arc[i], pars) == 0) return;
  }
  pos_arc[*arc_count] =strdup(pars);
  (*arc_count)++;
}

void record_archives(char *filenames[], sensor_t *sensor_reads, int size) {
  for (int i = 0; i < MAX_SENS_TYPE && filenames[i] != NULL; i++) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.txt", filenames[i]);
    FILE *new_fd = fopen(filename, "a");
    if (!new_fd) {
      perror("nao foi possivel abrir o arquivo corretamente");
      continue;
    }
    for (int z = 0; z < size; z++) {
      if (strcmp(sensor_reads[z].sens_id, filenames[i]) == 0) {
        fprintf(new_fd, "%ld %s %s\n", sensor_reads[z].timestamp, sensor_reads[z].sens_id, sensor_reads[z].value);
      }
    }
    fclose(new_fd);
  }
}
