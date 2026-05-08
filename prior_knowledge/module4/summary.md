# Resumen — Módulo 4: Arquitectura de Computadores
## Preparación Embedded Software Engineer — Aeronautical SW / Joby Aviation
*Generado al cierre del Módulo 4 — Mayo 2026*

---

## Glosario de Acrónimos y Nomenclatura

| Acrónimo | Significado | Contexto |
|----------|-------------|---------|
| ISA | Instruction Set Architecture | Contrato SW/HW: qué instrucciones entiende el procesador |
| RISC | Reduced Instruction Set Computer | ARM, MIPS, RISC-V — instrucciones simples de ancho fijo |
| CISC | Complex Instruction Set Computer | x86/x64 — instrucciones complejas de longitud variable |
| PC | Program Counter | R15 — dirección de la próxima instrucción |
| SP | Stack Pointer | R13 — tope del stack actual |
| LR | Link Register | R14 — dirección de retorno de función |
| MSP | Main Stack Pointer | Stack del OS/interrupciones en Cortex-M |
| PSP | Process Stack Pointer | Stack de tareas de RTOS |
| xPSR | Program Status Register | N, Z, C, V flags + modo + ISR activo |
| N | Negative flag | Resultado fue negativo (aritmética con signo) |
| Z | Zero flag | Resultado fue cero (condición if/while) |
| C | Carry flag | Carry-out / overflow sin signo |
| V | oVerflow flag | Overflow con signo (signed overflow) |
| AAPCS | ARM Architecture Procedure Call Standard | Convención de llamada: R0-R3 args/return, R4-R11 callee-saved |
| IF | Instruction Fetch | Etapa 1 del pipeline |
| ID | Instruction Decode | Etapa 2 del pipeline |
| EX | Execute | Etapa 3 del pipeline (ALU) |
| MEM | Memory access | Etapa 4 del pipeline |
| WB | Write Back | Etapa 5 del pipeline |
| RAW | Read After Write | Hazard de datos más común: I2 lee lo que I1 escribió |
| WAR | Write After Read | Hazard de datos raro (anti-dependence) |
| WAW | Write After Write | Hazard de datos raro (output dependence) |
| OOO | Out-of-Order execution | CPU reordena instrucciones para evitar stalls |
| LRU | Least Recently Used | Política de reemplazo de cache |
| MESI | Modified/Exclusive/Shared/Invalid | Protocolo de coherencia de cache en multicore |
| MPU | Memory Protection Unit | Unidad de protección de memoria para DO-178C |
| SMP | Symmetric MultiProcessing | Múltiples cores idénticos compartiendo memoria |
| NMI | Non-Maskable Interrupt | Interrupción que no puede ser deshabilitada |
| SVC | Supervisor Call | Instrucción para escalar a modo privilegiado |
| PendSV | Pendable Service | Excepción para context switch en RTOS |
| SysTick | System Tick Timer | Timer del sistema (1 ms en FreeRTOS típico) |
| AHB | Advanced High-performance Bus | Bus principal del Cortex-M (alta velocidad) |
| APB | Advanced Peripheral Bus | Bus de periféricos (más lento, APB1/APB2) |
| DMA | Direct Memory Access | Transferencia de datos sin CPU |
| EXC_RETURN | Exception Return | Valor especial en LR que codifica cómo retornar de una ISR |
| LDREX/STREX | Load/Store Exclusive | Instrucciones para operaciones atómicas sin deshabilitar IRQs |
| ART | Adaptive Real-Time accelerator | Buffer de prefetch de instrucciones en STM32 (actúa como I-cache) |

---

## 4.1 Modelo Von Neumann vs Harvard

### Von Neumann (Princeton)
- Instrucciones y datos comparten **el mismo bus y la misma memoria**
- El procesador alterna entre fetch de instrucción y acceso a datos → **Von Neumann bottleneck**
- Solo puede hacer una cosa a la vez en el bus
- Usado en: x86 PCs, sistemas de propósito general

### Harvard
- Instrucciones y datos tienen **memorias y buses separados**
- El procesador puede buscar instrucción y leer dato **simultáneamente**
- Ventaja: doble ancho de banda efectivo
- Desventaja: más hardware, no podés cargar código desde la memoria de datos
- Usado en: DSPs, AVR, PIC

