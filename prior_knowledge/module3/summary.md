# Resumen Módulo 3: Sistemas Digitales Avanzados
## Preparación Embedded Software Engineer — Aeronautical SW / Joby Aviation

> Este módulo conecta la lógica digital pura (Módulo 1) con la arquitectura real de los sistemas que vas a programar. Cubre los buses, periféricos, y herramientas de debug que un embedded engineer usa todos los días.

---

## 3.1 Buses y Protocolos de Comunicación

### Conceptos Fundamentales

**Serial vs Paralelo:**
- Paralelo: múltiples bits simultáneos por cables separados. Problema de skew a alta frecuencia. Sobrevive dentro del chip (bus de datos interno).
- Serial: bits uno tras otro por un solo cable (o par diferencial). Menos pines, sin skew. Domina fuera del chip: SPI, I2C, UART, CAN, USB, Ethernet.
- Serial ganó porque a frecuencias altas, el skew entre cables paralelos es peor que la latencia de serializar.

**Duplex:**
- Simplex: una sola dirección.
- Half-duplex: bidireccional pero no simultáneo (I2C, CAN).
- Full-duplex: bidireccional simultáneo por cables separados (SPI, UART).

**Topología:**
- Master-Slave: master inicia toda comunicación. Slaves solo responden (SPI, I2C modo normal).
- Multi-Master: varios dispositivos pueden iniciar. Requiere arbitraje (CAN, I2C multi-master).

**Señalización:**
- Single-ended: voltaje medido contra GND. Simple pero susceptible a ruido (SPI, I2C, UART).
- Diferencial: dos cables (D+, D−) con señales opuestas. El receptor mide la diferencia → ruido de modo común se cancela. Robusto contra EMI (CAN, USB, RS-485, Ethernet, LVDS).

**Baud rate vs Bit rate:**
- Baud rate = símbolos/segundo. Bit rate = bits/segundo.
- En UART, SPI, I2C, CAN: baud rate ≈ bit rate (señalización binaria, 1 símbolo = 1 bit).

---

### UART (Universal Asynchronous Receiver/Transmitter)

**Características:**
- Asíncrono (sin clock compartido — cada lado usa su propio oscilador).
- 2 cables: TX y RX (cruzados entre dispositivos) + GND.
- Punto a punto (solo 2 dispositivos).
- Full-duplex.
- No tiene direccionamiento ni ACK.

**Frame 8N1 (configuración más común):**
- Idle = HIGH.
- Start bit = 1 bit LOW (señala inicio).
- Data = 8 bits, LSB primero.
- Parity = ninguna (N).
- Stop bit = 1 bit HIGH (señala fin).
- Total: 10 bits por byte → eficiencia 80%.

**Baud rates importantes:**
- 9600: GPS, sensores lentos, legacy.
- 115200: debug console, comunicación general. Throughput real = 115200/10 = 11520 bytes/s ≈ 11.25 KB/s.

**Sincronización:**
- Baud rates deben coincidir entre TX y RX. Tolerancia ±3-5%.
- El receptor detecta el flanco HIGH→LOW del start bit y muestrea en el centro de cada periodo de bit.
- Cristales de cuarzo (~50 ppm) no dan problemas. Osciladores RC internos (~1-2%) pueden fallar en transferencias largas.

**UART vs RS-232 vs RS-485 vs TTL:**
- UART = protocolo (frame: start, data, stop). Implementado en el periférico del MCU.
- TTL/CMOS levels = niveles eléctricos directos del MCU (0V / 3.3V o 5V).
- RS-232 = estándar de capa física con niveles invertidos (-3V a -15V = HIGH, +3V a +15V = LOW). Necesita transceiver (MAX232). Conector DB-9.
- RS-485 = capa física diferencial. Distancias largas (1200 m), multi-drop. Industrial.

**Errores comunes de debug UART:**
1. Baud rate no coincide → basura.
2. TX conectado a TX en vez de RX → nada.
3. Niveles lógicos incompatibles (3.3V vs 5V).
4. Configuración (8N1 vs 8E1, etc.) no coincide.

**Uso en aviónica:**
- Debug console (UART → USB converter).
- GPS (protocolo NMEA a 9600 baud).
- Módulos simples (Bluetooth, WiFi).
- NO para comunicación crítica de vuelo.

---

### SPI (Serial Peripheral Interface)

**Características:**
- Síncrono (clock generado por el master: SCLK).
- 4 líneas: SCLK, MOSI (o COPI), MISO (o CIPO), CS̄ (una por slave).
- 1 master, N slaves. Total cables = 3 + N.
- Full-duplex verdadero.
- Sin dirección (selección por CS̄ dedicado), sin ACK.

**Nomenclatura moderna:**
- MOSI → COPI (Controller Out Peripheral In).
- MISO → CIPO (Controller In Peripheral Out).
- SS̄ → CS̄ (Chip Select). Activo LOW (barra indica activo bajo).

