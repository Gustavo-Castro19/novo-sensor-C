# novo-sensor-C

## Compilação

Este programa foi compilado usando o GCC, mais especificamente:  
`gcc (GCC) 15.1.1 20250521 (Red Hat 15.1.1-2)`  
Clang, GCC e MINGW64 funcionam corretamente.

Para compilar com o GCC, utilize:

```bash
gcc ordena.c
gcc busca.c
gcc gera_teste.c
```

Não há necessidade de flags adicionais de compilação.

---

## Resumo da Aplicação

Este programa visa trabalhar com sensores que carregam leituras dos tipos: `int`, `double`, `bool` e `string`, a partir de arquivos no formato:

```
<TIMESTAMP> <ID_SENSOR> <VALOR>
```

### 1. Ordenação de Leituras

O primeiro programa recebe como argumento um arquivo com as leituras, ordena cada leitura por timestamp, identifica os nomes dos sensores (ID_SENSOR) e cria arquivos separados para cada sensor.

**Execução:**
```bash
./<arquivo_compilado_de_ordena.c> <lista_de_sensores>
```

**Exemplo:**  
Se houver 20 leituras com o ID igual a `temp`, será gerado um arquivo `temp.txt` com as leituras de `temp` em ordem crescente de timestamp.

---

### 2. Busca por Data e Hora

O segundo programa recebe como argumentos uma lista de leituras e uma data/hora no formato `DD MM YYYY HH MM SS`. Ele faz uma busca binária pelos timestamps e retorna o valor exato ou o mais próximo da data fornecida.

**Execução:**
```bash
./<arquivo_compilado_de_busca.c> <lista_de_sensores> DD MM YYYY HH MM SS
```

---

### 3. Geração de Listas de Sensores

O terceiro programa gera listas de sensores no formato esperado pelos programas 1 e 2. Os argumentos esperados são: data/hora inicial (`DD MM YYYY HH MM SS`), data/hora final (mesmo formato), e pares `<SENSOR> <TIPO_DE_DADO>`.

Ao final da execução, espera-se que seja gerado um arquivo chamado `teste.txt`, ou, em caso de erro de verificação, nenhum arquivo será criado.

**Execução:**
```bash
./<arquivo_compilado_de_gera_teste.c> DD MM YYYY HH MM SS DD MM YYYY HH MM SS <SENSOR1> <TIPO1> ...
```
Uma execução mínima pode conter apenas um sensor e tipo, mas é possível adicionar mais.