### Modified Harvard — ARM Cortex-M ⭐
- **Buses internos separados** para instrucciones y datos
- **Pero el mismo espacio de direcciones** de 4 GB (como Von Neumann)
- Lo mejor de ambos mundos: flexibilidad + rendimiento paralelo
- Buses en Cortex-M4:
  - **I-Code bus**: instrucciones desde Flash
  - **D-Code bus**: datos/constantes desde Flash
  - **System bus**: SRAM, periféricos, resto del mapa

$Nota: El DMA puede transferir datos por el System bus mientras el core ejecuta instrucciones por el I-Code bus simultáneamente, sin que ninguno espere al otro.

---

## 4.2 Conjunto de Instrucciones (ISA)

### RISC vs CISC

| Característica | RISC (ARM) | CISC (x86) |
|---------------|-----------|-----------|
| Instrucciones | Pocas, simples | Muchas, complejas |
| Ancho | Fijo (32 bits) | Variable (1–15 bytes) |
| Responsable | Compilador | Hardware |
| Pipeline | Fácil | Complejo |
| Ejemplo | ARM, MIPS, RISC-V | Intel x86, AMD64 |

### Thumb-2 (lo que ejecuta el Cortex-M4)
- Mezcla de instrucciones de 16 y 32 bits
- El Cortex-M4 **solo ejecuta Thumb-2** (sin estado ARM de 32 bits)
- El bit 0 del PC debe ser siempre 1 (señala modo Thumb)

### Registros del ARM Cortex-M4

```
R0–R3    Argumentos de función / valores de retorno / scratch (caller-saved)
R4–R7    Variables locales persistentes (callee-saved, low regs)
R8–R11   Variables locales persistentes (callee-saved, high regs)
R12      Scratch intra-call
R13/SP   Stack Pointer (MSP o PSP según CONTROL[0])
R14/LR   Link Register — retorno de función / EXC_RETURN en ISR
R15/PC   Program Counter — próxima instrucción
xPSR     N, Z, C, V flags + bit T (Thumb) + ISR activo
MSP      Main Stack Pointer — OS/IRQ
PSP      Process Stack Pointer — tareas RTOS
```

**AAPCS (ARM calling convention):**
- R0–R3: se pasan como argumentos (hasta 4). Retorno en R0 (o R0:R1 para 64-bit)
- R4–R11: callee-saved → si una función los usa, debe guardar/restaurar con push/pop
- R12, LR, PC: caller-saved (se destruyen en BL)

### Modos de Direccionamiento

| Modo | Sintaxis | Ejemplo |
|------|---------|---------|
| Inmediato | `#valor` | `MOV R0, #42` |
| Registro | `Rx` | `ADD R2, R0, R1` |
| Base + Offset | `[Rx, #n]` | `LDR R0, [R1, #8]` — acceso a struct field |
| Base + Registro | `[Rx, Ry]` | `LDR R0, [R1, R2]` |
| Base + Reg LSL | `[Rx, Ry, LSL #n]` | `LDR R0, [R1, R2, LSL #2]` — `array[i]` |
| Pre-indexed | `[Rx, #n]!` | `LDR R0, [R1, #4]!` — R1 += 4 primero |
| Post-indexed | `[Rx], #n` | `LDR R0, [R1], #4` — R1 += 4 después |
| PC-relative | `=valor` | `LDR R0, =0x40020000` — literal pool |

### Instrucciones Clave

```asm
; Datos
MOV R0, #42          ; inmediato
LDR R0, [R1]         ; load de memoria
STR R0, [R1]         ; store a memoria
LDM R1, {R2-R5}      ; load multiple (pop)
STM R1, {R2-R5}      ; store multiple (push)

; Aritmética
ADD, SUB, MUL, UDIV  ; operaciones básicas
LSL #n               ; shift left (×2ⁿ)
LSR #n               ; shift right lógico (÷2ⁿ, rellena 0)
ASR #n               ; shift right aritmético (÷2ⁿ, preserva signo)

; Lógica (bitwise)
AND  ; bit clear selectivo
ORR  ; bit set selectivo
EOR  ; XOR — toggle bits, detección de cambio
BIC  ; Bit Clear = AND NOT (BIC R0, R0, #(1<<3) → clr bit 3)

; Comparación (actualiza flags, no guarda resultado)
CMP R0, R1           ; R0 - R1 → N, Z, C, V
TST R0, #(1<<5)      ; R0 AND mask → Z=1 si bit 5 está en 0

; Saltos
B label              ; branch incondicional
BL function          ; LR = PC+4, luego salta (para llamar funciones)
BX LR                ; return (salta a LR)
BEQ, BNE, BLT, BGT  ; condicionales por flags
```