**Mecanismo interno — Shift register circular:**
- Master y slave tienen un shift register conectados en anillo (MOSI → slave, MISO → master).
- Cada pulso de clock: ambos desplazan un bit. Después de 8 pulsos, los contenidos se intercambiaron.
- Siempre full-duplex: enviar = recibir simultáneamente. Para solo leer: enviar dummy (0x00/0xFF). Para solo escribir: ignorar lo recibido.

**Modos SPI (CPOL / CPHA):**
- CPOL (Clock Polarity): nivel del clock en idle. CPOL=0 → idle LOW. CPOL=1 → idle HIGH.
- CPHA (Clock Phase): flanco de muestreo. CPHA=0 → primer flanco (leading). CPHA=1 → segundo flanco (trailing).
- Modo 0 (CPOL=0, CPHA=0): el más común y el default. Muestreo en rising edge. Soportado por la mayoría de sensores.
- Modo 3 (CPOL=1, CPHA=1): segundo más usado. SD cards en modo SPI.
- Si un dispositivo no funciona, verificar que el modo SPI del master coincide con el datasheet del slave.

**Velocidades típicas:**
- Sensores: 1-10 MHz.
- Flash SPI: 20-80 MHz (quad SPI hasta 400 MHz).
- ADCs de alta velocidad: 20-50 MHz.
- SPI es el bus más rápido de los 4 principales.

**Ventajas:** Velocidad alta, full-duplex, simple (sin overhead de protocolo), sin pull-ups, sin conflictos de bus (CS̄ dedicado).
**Desventajas:** Muchos cables (3+N), sin ACK, sin estándar formal, solo master-slave, cables largos problemáticos (single-ended).

**Uso en aviónica:**
- Flash SPI externa (configuración, logs, firmware backup).
- ADCs de alta resolución/velocidad.
- IMUs — acelerómetros/giroscopios (MPU-6500 tiene SPI e I2C; SPI es más rápido).
- Displays/pantallas.

---

### I2C (Inter-Integrated Circuit)

**Características:**
- Síncrono (clock generado por el master: SCL).
- 2 líneas: SCL (clock), SDA (datos bidireccional) + GND, VCC.
- Multi-master, multi-slave. Hasta 127 dispositivos en un bus.
- Half-duplex (SDA es bidireccional).
- Open-drain con pull-up resistors (típico: 4.7 kΩ a 3.3V).

**Open-drain y sus implicaciones:**
- Cada dispositivo puede tirar la línea a LOW pero la suelta a HIGH por la resistencia pull-up.
- Wired-AND: si cualquier dispositivo tira a LOW, queda LOW. Solo HIGH cuando TODOS sueltan.
- Arbitraje natural: si un master envía 1 (suelta) pero lee 0 → otro master con prioridad está transmitiendo → se retira. Sin colisión.
- Clock stretching: un slave lento mantiene SCL en LOW para pedir "esperame" al master.

**Protocolo I2C:**
1. **START:** SDA baja mientras SCL está HIGH.
2. **Address byte:** 7 bits dirección + 1 bit R/W̄ (0=write, 1=read).
3. **ACK/NACK:** Receptor tira SDA a LOW (ACK) o la deja HIGH (NACK) durante un pulso de clock.
4. **Data bytes:** MSB primero. Cada byte seguido de ACK/NACK.
5. **STOP:** SDA sube mientras SCL está HIGH.

**Patrón de lectura (pregunta de entrevista):**
- Para leer un registro, se necesitan DOS fases:
  1. Write phase: START → dirección slave + W → registro a leer → ACK.
  2. Read phase: REPEATED START → dirección slave + R → dato → NACK → STOP.

**Velocidades:**
- Standard mode: 100 kHz.
- Fast mode: 400 kHz (el más usado).
- Fast-mode Plus: 1 MHz.
- High-speed: 3.4 MHz.
- Comparado con SPI (50+ MHz), I2C es significativamente más lento.

**Colisión de direcciones:**
- Cada dispositivo tiene dirección fija o parcialmente configurable (pines A0, A1, A2 modifican bits bajos).
- Si dos dispositivos iguales comparten dirección → colisión. Solución: I2C multiplexer (TCA9548A).

**Ventajas:** Solo 2 cables para muchos dispositivos, direccionamiento integrado, ACK/NACK, multi-master, estándar bien definido.
**Desventajas:** Lento vs SPI, half-duplex, pull-ups limitan velocidad, más complejo que UART/SPI, capacitancia del bus limita dispositivos (~400 pF máx).

**Uso en aviónica:**
- Sensores de baja velocidad (temperatura, presión barométrica BMP280: 0x76/0x77, humedad).
- EEPROM (calibración, configuración).
- RTC (timestamping de logs).
- Power management ICs (fuel gauges, PMIC).
- IO expanders.

---

### CAN Bus (Controller Area Network) ⭐ PRIORIDAD MÁXIMA

**Características:**
- Asíncrono (sin clock compartido, mecanismos de sincronización integrados).
- 2 cables diferenciales: CAN_H, CAN_L. Terminación 120Ω en cada extremo.
- Multi-master (todos los nodos son peers).
- Half-duplex.
- Señalización diferencial → excelente inmunidad a ruido/EMI.
- Inventado por Bosch (1986) para automóviles. Protocolo dominante en aviación moderna (ARINC 825).

