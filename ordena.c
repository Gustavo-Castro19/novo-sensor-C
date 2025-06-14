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

int get_sensors(FILE *fp, sensor_t *catching, char ***archive_names, int *arc_count, int *capacity);
void order_timestamp(sensor_t *sensor, int size);
void catch_pos_file(char *pars, char ***pos_arc, int *arc_count, int *capacity);
void record_archives(char **filenames, sensor_t *sensor_reads, int arc_count, int size);

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
  int sensor_capacity = INITIAL_SENSOR_CAPACITY;
  char **sensor_names = malloc(sensor_capacity * sizeof(char *));
  int sensor_count = 0;
  int arc_count = 0;

  sensor_count = get_sensors(fd, sensors_list, &sensor_names, &arc_count, &sensor_capacity);

  order_timestamp(sensors_list, sensor_count);
  record_archives(sensor_names, sensors_list, arc_count, sensor_count);

  for (int i = 0; i < arc_count; i++) {
    free(sensor_names[i]);
  }
  free(sensor_names);
  fclose(fd);
  return 0;
}

int get_sensors(FILE *fp, sensor_t *catching, char ***archive_names, int *arc_count, int *capacity) {
  static char line[MAX_LINE];
  char *parser;
  int count = 0;

  while (fgets(line, sizeof(line), fp)) {
    if (count >= MAX_SENS_READS) break;

    parser = strtok(line, " ");
    if (!parser) continue;
    catching[count].timestamp = (time_t)atol(parser);

    parser = strtok(NULL, " ");
    if (!parser) continue;
    strncpy(catching[count].sens_id, parser, MAX_ID_LEN - 1);
    catching[count].sens_id[MAX_ID_LEN - 1] = '\0';
    if (strlen(parser) > MAX_ID_LEN) {
      fprintf(stderr, "erro: alguns valores de ID excedem o limite do sistema havera truncamento no arquivo final\n");
      continue;
    }
    catch_pos_file(parser, archive_names, arc_count, capacity);

    parser = strtok(NULL, "\n");
    if (!parser) continue;
    strncpy(catching[count].value, parser, MAX_VALUE_LEN - 1);
    catching[count].value[MAX_VALUE_LEN - 1] = '\0';
    if (strlen(parser) > MAX_VALUE_LEN) {
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

void catch_pos_file(char *pars, char ***pos_arc, int *arc_count, int *capacity) {
  for (int i = 0; i < *arc_count; i++) {
    if (strcmp((*pos_arc)[i], pars) == 0) return;
  }
  if (*arc_count >= *capacity) {
    *capacity *= 2;
    *pos_arc = realloc(*pos_arc, (*capacity) * sizeof(char *));
    if (!*pos_arc) {
      fprintf(stderr, "Erro ao realocar memoria para lista de sensores\n");
      exit(1);
    }
  }
  (*pos_arc)[*arc_count] = strdup(pars);
  (*arc_count)++;
}

void record_archives(char **filenames, sensor_t *sensor_reads, int arc_count, int size) {
  for (int i = 0; i < arc_count; i++) {
    if (filenames[i] == NULL) continue;
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
