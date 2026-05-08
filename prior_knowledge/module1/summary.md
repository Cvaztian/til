# Módulo 1: Fundamentos de Circuitos Lógicos — Resumen Completo

> **Contexto:** Preparación para Embedded Software Engineer — Aeronautical SW / Joby Aviation
> **Última actualización:** Módulo 1 completo con aclaraciones

---

## 1.1 Sistemas Numéricos y Representación de Datos

### Bases Numéricas
- **Binario (base 2):** Lenguaje del hardware. 1 bit = voltaje alto/bajo. 8 bits = 1 byte.
- **Hexadecimal (base 16):** Representación humana de binario. 1 dígito hex = 4 bits (nibble). `0x1F` = `0001 1111`.
- **BCD (Binary Coded Decimal):** Cada dígito decimal se codifica en 4 bits individualmente. Ejemplo: 47 → `0100 0111` (BCD) vs `00101111` (binario puro). Uso: displays de 7 segmentos, relojes.

### Conversiones
- **Bin→Hex:** agrupar de derecha a izquierda en nibbles. `1101 0011` → `0xD3`.
- **Hex→Bin:** expandir cada dígito a 4 bits. `0xA7` → `1010 0111`.
- **Bin→Dec:** sumar potencias de 2 donde hay 1. `1101 0011` = 128+64+16+2+1 = 211.
- **Dec→Bin:** divisiones sucesivas por 2 o restar potencias de 2 descendentes.

### Aritmética Binaria
- **Suma:** carry en 1+1=10.
- **Resta:** se realiza sumando el complemento a 2 (el hardware usa el mismo sumador).
- **Shift izquierdo (`<< n`):** multiplica por 2^n. `x << 3` = `x * 8`, más rápido en el procesador.
- **Shift derecho (`>> n`):** divide por 2^n. Para signed, el comportamiento (aritmético vs lógico) depende de la implementación. *Pregunta frecuente de entrevista.*

### Complemento a 2
- **Representación de negativos en todo procesador moderno.**
- **Obtención:** invertir bits (complemento a 1) + sumar 1. Ejemplo: +5 = `0000 0101` → invertir = `1111 1010` → +1 = `1111 1011` = −5.
- **Ventaja:** mismo circuito sumador para suma y resta.
- **MSB = bit de signo** (0 = positivo, 1 = negativo), pero NO es signo-magnitud.
- **Rango con N bits:** −2^(N−1) a +2^(N−1) − 1.
  - `int8_t`: −128 a 127
  - `int16_t`: −32768 a 32767
  - `int32_t`: −2,147,483,648 a 2,147,483,647
- **Overflow:** sumar dos positivos → negativo, o dos negativos → positivo. El procesador levanta overflow flag.
- **En C:** overflow de signed es **undefined behavior**. Para unsigned, wrap-around está definido (aritmética módulo 2^N).

### Wrap-around
- Analogía: odómetro de 3 dígitos (999 + 1 = 000).
- `uint8_t`: 255 + 1 = 0 (definido por estándar C).
- `int8_t`: 127 + 1 = −128 en la práctica, pero es **undefined behavior** — no confiar en ello.
- MISRA C obliga a verificar antes de cada operación que no haya overflow.

### IEEE 754 (Punto Flotante)
- **32 bits:** [1 signo][8 exponente][23 mantisa]. Valor = (−1)^S × 1.M × 2^(E−127).
- **Relevancia embedded:** sin FPU, las operaciones float son mucho más lentas que enteros.
- **Nunca comparar floats con `==`.** Usar `fabs(a - b) < epsilon`. MISRA C lo prohíbe.

### Aritmética de Punto Fijo
- **Alternativa a float** en sistemas sin FPU o que requieren determinismo.
- Elegís cuántos bits son enteros y cuántos fraccionarios (ej: Q8.8 = 8 enteros + 8 fraccionarios).
- Almacenás como entero: 3.75 en Q8.8 → 3.75 × 256 = 960. Operaciones con enteros (rápido).
- **Cuidado con multiplicación:** dos Q8.8 multiplicados dan Q16.16 — hay que hacer shift derecho para volver a Q8.8.
- **Uso en aviónica:** sistemas de control de vuelo usan punto fijo por velocidad y determinismo.