**Dominante vs Recesivo — concepto clave:**
- Recesivo (bit = 1): CAN_H = CAN_L ≈ 2.5V. V_diff ≈ 0V. Estado default (nadie transmite activamente).
- Dominante (bit = 0): CAN_H ≈ 3.5V, CAN_L ≈ 1.5V. V_diff ≈ 2V. Transceiver separa las líneas activamente.
- **Dominante siempre gana:** Si un nodo transmite 0 y otro 1, el bus queda en 0. Es un wired-AND invertido.

**Frame CAN 2.0A (estándar):**
- SOF: 1 bit dominante.
- Identifier: 11 bits. **Menor ID = mayor prioridad.**
- RTR: 1 bit (0=data, 1=remote request).
- IDE: 1 bit (0=standard 11-bit, 1=extended 29-bit).
- DLC: 4 bits (Data Length Code: 0-8 bytes).
- Data: 0-8 bytes (CAN 2.0) o 0-64 bytes (CAN FD).
- CRC: 15 bits + delimitador.
- ACK: 2 bits (slot + delimitador).
- EOF: 7 bits recesivos.

**Arbitraje por prioridad (pregunta de entrevista garantizada):**
1. Múltiples nodos empiezan a transmitir simultáneamente.
2. Cada nodo monitorea el bus mientras transmite el Identifier bit por bit.
3. Si un nodo envía recesivo (1) pero lee dominante (0) → otro nodo con ID menor está ganando → se retira silenciosamente.
4. El nodo con ID más bajo siempre gana. No hay colisión ni pérdida de datos.
5. En aviónica: mensajes safety-critical (alarmas, comandos de actuadores) tienen IDs bajos para ganar siempre.

**CAN FD (Flexible Data-rate):**
- Data payload: hasta 64 bytes (vs 8 en CAN 2.0).
- Bit rate de datos: hasta 8 Mbps (arbitraje sigue a 1 Mbps máx).
- Retrocompatible en fase de arbitraje.

**5 mecanismos de detección de errores:**
1. Bit monitoring: transmite y verifica lo que lee del bus.
2. Bit stuffing: después de 5 bits iguales consecutivos, inserta bit opuesto.
3. CRC check: 15 bits.
4. ACK check: al menos un nodo debe hacer ACK.
5. Frame check: verificación de formato.

**Error confinement (auto-aislamiento):**
- Error Active (0-127 errores): funciona normal.
- Error Passive (128-255): transmite con restricciones.
- Bus Off (>255): se desconecta del bus.
- Crítico en aviónica: nodo defectuoso se auto-aísla.

**Velocidades y distancias:**
- 1 Mbps → ~40 m. 500 kbps → ~100 m. 125 kbps → ~500 m. 50 kbps → ~1000 m.
- Relación inversa: a mayor velocidad, menor distancia (velocidad de propagación de la señal).

**ARINC 825 (CAN en aviación):**
- Define mapeo de IDs CAN para sistemas avionics.
- Prioridades estándar para tipos de mensajes.
- Formato de datos estandarizado.
- Redundancia (buses CAN duales).
- Otros protocolos de aviación: ARINC 429 (legacy, unidireccional), AFDX (Ethernet-based).

**Ventajas:** Diferencial (inmune a EMI), multi-master con arbitraje sin colisiones, priorización por ID, 5 mecanismos de detección de errores + auto-confinement, estándar robusto (40+ años), solo 2 cables.
**Desventajas:** Velocidad limitada (1 Mbps / 8 Mbps FD), payload pequeño (8/64 bytes), latencia no determinista para baja prioridad, necesita transceivers externos, más complejo.

---

### Tabla Comparativa Rápida: UART vs SPI vs I2C vs CAN

| | UART | SPI | I2C | CAN |
|---|---|---|---|---|
| Cables | 2 (TX, RX) | 3 + N CS̄ | 2 (SCL, SDA) | 2 (CAN_H, CAN_L) |
| Clock | Asíncrono | Síncrono | Síncrono | Asíncrono |
| Duplex | Full | Full | Half | Half |
| Topología | Punto a punto | 1 master, N slaves | Multi-master, multi-slave | Multi-master (peers) |
| Velocidad | ~115 kbps típico | Hasta ~50+ MHz | 100-400 kHz típico | 1 Mbps / 8 Mbps FD |
| Señal | Single-ended | Single-ended | Open-drain + pull-up | Diferencial |
| Dirección | Ninguna | CS̄ por slave | 7-bit address | Message ID (11/29 bits) |
| ACK | No | No | Sí (por byte) | Sí (por frame) |
| Detección errores | Paridad (opcional) | Ninguna | ACK/NACK | 5 mecanismos + CRC |
| Distancia | ~15 m (RS-232) | ~1 m (PCB) | ~1 m (PCB) | Hasta 1000 m |
| Dispositivos | 2 | Limitado por CS̄ | Hasta 127 | Hasta ~110 nodos |
| Uso principal | Debug, GPS | Flash, ADC, displays | Sensores, EEPROM, RTC | Aviación, automotive |