---

## 4.3 Pipeline

### Pipeline de 5 Etapas Clásico (MIPS/concepto base)

```
Ciclo:    1    2    3    4    5    6    7
I1:      IF → ID → EX → MEM → WB
I2:           IF → ID → EX → MEM → WB
I3:                IF → ID → EX → MEM → WB
```

- Sin pipeline: CPI = 5 ciclos/instrucción
- Con pipeline: CPI ≈ 1 ciclo/instrucción (en steady state)
- Speedup teórico = número de etapas

### Pipeline del Cortex-M3/M4: 3 Etapas

1. **Fetch** — lee instrucción de Flash/cache
2. **Decode** — decodifica + lee registros
3. **Execute** — ALU + memoria + write-back (todo junto)

⚠ PC durante ejecución apunta **2 instrucciones adelante** (herencia de 3 etapas):
- Para instrucción Thumb de 16 bits: PC = dirección_actual + 4
- Para instrucción de 32 bits: PC = dirección_actual + 4

### Los 3 Tipos de Hazards

#### 1. Data Hazard (RAW — Read After Write) ⭐ El más común
```asm
ADD R2, R0, R1    ; R2 producido en EX
SUB R4, R2, R3    ; R2 necesario en ID → stall sin forwarding
```
- Sin forwarding: 2 stall cycles
- Con forwarding EX→EX: 0 stall cycles
- **Load-use hazard** (LDR seguido de uso): 1 stall cycle mínimo (inevitable)

**Solución:** forwarding (bypassing) en hardware, reordenamiento de instrucciones por el compilador, insertar instrucciones independientes entre dependientes.

#### 2. Control Hazard (Branch Hazard)
- El branch cambia el PC → el pipeline ya buscó instrucciones equivocadas
- Penalty: número de etapas entre fetch y resolución del branch
- En Cortex-M3/M4 (3 etapas): penalty ≈ 1–3 ciclos

**Solución:** branch prediction, branch delay slot, loop unrolling, condicionales Thumb (IT block).

#### 3. Structural Hazard
- Dos instrucciones necesitan el mismo recurso hardware simultáneamente
- Ejemplo clásico: una sola memoria compartida (Von Neumann) → fetch de instrucción colisiona con load de datos

**Solución:** caches separadas I/D (arquitectura Harvard), más unidades funcionales, stalls.

### Forwarding (Data Bypassing)
- Pasa el resultado de la ALU **directamente** a la entrada de la siguiente ALU, sin pasar por el banco de registros
- Reduce stalls de 2 ciclos a 0 en el caso RAW más común
- El compilador ARM reordena instrucciones para minimizar dependencias

---

## 4.4 Jerarquía de Memoria y Caché

### La Brecha de Velocidad

```
Registros       0 ciclos    (flip-flops, 16 × 32b en Cortex-M)
L1 cache     1–4 ciclos    (SRAM, 4–64 KB)
L2 cache    10–20 ciclos   (SRAM, 256 KB – 4 MB)
DRAM        60–100 ns      (cientos de ciclos a 100 MHz+)
Flash (MCU) 60–100 ns      (datos const leídos directo)
SSD         ~100 µs        (acceso a archivos)
```

**En Cortex-M4 (STM32F4):** ART Accelerator = prefetch buffer para instrucciones. Sin cache de datos → SRAM accessible en 0 ciclos extras de wait state.
**En Cortex-M7 (STM32H7):** L1 cache de datos e instrucciones (típicamente 16–32 KB cada una).

### Organización de la Caché

**Campos de la dirección:**
```
Dirección 32 bits: [ Tag | Index | Offset ]
                     bits altos  bits bajos
```
- **Offset** = log₂(tamaño_línea) bits → selecciona byte dentro de la línea
- **Index** = log₂(número_sets) bits → selecciona el set/fila en la cache
- **Tag** = bits restantes → guardado en la cache para verificar que es el bloque correcto