### Tipos de datos en embedded
- Siempre usar `<stdint.h>`: `uint8_t`, `int32_t`, etc.
- Nunca usar `int`, `short` — su tamaño varía entre plataformas. En aviónica, esta ambigüedad es inaceptable.

---

## 1.2 Álgebra Booleana y Simplificación

### Operaciones Fundamentales
| Operación | Símbolo | C (bitwise) | Descripción |
|-----------|---------|-------------|-------------|
| AND       | ·       | `a & b`     | 1 solo si todas las entradas son 1 |
| OR        | +       | `a \| b`    | 1 si alguna entrada es 1 |
| NOT       | '       | `~a` / `!a` | Invierte |
| NAND      | (·)'    | `~(a & b)`  | NOT de AND — funcionalmente completa |
| NOR       | (+)'    | `~(a \| b)` | NOT de OR — funcionalmente completa |
| XOR       | ⊕       | `a ^ b`     | 1 si entradas son diferentes |
| XNOR      | (⊕)'    | `~(a ^ b)`  | 1 si entradas son iguales (comparador) |

### NAND como Compuerta Universal
Cualquier función lógica se puede construir solo con NANDs:
- **NOT:** `NAND(A, A) = A'` (2 transistores de la compuerta)
- **AND:** `NAND(NAND(A,B), NAND(A,B))` = dos NANDs
- **OR:** `NAND(NAND(A,A), NAND(B,B))` = tres NANDs (DeMorgan: A+B = (A'·B')')

**¿Por qué NAND y no AND?** En CMOS, NAND = 4 transistores, AND = 6 (NAND + inversor). Con millones de compuertas, la diferencia es enorme. Las herramientas de síntesis convierten todo a redes NAND.

NOR también es universal, pero NAND es más rápido en CMOS (NMOS en serie en pull-down es más eficiente que PMOS en serie en pull-up).

### Teoremas Clave
- **Identidad:** A+0=A, A·1=A
- **Nulo:** A+1=1, A·0=0
- **Idempotencia:** A+A=A, A·A=A
- **Complemento:** A+A'=1, A·A'=0
- **Involución:** (A')'=A
- **Absorción:** A+A·B=A, A·(A+B)=A
- **Distributiva (especial):** A+B·C = (A+B)·(A+C) ← no existe en aritmética normal
- **DeMorgan:** `(A·B)' = A'+B'` y `(A+B)' = A'·B'`
  - En C: `!(a && b)` ↔ `!a || !b`

### Formas Canónicas
- **SOP (Suma de Productos / mintérminos):** OR de ANDs. Filas donde F=1. Mapea a AND-OR / NAND-NAND.
- **POS (Producto de Sumas / maxtérminos):** AND de ORs. Filas donde F=0. Mapea a OR-AND / NOR-NOR.
- **Criterio práctico:** si la tabla tiene más 1s que 0s, POS produce menos términos (y viceversa).

### Mapas de Karnaugh
- Grilla organizada en **código Gray** (adyacentes difieren en 1 bit).
- Agrupar 1s en rectángulos de tamaño potencia de 2 (1, 2, 4, 8...).
- Grupos pueden envolver bordes (toroide).
- Grupos lo más grandes posible. Superponer está permitido.
- Variables constantes en el grupo quedan; variables que cambian se eliminan.
- **Don't cares (X):** tratar como 0 o 1 según convenga para agrandar grupos.

### Código Gray
- Codificación donde valores consecutivos difieren en exactamente 1 bit.
- **Por qué existe:** evitar glitches en transiciones (en binario, múltiples bits cambian a la vez → estados intermedios espurios).
- **Conversión Bin→Gray:** G[n]=B[n], G[i]=B[i+1] XOR B[i].
- **Usos:** K-maps (adyacencia), encoders rotativos, cruces de dominio de reloj (FIFOs asíncronas).

### Quine-McCluskey
- Método algorítmico equivalente a K-maps, pero para cualquier número de variables.
- Pasos: (1) agrupar mintérminos por # de 1s, (2) combinar adyacentes que difieren en 1 bit, (3) repetir, (4) obtener implicantes primos, (5) tabla de cobertura mínima.
- Para la prueba: saber que existe y el concepto. K-maps hasta 4 variables es lo práctico.

---

## 1.3 Compuertas Lógicas y Familias

### Transistores MOSFET — Repaso
- **NMOS:** conduce con Gate=1 (V_GS > V_th). Pull-down (Source a GND).
- **PMOS:** conduce con Gate=0 (V_GS < −|V_th|). Pull-up (Source a VDD).
- PMOS es más lento que NMOS (movilidad de huecos < electrones) → se hace 2-3x más ancho.

### Inversor CMOS
- PMOS arriba (a VDD), NMOS abajo (a GND), gates conectados.
- In=1 → NMOS conduce (Out→GND), PMOS no conduce → Out=0.
- In=0 → PMOS conduce (Out→VDD), NMOS no conduce → Out=1.
- Consumo estático ≈ 0 (nunca hay camino VDD→GND simultáneo).

### NAND CMOS
- PMOS en **paralelo** (pull-up): cualquier entrada=0 → Out=1.
- NMOS en **serie** (pull-down): ambas entradas=1 → Out=0.
- 4 transistores total (AND requeriría 6).

### CMOS vs TTL
- **CMOS:** consumo estático ~0, alta densidad, domina en todo lo moderno.
- **TTL:** transistores bipolares, mayor consumo, históricamente más rápido. Serie 74xx en legacy.

### Niveles Lógicos
- V_OH, V_OL (salida), V_IH, V_IL (entrada).
- **Noise margin:** NM_H = V_OH − V_IH, NM_L = V_IL − V_OL.
- **Problema 3.3V↔5V:** sensor 5V → micro 3.3V puede dañar el pin.

### Level Shifters y Divisores de Voltaje
- **Divisor de voltaje:** R1 + R2. V_out = V_in × R2/(R1+R2). Barato, unidireccional, lento.
- **Level shifter MOSFET:** NMOS con gate a referencia baja. Bidireccional, más rápido.
- **Chips dedicados** (TXS0108E): para buses de alta velocidad (SPI, I2C). Estándar profesional.

### Fan-in/Fan-out
- **Fan-out:** cuántas entradas puede alimentar una salida. En CMOS, limitado por capacitancia (más carga = más lento).
- **Fan-in:** cuántas entradas tiene la compuerta. Más entradas = más transistores en serie = más delay.

### Propagation Delay y Frecuencia
- **t_pd:** tiempo para que la señal atraviese una compuerta.
- **Critical path:** camino más largo → determina frecuencia máxima.
- **f = 1/T.** T=10ns → f=100MHz.

| Período | Frecuencia |
|---------|-----------|
| 1 μs    | 1 MHz     |
| 100 ns  | 10 MHz    |
| 10 ns   | 100 MHz   |
| 1 ns    | 1 GHz     |

### Otros Factores que Limitan Frecuencia
- Setup time de flip-flops (se suma al delay combinacional).
- Clock skew (el reloj no llega simultáneamente a todos los FF).
- Potencia: P_dyn = C × V² × f (crece linealmente con f).
- Integridad de señal (reflexiones, impedancia a alta frecuencia).
- EMI (más frecuencia = más radiación, crítico en aviónica por DO-160).
- Capacitancia de carga (fan-out).
- Temperatura (transistores más lentos en caliente).

---

## 1.4 Circuitos Combinacionales

### MUX (Multiplexor)
- Selector de datos: 2^N entradas, N selectores, 1 salida.
- **MUX 2:1:** Y = S'·I0 + S·I1.
- **Función universal:** MUX 2^N:1 implementa cualquier función de N variables conectando entradas a 0/1 según tabla de verdad. **Esto es exactamente lo que hace una LUT de FPGA.**
- **Truco de optimización:** función de N+1 variables con MUX 2^N:1 usando la variable extra como dato (0, 1, C, o C').
- **En embedded:** MUX de alternate function en pines del micro (GPIO / UART TX / SPI MOSI).

### Decodificador
- N bits entrada → 2^N salidas, solo una activa.
- **2:4:** Y0=A'B', Y1=A'B, Y2=AB', Y3=AB.
- **Aplicación:** decodificación de direcciones de memoria (selecciona qué chip/periférico se activa).

### Encoder / Priority Encoder
- Inverso del decodificador: 2^N entradas → N salidas.
- **Priority encoder:** codifica la entrada de mayor prioridad cuando hay múltiples activas.
- **Aplicación directa:** controlador de interrupciones del procesador (NVIC en ARM).

### Sumadores
- **Half adder:** Sum = A⊕B, Carry = A·B.
- **Full adder:** Sum = A⊕B⊕Cin, Cout = A·B + Cin·(A⊕B).
- **Ripple carry:** N full adders encadenados. Simple, lento (carry propaga por toda la cadena).
- **Carry lookahead (CLA):** calcula carries en paralelo con Generate y Propagate. Rápido, más hardware.
- **ALU:** sumador + lógica de control → AND, OR, XOR, suma, resta, comparación.

### Comparador
- Internamente hace resta A−B y evalúa flags.
- **Flags:** Zero (Z: resultado=0), Carry (C: borrow unsigned), Negative (N: MSB del resultado), Overflow (V: overflow signed).
- **Comparaciones con flags (ARM):** A==B → Z=1; A>B unsigned → C=1 y Z=0; A<B signed → N≠V.
- En C, `if (a > b)` compila a `CMP` + branch condicional que lee flags.

---

## 1.5 Circuitos Secuenciales

### Latches
- **SR:** S=1→set, R=1→reset, S=R=0→mantiene, S=R=1→prohibido.
- **D Latch:** enable=1→Q sigue a D (transparente), enable=0→Q mantiene.
- Problema: transparencia causa inestabilidad en cadenas.

### Flip-Flops
- Capturan dato solo en el **borde** del reloj (no durante el nivel).

#### Cómo funciona internamente (Master-Slave)
- Dos latches en serie con clocks invertidos.
- Clock=0: master transparente (acepta D), slave bloqueado.
- Rising edge: master se bloquea (captura D), slave se abre (pasa valor a Q).
- Clock=1: master bloqueado, D puede cambiar libremente sin afectar Q.

#### Tipos
- **D (el dominante):** Q = D en el borde. Simple, predecible, mapea a "almacenar un valor".
- **JK:** J=K=0→hold, 01→reset, 10→set, 11→toggle. Menos usado hoy — la simplicidad del D es preferible y las herramientas de síntesis optimizan alrededor del D.
- **T (Toggle):** T=1→cambia de estado en cada borde. T=0→mantiene. **No es un oscilador** — necesita reloj externo. Divide la frecuencia del reloj por 2. Base de contadores.

#### Timing Crítico
- **Setup time (t_su):** D debe estar estable ANTES del borde por este tiempo.
- **Hold time (t_h):** D debe mantenerse estable DESPUÉS del borde por este tiempo.
- Violar → **metaestabilidad** (estado intermedio indeterminado).

### Metaestabilidad y Mitigación
- No se "limpia" — se mitiga con **sincronizador de doble flip-flop:**
  ```
  Señal asíncrona → [FF1] → [FF2] → Señal sincronizada (mismo clock)
  ```
- FF1 puede entrar en metaestabilidad, pero tiene un período completo para resolverse antes de que FF2 capture.
- Probabilidad de fallo = e^(−T/τ), exponencialmente baja.
- **Triple FF** para ultra-alta confiabilidad (aviónica): 3 ciclos de latencia pero probabilidad de fallo ridículamente baja.
- **Regla:** toda señal que cruza dominio de reloj o viene del exterior → sincronizador obligatorio.

### Registros
- Grupo de flip-flops D con el mismo clock. Registro de 8 bits = 8 FFs D en paralelo.
- Los registros del procesador (R0-R15, PC, SP) son esto.

### Shift Registers
- FFs D en cadena: Q de cada uno alimenta D del siguiente.
- **SISO:** serial in, serial out (línea de retardo).
- **SIPO:** serial in, parallel out (receptor SPI/UART — reconstituye byte de bits seriales).
- **PISO:** parallel in, serial out (transmisor SPI — envía byte bit a bit).
- **PIPO:** parallel in, parallel out (registro con capacidad de shift).
- **LFSR:** shift register con retroalimentación XOR → secuencias pseudo-aleatorias (CRC, scrambling, test).
- **JTAG:** cadena de scan global para debugging de PCBs — shift register gigante que conecta todos los pines.

### Contadores
- **Asíncrono (ripple):** clock solo al primer FF, cada FF se cloquea con Q del anterior. Simple, pocos recursos, pero bits no cambian simultáneamente → glitches. Uso: solo cuando velocidad no importa.
- **Síncrono:** todos los FF reciben el mismo clock. Lógica combinacional determina toggle de cada FF (T0=1, T1=Q0, T2=Q0·Q1...). Sin glitches. Uso: casi siempre.
- **Aplicaciones:** timer/counter del procesador, PWM, watchdog timer (crítico en aviónica), prescaler, program counter.

### Máquinas de Estados Finitos (FSM)
- **Moore:** salida depende solo del estado. Más predecible.
- **Mealy:** salida depende del estado + entradas. Más rápida pero puede tener glitches.
- **Diseño:** (1) diagrama de estados, (2) asignar códigos binarios, (3) tabla de transición, (4) ecuaciones de salida, (5) implementar con FFs + lógica combinacional.
- **En software (C):** `switch-case` en ISR = FSM. Patrón fundamental en embedded.

#### Estados de Recovery
- Estado ERROR/RECOVERY para condiciones inesperadas.
- `default` en switch-case → siempre ir a ERROR.
- Watchdog/timeout por estado para prevenir deadlocks.
- Codificación one-hot con detección de estados inválidos.
- **DO-178C:** análisis exhaustivo obligatorio — todas las combinaciones estado+entrada deben tener transición definida.

### Diseño Síncrono
- Todo opera bajo reloj(es) global(es). FFs cambian solo en borde de reloj.
- **Regla:** T_clock > t_prop(FF_A) + t_delay(combinacional) + t_setup(FF_B).
- Violación = timing violation (dato incorrecto capturado).
- **¿Por qué síncrono?** Más fácil de diseñar, verificar, testear. Herramientas de timing y verificación asumen sincronía. En aviónica, predecibilidad es mandatoria.
- **Clock domains:** múltiples relojes (CPU 168MHz, APB 42MHz, RTC 32.768kHz). Dentro de un dominio: síncrono. Entre dominios: sincronizadores o FIFOs con contadores Gray.

---

## 1.6 Memorias

### Celda de Memoria — Definición Exacta
La unidad mínima que almacena 1 bit. Implementación varía:
- **SRAM:** 6 transistores (2 inversores realimentados + 2 access transistors). Biestable, retiene mientras hay alimentación.
- **DRAM:** 1 transistor + 1 capacitor. Carga = dato. Se descarga → necesita refresh (~64ms).
- **Flash:** 1 transistor con floating gate. Electrones atrapados por túnel cuántico cambian V_th.

### Tipos de Memoria
| Tipo | Tecnología | Velocidad | Volátil | Escritura | Uso principal |
|------|-----------|-----------|---------|-----------|---------------|
| SRAM | 6T | ~1ns | Sí | Ilimitada | Cache, RAM de micro |
| DRAM | 1T+1C | ~50-100ns | Sí | Ilimitada | RAM principal (DDR) |
| ROM | Conexiones fijas | Rápida | No | Ninguna | Datos fijos fábrica |
| EEPROM | Floating gate | Lenta escr. | No | ~100K-1M ciclos | Config, calibración |
| Flash | Floating gate | Media | No | ~10K-100K ciclos | Firmware, almacenamiento |

### Cómo se hace una ROM
- Decodificador de direcciones + matriz de conexiones (presente=1, ausente=0).
- Cada dirección activa una word line; las bit lines con conexión se activan.
- **ROM = tabla de verdad en hardware** → puede implementar cualquier función combinacional.
- Variantes: Mask ROM (fábrica), PROM (fusibles, una vez), EPROM (UV), EEPROM (eléctrico), Flash (bloques).

### Desgaste de Flash
- Programar/borrar fuerza electrones a través de óxido (túnel cuántico).
- Cada ciclo daña el óxido (crea "trampas" en estructura cristalina).
- Trampas acumuladas → electrones escapan o quedan atrapados permanentemente → celda falla.
- **Endurance:** 10K-100K ciclos (micro Flash NOR); 1K-100K (SSD NAND).
- **Retention:** 10-20 años inicialmente, se degrada con ciclos.
- **Lectura NO desgasta** significativamente. Solo escritura/borrado.
- **Wear leveling:** distribuir escrituras uniformemente. LittleFS para micros. SSDs lo hacen internamente.

### Jerarquía de Memoria — Computador Completo
```
Registros (SRAM, ~32, 0 wait states, ~0.5ns)
  ↓
Cache L1 (SRAM, 32-64KB, ~1-2ns)
  ↓
Cache L2 (SRAM, 256KB-1MB, ~3-10ns)
  ↓
Cache L3 (SRAM, 2-64MB compartida, ~10-30ns)
  ↓
RAM Principal (DRAM DDR4/5, 4-128GB, ~50-100ns)
  ↓
SSD (Flash NAND, 256GB-4TB, ~100μs)
  ↓
HDD (Magnético, 1-20TB, ~10ms)
```

**En microcontrolador (STM32):**
```
Registros → Flash con ART cache (código) → SRAM interna (datos) → (opcional) SDRAM/Flash externas
```

### Mapa de Memoria (ARM Cortex-M)
```
0x0000_0000  Flash (código, constantes)
0x0800_0000  System Memory (bootloader de fábrica)
0x2000_0000  SRAM (variables, stack, heap)
0x4000_0000  Periféricos APB1 (UART, I2C, SPI bajo)
0x4001_0000  Periféricos APB2 (ADC, timers, alta velocidad)
0x4002_0000  Periféricos AHB1 (GPIO, DMA)
0xA000_0000  External Memory (SDRAM, NOR Flash)
0xE000_0000  Cortex-M System (NVIC, SysTick, debug, MPU)
```

- **Memory-mapped I/O:** escribir a dirección de periférico = controlar hardware.
- **Decodificación de direcciones:** el bus determina qué dispositivo responde según el rango.
- **Linker script (.ld):** define dónde va cada sección (.text→Flash, .data→RAM, .bss→RAM).
- **Startup code:** antes de main(), copia .data de Flash a RAM y llena .bss con ceros.
- **MPU (Memory Protection Unit):** configura regiones read-only, read-write, privilegiadas. Previene corrupción accidental. Requerido por DO-178C.

### volatile — La Keyword Más Importante en Embedded
- Dice al compilador: "no optimices accesos a esta variable — cada lectura/escritura debe ser real."
- **Cuándo usar:**
  1. Registros de periféricos (memory-mapped I/O) — siempre.
  2. Variables compartidas con ISRs.
  3. Variables compartidas entre threads/tareas RTOS.
  4. Registros con side-effects al leer (read-to-clear).
- **Sin volatile:** el compilador puede cachear el valor en un registro y nunca releer de memoria → bugs silenciosos.
- **Lo que volatile NO hace:** no garantiza atomicidad (variable de 32 bits en procesador de 8 bits = 4 lecturas, ISR puede interrumpir entre medio). No garantiza ordenamiento completo entre diferentes variables volatile (usar memory barriers para eso).
- **Pregunta de entrevista #1 en embedded.** Respuesta: qué hace, cuándo se usa, qué no hace.

---

## Errores Comunes y Malentendidos

1. **Confundir binario puro con BCD.** 47 en BCD ≠ 47 en binario.
2. **Asumir que overflow signed se comporta como unsigned.** Signed overflow = UB en C.
3. **Usar `int` en lugar de `uint32_t` en embedded.** Tamaño de `int` varía por plataforma.
4. **Comparar floats con `==`.** Usar epsilon.
5. **Poner orden binario en K-maps en lugar de Gray.** La adyacencia no funciona.
6. **Olvidar wrap-around en bordes de K-maps.** El mapa es un toroide.
7. **Conectar 5V a pin de 3.3V sin level shifter.** Daña el chip.
8. **Confundir latch con flip-flop.** Latch = nivel (transparente), FF = borde.
9. **No poner volatile en registros de hardware.** Bug silencioso por optimización del compilador.
10. **Olvidar sincronizador al cruzar dominios de reloj.** Metaestabilidad.
11. **FSM sin estado de recovery / sin default en switch.** Defecto de certificación en aviónica.
12. **Asumir que lectura desgasta Flash.** Solo escritura/borrado desgasta.

---

## Referencias del Módulo

- **Morris Mano** — *Digital Design* (clásico del TEC)
- **Harris & Harris** — *Digital Design and Computer Architecture* (conecta lógica con ARM)
- **Horowitz & Hill** — *The Art of Electronics* (analógicos prácticos)
- **Patterson & Hennessy** — *Computer Organization and Design: RISC-V Edition*

---

*Siguiente: Módulo 2 — Circuitos Analógicos Esenciales (enfoque embedded)*