**Regla de entrevista para elegir protocolo:**
- ¿2 dispositivos, simple, debug? → UART.
- ¿Alta velocidad, pocos slaves en PCB? → SPI.
- ¿Muchos dispositivos, pocos pines, baja velocidad? → I2C.
- ¿Ambiente ruidoso, seguridad crítica, múltiples nodos? → CAN.

---

## 3.2 Interfaz con Periféricos

### GPIO (General Purpose Input/Output)

**Registros principales (ARM Cortex-M / STM32):**
- GPIOx_MODER: Input (00), Output (01), Alternate Function (10), Analog (11). 2 bits por pin.
- GPIOx_OTYPER: Push-pull (0) o Open-drain (1).
- GPIOx_OSPEEDR: Low, Medium, High, Very High (afecta slew rate y EMI).
- GPIOx_PUPDR: None (00), Pull-up (01), Pull-down (10).
- GPIOx_IDR: Input Data Register (solo lectura, lee estado de pines).
- GPIOx_ODR: Output Data Register (lee/escribe estado de salida).
- GPIOx_BSRR: Bit Set/Reset Register. Setea o limpia bits individuales **atómicamente**.

**BSRR es crítico:**
- Escribir en ODR con read-modify-write → una interrupción en medio puede corromper otros bits.
- BSRR es atómico: escribir 1 en bit N → set pin N. Escribir 1 en bit (N+16) → reset pin N. Los demás bits no se afectan.

**Alternate Function (AF):**
- La mayoría de pines pueden funcionar como periféricos (UART TX, SPI MOSI, I2C SCL, etc.).
- Se configura con MODER = 10 y selección de AF (AF0-AF15) en GPIOx_AFR[0/1].
- Cada pin tiene un set fijo de funciones alternativas definidas en el datasheet.

**Error #1 de principiantes:**
- Olvidar habilitar el clock del periférico GPIO vía RCC (Reset and Clock Control) antes de configurarlo. Sin clock, el periférico no responde y los registros leen cero.

---

### Interrupciones

**Concepto:**
- Señal al procesador: "dejá lo que estás haciendo, atendé esto, y volvé".
- Alternativa a polling (revisar constantemente). Polling bloquea el CPU; interrupciones lo liberan.

**NVIC (Nested Vectored Interrupt Controller) en ARM Cortex-M:**
- Nested: interrupción de mayor prioridad puede interrumpir a otra en ejecución.
- Vectored: cada fuente de interrupción tiene dirección fija en la tabla de vectores (Flash, dirección 0x00000000).
- Prioridad: número menor = mayor prioridad. Típicamente 4 bits → 16 niveles. Se divide en preemption priority y sub-priority.
- IRQ number: cada fuente tiene número único (EXTI0=6, TIM2=25, USART1=37, SPI1=53, etc.).
- ISR / IRQ Handler: la función que se ejecuta al ocurrir la interrupción.

**Flujo de interrupción en Cortex-M:**
1. Evento dispara IRQ.
2. Hardware guarda contexto automáticamente (push al stack): R0-R3, R12, LR, PC, xPSR.
3. NVIC resuelve prioridad y busca dirección en la tabla de vectores.
4. ISR ejecuta (clear flag + acción mínima).
5. Hardware restaura contexto (pop del stack).
6. main() continúa donde quedó.
- Latencia: 12 ciclos (Cortex-M4). Todo el push/pop es automático en hardware.

**Reglas de oro para ISRs (pregunta de entrevista):**
1. ISR lo más corta posible — setear flag, procesar en main loop o tarea RTOS.
2. No usar delay ni funciones bloqueantes (HAL_Delay(), printf(), malloc()).
3. Siempre limpiar el pending flag. Si no se limpia → ISR se ejecuta infinitamente.
4. Variables compartidas con ISR deben ser `volatile`.
5. Operaciones atómicas o deshabilitar interrupciones brevemente para secciones críticas.
6. En aviónica DO-178C: WCET determinista, sin recursión, cobertura MC/DC.

**Optimizaciones del NVIC:**
- Tail-chaining: si hay otra IRQ pendiente al terminar una ISR, no restaura y re-guarda contexto → salta directo. Ahorra 12 ciclos → solo 6 de transición.
- Late arrival: si llega IRQ de mayor prioridad durante el push de contexto, cambia el vector destino "en vuelo".

---

### Timers y PWM

**Timer = contador de hardware que cuenta pulsos de clock.**

**Usos principales:**
- Generar delays precisos sin bloquear CPU.
- Medir tiempo entre eventos (input capture).
- Generar señales periódicas (output compare, PWM).
- Disparar eventos periódicamente (interrupciones, trigger ADC).
- Contar eventos externos (encoder, pulse counting).

**Parámetros esenciales:**
- PSC (Prescaler): divide la frecuencia del clock. f_timer = f_clock / (PSC + 1).
- ARR (Auto-Reload Register): valor máximo del contador. Al llegar a ARR, resetea a 0 y genera evento "update".
- CNT (Counter): valor actual.