**Ejemplo de cálculo (pregunta de entrevista frecuente):**
- Cache: 4 KB, líneas de 32 bytes, direct-mapped
- Offset: log₂(32) = **5 bits** → bits [4:0]
- Sets: 4096/32 = 128 → Index: log₂(128) = **7 bits** → bits [11:5]
- Tag: 32 - 7 - 5 = **20 bits** → bits [31:12]

### Direct-Mapped vs Set-Associative

| Característica | Direct-mapped (1-way) | N-way set-associative |
|---------------|----------------------|-----------------------|
| Entradas por set | 1 | N |
| Conflictos | Muchos (thrashing) | N-1 más flexibles |
| Hardware | Más simple | Más complejo |
| Hit time | Más rápido | Algo más lento |
| Uso típico | Instruction cache pequeña | L1/L2 datos |

**Thrashing:** Dos arrays cuya diferencia de dirección es múltiplo exacto del tamaño de cache → mapean a la misma línea → se expulsan mutuamente → miss rate ≈ 100%.

### Políticas de Reemplazo
- **LRU** (Least Recently Used): expulsa la menos usada. Óptimo práctico, costoso.
- **Pseudo-LRU**: aproximación de LRU. Lo que usan los CPUs reales.
- **FIFO**: expulsa la más vieja. Simple.
- **Random**: sorprendentemente bueno, muy simple.

### Write Policies

| Política | En hit | En miss | Característica |
|---------|--------|---------|---------------|
| Write-through | Escribe cache Y memoria | No trae línea | Simple, siempre coherente |
| Write-back | Escribe solo cache, dirty bit | Trae línea | Más rápido, pero cache ≠ memoria |
| Write-allocate | (en miss) trae línea, luego escribe | — | Combina con write-back |
| No-write-allocate | Escribe directo a memoria | — | Combina con write-through |

⚠ **Trampa crítica en embedded:** Write-back cache + DMA = inconsistencia potencial.
- CPU escribe en buffer → dato en cache (no en memoria todavía)
- DMA lee de memoria → lee dato **viejo**
- **Solución:** flush cache antes de iniciar DMA (`SCB_CleanDCache_by_Addr()` en Cortex-M7)

### Localidad de Referencia
- **Temporal:** si accediste a X, probablemente lo accedés pronto de nuevo (variables de loop)
- **Espacial:** si accediste a X, probablemente accedés a X±n pronto (arrays, código secuencial)
- Arrays row-major (C) > column-major para cache hits

### Miss Types (3 Cs)
1. **Compulsory (cold miss):** primera vez que se accede a esa línea — inevitable
2. **Capacity miss:** el working set es mayor que el tamaño de la cache
3. **Conflict miss:** dos direcciones mapean al mismo set — evitable con más vías

---

## 4.5 Modos de Privilegio y Excepciones

### Privilege Levels en Cortex-M

```
┌──────────────────────────────────────────────┐
│ Handler Mode (ISRs, faults)                  │  ← SIEMPRE privilegiado
│ Usa MSP                                      │
└──────────────────────┬───────────────────────┘
                        │ IRQ/fault ↕ EXC_RETURN
┌──────────────────────┴───────────────────────┐
│ Thread Mode                                   │
│  ├─ Privilegiado (CONTROL[0]=0) — OS kernel  │
│  └─ Unprivileged (CONTROL[0]=1) — RTOS task  │
│       Solo puede escalar via SVC              │
└──────────────────────────────────────────────┘
```

- MSP: Handler mode + privileged thread → OS, IRQs
- PSP: Unprivileged thread → cada tarea RTOS tiene su propio PSP

### Vector Table (empieza en 0x00000000 en Flash)

```
Offset  Entrada
0x0000  Valor inicial del MSP (top of SRAM)
0x0004  Reset_Handler  ← dirección de main() (entry point)
0x0008  NMI_Handler
0x000C  HardFault_Handler
0x0010  MemManage_Handler    ← violación de MPU
0x0014  BusFault_Handler
0x0018  UsageFault_Handler   ← instrucción inválida
0x002C  SVC_Handler          ← escalada de privilegio
0x0038  PendSV_Handler       ← context switch en RTOS
0x003C  SysTick_Handler      ← 1 ms tick en FreeRTOS
0x0040+ IRQ0–IRQ239          ← periféricos (UART, SPI, CAN, ADC...)
```

