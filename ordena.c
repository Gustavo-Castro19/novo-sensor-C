#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define INITIAL_SENSOR_CAPACITY (8)
#define MAX_SENS_READS (8000)
#define MAX_LINE (1000)
#define MAX_ID_LEN   (32)
#define MAX_VALUE_LEN (32)

typedef struct {
  time_t timestamp;
  char sens_id[MAX_ID_LEN];
  char value[MAX_VALUE_LEN];
} sensor_t;

typedef struct {
  char **names;
  int count;
  int capacity;
} sensor_names_t;

int get_sensors(FILE *fp, sensor_t *readings, sensor_names_t *archives);
void order_timestamp(sensor_t *sensor, int size);
void record_archives(sensor_names_t *archives, sensor_t *sensor_reads, int size);
void add_unique_sensor(char *sensor_id, sensor_names_t *archives);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "erro: parametros passados incorretamente\n usagem: %s <NOME_DO_ARQUIVO>\n", argv[0]);
    return 1;
  }

  FILE *fd = fopen(argv[1], "r");
  if (!fd) {
    fprintf(stderr, "ERRO: nao foi possivel ler o texto do arquivo\n");
    return 1;
  }

  sensor_t sensors_list[MAX_SENS_READS];
  sensor_names_t sensor_names = {
    .names = malloc(INITIAL_SENSOR_CAPACITY * sizeof(char *)),
    .count = 0,
    .capacity = INITIAL_SENSOR_CAPACITY
  };

  if (!sensor_names.names) {
    fprintf(stderr, "erro ao alocar memoria");
    fclose(fd);
    return -1;
  }

  int sensor_count = get_sensors(fd, sensors_list, &sensor_names);

  order_timestamp(sensors_list, sensor_count);
  record_archives(&sensor_names, sensors_list, sensor_count);

  for (int i = 0; i < sensor_names.count; i++) {
    free(sensor_names.names[i]);
  }
  free(sensor_names.names);
  fclose(fd);
  return 0;
}

int get_sensors(FILE *fp, sensor_t *readings, sensor_names_t *archives) {
  static char line[MAX_LINE];
  char *parser;
  int count = 0, lcount = 0;

  while (fgets(line, sizeof(line), fp)) {
    ++lcount;
    if (count >= MAX_SENS_READS) {
      fprintf(stderr, "o programa excedeu seu limite de %d, havera truncamento nos arquivos gerados\n", MAX_SENS_READS);
      break;
    }

    parser = strtok(line, " ");
    if (!parser) {
      fprintf(stderr, "a linha %d \n%s\n nao contem um timestamp valido, houve truncamento", lcount, line);
      continue;
    }
    time_t temptime = (time_t)atol(parser);

    parser = strtok(NULL, " ");
    if (!parser) {
      fprintf(stderr, "a linha %d \n%s\n nao contem um ID valido, houve truncamento\n", lcount, line);
      continue;
    }
    char tempcharID[MAX_ID_LEN];
    strncpy(tempcharID, parser, MAX_ID_LEN - 1);
    tempcharID[MAX_ID_LEN - 1] = '\0';

    parser = strtok(NULL, "\n");
    if (!parser) {
      fprintf(stderr, "a linha %d \n%s\n nao contem um valor valido\n", lcount, line);
      continue;
    }
    char tempcharVal[MAX_VALUE_LEN];
    strncpy(tempcharVal, parser, MAX_VALUE_LEN - 1);
    tempcharVal[MAX_VALUE_LEN - 1] = '\0';

    readings[count].timestamp = temptime;
    strcpy(readings[count].sens_id, tempcharID);
    strcpy(readings[count].value, tempcharVal);
    add_unique_sensor(readings[count].sens_id, archives);
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

void add_unique_sensor(char *sensor_id, sensor_names_t *archives) {
  for (int i = 0; i < archives->count; i++) {
    if (strcmp(archives->names[i], sensor_id) == 0) return;
  }

  if (archives->count >= archives->capacity) {
    archives->capacity *= 2;
    archives->names = realloc(archives->names, archives->capacity * sizeof(char *));
    if (!archives->names) {
      perror("Erro ao realocar memoria para lista de sensores");
      exit(EXIT_FAILURE);
    }
  }

  archives->names[archives->count++] = strdup(sensor_id);
}

void record_archives(sensor_names_t *archives, sensor_t *sensor_reads, int size) {
  for (int i = 0; i < archives->count; i++) {
    if (!archives->names[i]) continue;
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.txt", archives->names[i]);
    FILE *new_fd = fopen(filename, "w");
    if (!new_fd) {
      perror("nao foi possivel abrir o arquivo gerado");
      continue;
    }
    for (int z = 0; z < size; z++) {
      if (strcmp(sensor_reads[z].sens_id, archives->names[i]) == 0) {
        fprintf(new_fd, "%ld %s %s\n", sensor_reads[z].timestamp, sensor_reads[z].sens_id, sensor_reads[z].value);
      }
    }
    fclose(new_fd);
  }
}