**Fórmulas fundamentales del timer:**
```
f_overflow = f_clock / ((PSC + 1) × (ARR + 1))
T_overflow = (PSC + 1) × (ARR + 1) / f_clock
```

**Ejemplos resueltos (f_clock = 84 MHz):**
1. Interrupción cada 500 ms: PSC=8399 → f_timer=10 kHz, ARR=4999 → T = 5000/10000 = 0.5 s ✓
2. Resolución de 1 µs: PSC = (84M/1M)−1 = 83 → f_timer = 1 MHz, 1 tick = 1 µs ✓
3. Señal de 1 kHz: PSC=83 → f_timer=1 MHz, ARR=999 → f = 1M/1000 = 1 kHz ✓

**PWM (Pulse Width Modulation):**
- Señal cuadrada con duty cycle (% de tiempo en HIGH) controlable.
- Timer cuenta 0 → ARR. CCR (Capture/Compare Register) define punto de comparación.
- CNT < CCR → HIGH. CNT ≥ CCR → LOW.
- Duty cycle = CCR / (ARR + 1) × 100%.
- f_PWM = f_clock / ((PSC + 1) × (ARR + 1)).

**Ejemplo PWM:** Servo a 50 Hz, posición central (7.5% duty, pulso 1.5 ms).
- PSC=83 → f_timer=1 MHz. ARR=19999 → T=20 ms → f=50 Hz ✓.
- CCR = 0.075 × 20000 = 1500 → HIGH por 1.5 ms ✓.
- Servos: 1 ms (0°) a 2 ms (180°). ESCs de motores brushless: mismo rango.

**Nota sobre timers en APB:**
- Si el prescaler del bus APB es >1, el clock del timer se multiplica por 2 internamente.
- Ejemplo: PCLK1 = 42 MHz con prescaler /4, pero timers en APB1 reciben 84 MHz.

---

### DMA (Direct Memory Access)

**Concepto:**
- Controlador de hardware que transfiere datos entre memoria y periféricos (o memoria-memoria) sin intervención del CPU.
- CPU configura transferencia, la arranca, y queda libre para hacer otra cosa.
- DMA avisa al terminar con una interrupción.

**Sin DMA vs con DMA:**
- Sin DMA (polling): CPU lee dato del periférico → escribe en buffer → repite. CPU 100% ocupado.
- Con DMA: CPU dice "transferí 1000 muestras del ADC al buffer" y se va. DMA hace las 1000 transferencias solo.

**Modos:**
- Normal: transfiere N datos y para. Hay que reconfigurar para otra transferencia.
- Circular: al llegar al final del buffer, vuelve al inicio automáticamente. Perfecto para ADC continuo, audio.

**Double buffering:**
- DMA alterna entre dos buffers. Mientras DMA llena buffer A, CPU procesa buffer B. Cero muestras perdidas.

**Prioridades y conflictos de bus:**
- Múltiples streams DMA competen por el bus AHB (Very High, High, Medium, Low).
- DMA y CPU comparten el mismo bus → DMA constante puede ralentizar al CPU (bus stalling).
- En sistemas críticos, presupuestar ancho de banda del bus.

**Relevancia en aviónica:** Esencial para adquirir datos de sensores a alta velocidad sin perder deadlines del RTOS.

---

### Polling vs Interrupciones vs DMA — Cuándo usar cada uno

| Método | Cuándo usar | Ejemplo | CPU overhead |
|---|---|---|---|
| Polling | Evento raro/simple, o busy-wait durante inicialización | Leer botón, LED toggle, esperar PLL lock | 100% durante la espera |
| Interrupciones | Eventos asincrónicos con poco dato, reacción rápida | UART byte recibido, GPIO edge, timer expira | Bajo (solo ISR entry/exit) |
| DMA | Transferencias masivas, CPU no necesita intervenir por byte | ADC stream 1000 muestras, SPI bulk, audio buffer | ~Cero (solo configuración) |

En la práctica, la mayoría de sistemas usan los tres simultáneamente: DMA para ADC, interrupciones para UART de debug, polling para LED heartbeat.

---

## 3.3 FPGA vs ASIC vs Microcontrolador

### Tres opciones de implementación digital

**Microcontrolador (MCU):**
- Procesador + memoria + periféricos en un chip. Ejecuta instrucciones secuencialmente.
- Flexibilidad por software (cambiás el programa). Desarrollo rápido en C/C++.
- Bajo costo unitario (~$1-$20). Rendimiento limitado por ejecución secuencial.
- Analogía: un chef hábil con una sola mano — cualquier receta, un paso a la vez.

**FPGA (Field-Programmable Gate Array):**
- Miles/millones de bloques lógicos configurables (CLBs) con interconexiones programables.
- No ejecuta instrucciones — describís hardware en HDL (VHDL/Verilog) y el FPGA lo implementa como circuito.
- Paralelismo real. Desarrollo lento y costoso. Reprogramable en campo.
- Costo medio-alto (~$10-$10,000+). NRE = $0 (comprás el chip y lo programás).
- Analogía: cocina con miles de estaciones independientes, todas trabajan en paralelo.