### EXC_RETURN
Cuando el hardware entra a una ISR, LR = valor especial:
- `0xFFFFFFF9` → retornar a Thread mode, MSP, sin FPU
- `0xFFFFFFFD` → retornar a Thread mode, PSP, sin FPU
- `0xFFFFFFE9/ED` → versiones con FPU state

### MPU — Memory Protection Unit
- Define hasta 8 regiones con permisos de acceso (R/W/X) y nivel (privilegiado/unprivileged)
- Violación → **MemManage Fault** → OS puede matar la tarea sin afectar el sistema
- **Obligatoria para DO-178C** (partición espacial, fault containment)

### Boot Sequence del Cortex-M
1. Hardware lee `[0x00000000]` → carga MSP
2. Hardware lee `[0x00000004]` → salta a Reset_Handler
3. Reset_Handler:
   - Copia `.data` de Flash a SRAM
   - Rellena `.bss` con ceros
   - Llama `SystemInit()` (PLL, clocks)
   - Llama `main()`

---

## 4.6 DMA y Buses del Sistema

### Jerarquía de Buses en STM32 (Cortex-M4)

```
Core → AHB Bus Matrix (≤ SYSCLK = 168 MHz)
            ├── Flash (I-Code, D-Code)
            ├── SRAM1/2
            ├── DMA1/2
            ├── APB2 Bridge (≤ 84 MHz)
            │     └── TIM1/8, ADC1-3, SPI1, USART1/6
            └── APB1 Bridge (≤ 42 MHz)
                  └── TIM2-7, SPI2/3, I2C, CAN, USART2-5
```

⚠ **Trampa del timer clock:** Si APB1 prescaler > 1, el clock del timer = APB1 × 2.
- APB1 = SYSCLK/4 = 42 MHz → timer clock = 84 MHz (no 42 MHz)

### Memory-Mapped I/O en ARM
- **No hay instrucciones especiales de I/O** (a diferencia de x86 con IN/OUT)
- Todos los periféricos están mapeados al espacio de direcciones de 4 GB
- Acceso igual que una variable, pero con `volatile`:

```c
#define GPIOA_ODR   (*((volatile uint32_t *)0x40020014))
GPIOA_ODR |= (1 << 5);   // enciende LED — igual que acceder a memoria
```

### DMA vs Core en el Bus
- DMA y core compiten por el AHB matrix
- Prioridad DMA configurable: Very High, High, Medium, Low
- DMA intensivo puede causar **bus stalling** → el core se frena aunque no haga nada "pesado"

---

## 4.7 Sistemas Multiprocesador (Intro)

### SMP y Coherencia de Cache
- Múltiples cores con sus propias L1 caches → pueden tener copias diferentes de la misma variable
- **MESI protocol:** mantiene coherencia via mensajes entre cores
  - **M**odified: solo en este core, diferente a memoria
  - **E**xclusive: solo en este core, igual a memoria
  - **S**hared: puede estar en varios cores, igual a memoria
  - **I**nvalid: no válida
- Cuando un core escribe en una línea Shared → manda invalidación → otros cores tienen I → próxima lectura es miss

### Operaciones Atómicas en Cortex-M
```c
// LDREX/STREX: operación atómica sin deshabilitar IRQs
uint32_t old_val, new_val;
do {
    old_val = __LDREXW(&counter);    // Load exclusive
    new_val = old_val + 1;
} while (__STREXW(new_val, &counter));  // falla si alguien escribió → reintentar

// C11/C++11: abstracción portable
#include <stdatomic.h>
atomic_uint32_t counter = ATOMIC_VAR_INIT(0);
atomic_fetch_add(&counter, 1);
```

---

## Misunderstandings Comunes ⚠