**ASIC (Application-Specific Integrated Circuit):**
- Chip diseñado desde cero para una función específica. Circuito "quemado" en silicio. No reprogramable.
- Máxima eficiencia (velocidad, consumo, área). Desarrollo muy lento.
- NRE altísimo ($1M-$100M para máscaras). Costo unitario bajísimo en volumen alto (~$0.10).
- Analogía: máquina de fábrica para un solo producto — la más eficiente, pero cambiar = tirar y rehacer.

### Dentro de un FPGA

- **LUT (Look-Up Table):** Tabla de verdad en SRAM. LUT de 4 entradas = 16 bits SRAM = cualquier función booleana de 4 variables. Es un MUX programable (concepto de Módulo 1).
- **CLB (Configurable Logic Block):** Unidad básica. Contiene LUTs, flip-flops, carry chain, MUXes.
- **Routing matrix:** Red de interconexiones programables. ~80% del área del chip.
- **Bloques dedicados ("hard"):** Block RAM, DSP slices (multiplicadores), PLLs, transceivers serial, procesadores ARM embebidos (Xilinx Zynq, Intel Cyclone V SoC).

### FPGAs en aviónica

1. Paralelismo real: procesar 32 canales de ADC simultáneamente.
2. Latencia determinista: sin cache misses, sin interrupciones. Crítico para control de vuelo.
3. Interfaces custom: protocolos propietarios, timing exótico.
4. TMR (Triple Modular Redundancy): triplicar circuito + voter. Si un módulo falla, los otros dos ganan.
5. Certificación DO-254 (equivalente de DO-178C para hardware). FPGA permite actualizar post-fabricación.

### DO-254 (conceptual)

| DO-178C (Software) | DO-254 (Hardware programable) |
|---|---|
| Requirements → Code | Requirements → HDL |
| Code review | HDL review |
| Unit testing | Simulation |
| Cobertura MC/DC | Cobertura toggle/statement |
| Configuration management | Configuration management |

Para la entrevista: "DO-178C para software, DO-254 para hardware programable".

---

## 3.4 Timing y Sincronización Avanzada

### Fuentes de Clock (ARM Cortex-M / STM32)

- **HSE** (High-Speed External): cristal de cuarzo externo, típico 8 o 25 MHz. Precisión ~20 ppm. Fuente más precisa.
- **HSI** (High-Speed Internal): oscilador RC interno, típico 8-16 MHz. Precisión ~1-2%. Arranca rápido.
- **LSE** (Low-Speed External): cristal 32.768 kHz para RTC. 32768 = 2¹⁵ → contador de 15 bits da exactamente 1 Hz.
- **LSI** (Low-Speed Internal): ~32 kHz RC. Para watchdog independiente.

### PLL (Phase-Locked Loop)

Circuito que multiplica la frecuencia del clock. Convierte 8 MHz del cristal externo en 168 MHz para el CPU.

**Configuración típica STM32F4:**
```
HSE = 8 MHz → ÷M=8 → 1 MHz → ×N=336 → 336 MHz (VCO) → ÷P=2 → 168 MHz (SYSCLK)
                                                        → ÷Q=7 → 48 MHz (USB)
```

**Fórmula:** f_out = (HSE / M) × N / P

**Importante:** PLL necesita tiempo para engancharse (lock). Startup code espera PLL_RDY antes de cambiar SYSCLK. Usar PLL antes de lock → clock inestable → comportamiento errático.

### Bus Prescalers

```
SYSCLK = 168 MHz
├── AHB (/1) → HCLK = 168 MHz (CPU, memoria, DMA)
│   ├── APB2 (/2) → PCLK2 = 84 MHz (periféricos rápidos: SPI1, USART1)
│   └── APB1 (/4) → PCLK1 = 42 MHz (periféricos lentos: I2C, USART2)
└── SysTick (/8 opcional)
```

**Trampa:** Si prescaler APB > 1, el clock de los timers se multiplica por 2. PCLK1=42 MHz → timers en APB1 reciben 84 MHz.

### Jitter

Variación no deseada en el timing de los flancos del clock.

**Impacto:**
- ADC: jitter del clock del ADC se traduce directamente en ruido. 100 ps de jitter en ADC de 14 bits a 10 MHz → pérdida de 2-3 bits ENOB.
- Comunicación serial: jitter excesivo puede violar setup/hold time.
- Aviónica: estándares de timing estrictos.

**Fuentes:** Ruido de fuente de alimentación (por eso LDO para PLLs), ruido del oscilador, PLL, EMI.

### Clock Domain Crossing (CDC) — Ampliación de Módulo 1

**Ocurre en la práctica:**
- MCU (168 MHz) leyendo periférico I2C (400 kHz).
- CPU core (168 MHz) comunicándose con bloque USB (48 MHz).
- FPGA con múltiples dominios internos.
- Dos MCUs comunicándose por SPI con clocks independientes.

**Soluciones para datos multi-bit** (doble FF no basta — cada bit puede resolverse en ciclo diferente):
1. **Gray code:** Solo un bit cambia a la vez → peor caso = leer valor viejo o nuevo, nunca basura.
2. **Handshake protocol:** Señales req/ack entre dominios, sincronizadas con doble FF.
3. **Asynchronous FIFO:** FIFO con punteros de escritura/lectura en código Gray. Solución estándar.

**En aviónica certificada:** TODO CDC debe ser documentado y verificado. CDC no sincronizado = bug latente que causa fallas intermitentes imposibles de debuggear.

---

## 3.5 Testabilidad y Debug: JTAG / SWD

### JTAG (IEEE 1149.1 — Joint Test Action Group)

**Uso original — Boundary Scan:**
- Cada pin del chip tiene una celda de boundary scan (flip-flop) conectada en cadena.
- Permite capturar estado de todos los pines, forzar valores, verificar conectividad.
- Resuelve el problema de testear BGAs donde los pines están debajo del chip.

**Señales JTAG (4 + opcional):**
- TCK (Test Clock): clock de la interfaz.
- TMS (Test Mode Select): controla máquina de estados del TAP controller.
- TDI (Test Data In): datos hacia el chip.
- TDO (Test Data Out): datos desde el chip.
- TRST̄ (Test Reset, opcional): reset del TAP controller.

**Daisy-chain:** TDO de un chip → TDI del siguiente. Un solo conector JTAG accede a todos los chips de la PCB.

**Capacidades de JTAG:**
- Boundary scan (test de soldadura PCB).
- Debug de CPU (breakpoints, single-step, memory access).
- Programación de Flash del MCU.
- Programación de bitstream del FPGA.

### SWD (Serial Wire Debug) — Alternativa ARM

**Señales (solo 2):**
- SWDIO (Serial Wire Data I/O): bidireccional.
- SWCLK (Serial Wire Clock).

**Funcionalidad:** Idéntica a JTAG para debug (breakpoints, single-step, memory read/write, Flash programming). NO soporta boundary scan ni daisy-chain. Solo ARM.

**En la práctica:** Debug probes modernos (J-Link, ST-Link) soportan ambos. Conector ARM de 20 pines tiene ambas interfaces. SWD es más conveniente para debug de un solo Cortex-M (menos pines, mismo debug power).

### Capacidades de Debug (JTAG/SWD)

1. **Breakpoints:** Parar ejecución en instrucción específica. Hardware breakpoints (4-8, para Flash). Software breakpoints (ilimitados, para RAM).
2. **Watchpoints:** Parar cuando se lee/escribe una dirección de memoria. Invaluable para encontrar corrupción de variables.
3. **Single-step:** Ejecutar una instrucción a la vez.
4. **Register inspection:** Leer/escribir R0-R12, SP, LR, PC, xPSR.
5. **Memory read/write:** Cualquier dirección (RAM, Flash, periféricos).
6. **ITM (Instrumentation Trace Macrocell):** Printf por SWD sin usar UART.
7. **ETM (Embedded Trace Macrocell):** Traza completa de instrucciones ejecutadas (requiere hardware de traza dedicado).

### BIST (Built-In Self-Test)

- Lógica de auto-test que verifica hardware al encender (Power-On BIST) o periódicamente.
- Ejemplo: ADC con modo test que conecta internamente a referencia de voltaje conocida y verifica lectura.
- Como Embedded SW Engineer, escribís el código que invoca BIST al startup y maneja resultados (pass/fail).

---

## Glosario de Acrónimos y Nomenclatura — Módulo 3