| Error | Corrección |
|-------|-----------|
| "El PC apunta a la instrucción actual" | En Cortex-M3/M4 (3 etapas), PC = dirección_actual + 4 durante ejecución |
| "El bit 0 del PC no importa" | El bit 0 DEBE ser 1 en Cortex-M (señala modo Thumb). Un branch a dirección par → HardFault |
| "Write-back cache es siempre más rápido y es preferible" | En embedded con DMA, write-back causa incoherencia. Hay que flush antes de DMA. |
| "Direct-mapped es mejor porque es más rápido" | El thrashing puede hacerla inútil. Set-associative balances mejor. |
| "El Cortex-M4 tiene 5 etapas de pipeline" | Cortex-M3/M4 tiene 3 etapas. El de 5 etapas es el concepto clásico (MIPS). |
| "RISC siempre gana a CISC en velocidad" | Los chips x86 modernos convierten CISC en micro-ops RISC internamente. El ISA externo no determina el rendimiento. |
| "LDR no puede causar hazards" | Load-use hazard: LDR seguido de uso inmediato del registro → 1 stall cycle inevitable (el dato viene de MEM, no de EX). |
| "El Cortex-M4 tiene cache de datos" | El STM32F4 (Cortex-M4) NO tiene L1 data cache. Solo el ART Accelerator (instruction buffer). El STM32H7 (Cortex-M7) sí tiene. |
| "R13 siempre apunta al mismo stack" | R13/SP puede ser MSP o PSP según CONTROL[0]. En RTOS, cada tarea tiene su PSP. Las ISRs usan MSP. |
| "Cache miss siempre es malo" | Cold miss es inevitable (primera vez). Solo capacity y conflict miss son "evitables" con mejor diseño. |

---

## Fórmulas y Cálculos Clave

### Cálculo de campos de dirección en cache
```
Offset bits   = log₂(bytes por línea)
Index bits    = log₂(número de sets)    = log₂(capacidad / (vías × bytes_línea))
Tag bits      = 32 - Index_bits - Offset_bits
```

### Tamaño de la cache
```
Capacidad = número_sets × vías × tamaño_línea
```

### Performance del pipeline
```
CPI_ideal = 1  (sin stalls)
CPI_real  = 1 + stalls_por_instrucción
Speedup pipeline = CPI_sin_pipeline / CPI_con_pipeline
```

### Latencia de instrucciones ARM
```
Instrucción simple (ADD, AND): 1 ciclo
LDR: 2 ciclos (1 extra para acceso a memoria)
MUL: 1–3 ciclos según operandos
LDM de N registros: N+1 ciclos
Branch tomado (Cortex-M4): ~3-4 ciclos de penalty
```

---

## Relevancia para el Puesto (Aeronautical SW / Joby)

1. **Modified Harvard** → entiende por qué el DMA puede transferir mientras el core ejecuta
2. **ARM Thumb-2 + AAPCS** → lees y escribís código ensamblador al depurar con GDB/JTAG
3. **Pipeline hazards** → el compilador los evita, pero si optimizás a mano o escribís ASM crítico, necesitás saberlos
4. **Cache + DMA coherency** → bug sutil con STM32H7 que solo aparece en hardware, no en simulación
5. **Privilege modes + MPU** → DO-178C requiere partición espacial; usarás MPU en el RTOS de producción
6. **Vector table** → al depurar un HardFault, el vector table te dice qué handler saltó
7. **Boot sequence** → el linker script y el startup code son los primeros archivos que editan los embedded engineers en un proyecto nuevo

---

## Referencias Bibliográficas

| Tema | Referencia |
|------|-----------|
| ISA y ARM | *ARM Cortex-M4 Technical Reference Manual* — ARM Ltd (gratuito, arm.com) |
| ISA y ARM | *The Definitive Guide to Arm Cortex-M3 and Cortex-M4 Processors* — Joseph Yiu |
| Pipeline | *Computer Organization and Design (ARM Edition)* — Patterson & Hennessy |
| Caché | *Computer Architecture: A Quantitative Approach* — Hennessy & Patterson (el libro de referencia) |
| Embedded C y periféricos | *Embedded Systems with ARM Cortex-M Microcontrollers* — Dogan Ibrahim |
| Arquitectura detallada | *Modern Processor Design* — Shen & Lipasti |
| RTOS y modos de privilegio | *Mastering the FreeRTOS Real Time Kernel* — Richard Barry (gratuito, FreeRTOS.org) |
| DO-178C y MPU | *DO-178C Software Considerations in Airborne Systems* — RTCA/EUROCAE |