| Acrónimo | Significado | Contexto |
|---|---|---|
| UART | Universal Asynchronous Receiver/Transmitter | Protocolo serial asíncrono |
| SPI | Serial Peripheral Interface | Protocolo serial síncrono de alta velocidad |
| I2C | Inter-Integrated Circuit | Protocolo serial síncrono de 2 cables |
| CAN | Controller Area Network | Bus diferencial para automotive/aviación |
| MOSI / COPI | Master Out Slave In / Controller Out Peripheral In | Línea de datos master→slave en SPI |
| MISO / CIPO | Master In Slave Out / Controller In Peripheral Out | Línea de datos slave→master en SPI |
| CS̄ / SS̄ | Chip Select / Slave Select | Línea de selección de slave en SPI (activo LOW) |
| SCLK | Serial Clock | Clock de SPI |
| SCL | Serial Clock Line | Clock de I2C |
| SDA | Serial Data Line | Datos de I2C |
| CPOL | Clock Polarity | Nivel idle del clock en SPI |
| CPHA | Clock Phase | Flanco de muestreo en SPI |
| ACK / NACK | Acknowledge / Not Acknowledge | Confirmación en I2C y CAN |
| CAN_H / CAN_L | CAN High / CAN Low | Par diferencial del bus CAN |
| CAN FD | CAN Flexible Data-rate | Extensión de CAN con 64 bytes y 8 Mbps |
| ARINC 825 | Aeronautical Radio Inc. standard 825 | CAN bus para aviación |
| GPIO | General Purpose Input/Output | Pines digitales configurables del MCU |
| MODER | Mode Register | Registro de configuración de modo de GPIO |
| BSRR | Bit Set/Reset Register | Registro atómico de set/reset de GPIO |
| ODR | Output Data Register | Registro de salida de GPIO |
| IDR | Input Data Register | Registro de lectura de GPIO |
| AF | Alternate Function | Función alternativa de un pin GPIO |
| RCC | Reset and Clock Control | Periférico que habilita clocks en STM32 |
| NVIC | Nested Vectored Interrupt Controller | Controlador de interrupciones de ARM Cortex-M |
| ISR / IRQHandler | Interrupt Service Routine | Función que se ejecuta al ocurrir interrupción |
| WCET | Worst-Case Execution Time | Tiempo máximo de ejecución (crítico en DO-178C) |
| PSC | Prescaler | Divisor de frecuencia del timer |
| ARR | Auto-Reload Register | Valor máximo del contador del timer |
| CNT | Counter | Valor actual del contador del timer |
| CCR | Capture/Compare Register | Registro de comparación para PWM |
| PWM | Pulse Width Modulation | Señal cuadrada con duty cycle controlable |
| DMA | Direct Memory Access | Transferencia de datos sin CPU |
| FPGA | Field-Programmable Gate Array | Chip de lógica reconfigurable |
| ASIC | Application-Specific Integrated Circuit | Chip diseñado para función específica |
| CLB | Configurable Logic Block | Unidad básica del FPGA |
| LUT | Look-Up Table | Tabla de verdad programable (base del FPGA) |
| HDL | Hardware Description Language | Lenguaje de descripción de hardware (VHDL, Verilog) |
| TMR | Triple Modular Redundancy | Triplicar circuito + voter para tolerancia a fallos |
| NRE | Non-Recurring Engineering | Costo de diseño y fabricación del primer chip |
| DO-254 | Design Assurance Guidance for Airborne Electronic Hardware | Estándar de certificación de hardware para aviación |
| HSE / HSI | High-Speed External / Internal | Fuentes de clock del MCU |
| LSE / LSI | Low-Speed External / Internal | Fuentes de clock de baja velocidad |
| PLL | Phase-Locked Loop | Multiplicador de frecuencia de clock |
| VCO | Voltage-Controlled Oscillator | Oscilador interno del PLL |
| AHB | Advanced High-performance Bus | Bus principal del MCU (CPU, DMA, memoria) |
| APB1 / APB2 | Advanced Peripheral Bus 1/2 | Buses de periféricos (lentos / rápidos) |
| CDC | Clock Domain Crossing | Cruce de señales entre dominios de clock |
| JTAG | Joint Test Action Group (IEEE 1149.1) | Interfaz de test y debug |
| SWD | Serial Wire Debug | Interfaz de debug ARM de 2 cables |
| TAP | Test Access Port | Puerto de acceso de JTAG |
| TCK / TMS / TDI / TDO | Test Clock / Mode Select / Data In / Data Out | Señales del bus JTAG |
| SWDIO / SWCLK | Serial Wire Data I/O / Clock | Señales de SWD |
| DAP | Debug Access Port | Puerto de debug ARM (accesible por JTAG o SWD) |
| ITM | Instrumentation Trace Macrocell | Printf por SWD sin UART |
| ETM | Embedded Trace Macrocell | Traza completa de instrucciones |
| BIST | Built-In Self-Test | Auto-test de hardware al encender |
| RS-232 | Recommended Standard 232 | Estándar de capa física con niveles invertidos |
| RS-485 | Recommended Standard 485 | Estándar de capa física diferencial, multi-drop |
| TTL | Transistor-Transistor Logic | Niveles lógicos de 0V/5V |
| BGA | Ball Grid Array | Encapsulado de chip con pines debajo |

---

## Referencias Bibliográficas — Módulo 3

- **Harris & Harris** — *Digital Design and Computer Architecture* (2nd ed). Capítulos sobre buses, I/O, y sistemas digitales completos. Conecta lógica digital con ARM.
- **Joseph Yiu** — *The Definitive Guide to ARM Cortex-M3/M4 Processors*. Referencia definitiva para NVIC, interrupciones, SysTick, debug, mapa de memoria, y periféricos.
- **Jonathan Valvano** — *Embedded Systems: Introduction to ARM Cortex-M Microcontrollers* (5th ed). GPIO, timers, UART, SPI, I2C con código práctico en C para TM4C123.
- **Elecia White** — *Making Embedded Systems* (O'Reilly). Visión práctica de debug, buses, DMA, y diseño de sistemas embebidos.
- **CAN specification** — Bosch CAN 2.0A/2.0B specification. Documento original del protocolo.
- **ARINC 825** — Aeronautical Radio Inc. *General Standardization of CAN Bus Protocol for Airborne Use*. Estándar de CAN para aviación.
- **STM32 Reference Manual** (RM0090 para STM32F4). Documentación completa de todos los periféricos (GPIO, timers, DMA, USART, SPI, I2C, CAN).
- **IEEE 1149.1** — Standard for Test Access Port and Boundary-Scan Architecture (JTAG).
- **Pong P. Chu** — *FPGA Prototyping by VHDL Examples*. Introducción conceptual a FPGAs para ingenieros de software.

---

*Resumen generado al cierre del Módulo 3 — Mayo 2026*
